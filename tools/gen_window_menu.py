# Generate C for window.c's debug menu tables (.data 0x245E4-0x2D2B0) from the target asm.
# usage: gen_window_menu.py <data.s> <out.inc> <known-decls-file...>
import os, sys, re, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dparse import parse

START, END = 0x245E4, 0x2D2B0
objs = parse(sys.argv[1])
outpath = sys.argv[2]
known_src = ''.join(open(p).read() for p in sys.argv[3:])

byname = {o['name']: o for o in objs}
inrange = [o for o in objs if START <= o['addr'] < END]

def words(o):
    ws = []
    for t, v in o['items']:
        if t == '4byte':
            ws.append(v)
        elif t in ('string', 'byte', '2byte'):
            return None
        else:
            raise Exception((o['name'], t))
    return ws

def is_str(o):
    return any(t == 'string' for t, v in o['items'])

def strval(o):
    s = ''.join(v for t, v in o['items'] if t == 'string')
    return eval(s).encode('latin1')

# ---- split merged objects into pieces --------------------------------------------------
pieces = []  # dict(addr, name, kind, words / bytes)
for o in inrange:
    if o['name'].startswith('gap_') or o['name'].startswith('jumptable_'):
        continue
    if is_str(o):
        pieces.append(dict(addr=o['addr'], name=o['name'], kind='str', val=strval(o)))
        continue
    ws = words(o)
    if ws is None:
        raise Exception('odd object ' + o['name'])
    if ws and all(w.startswith('0x') for w in ws):
        b = b''.join(struct.pack('>I', int(w, 16)) for w in ws)
        core = b.rstrip(b'\0')
        if len(core) >= 4 and b'\0' not in core and sum(c >= 0x80 for c in core) * 2 >= len(core):
            try:
                core.decode('shift_jis')
                pieces.append(dict(addr=o['addr'], name=o['name'], kind='str', val=core, pad=len(b) - len(core) - 1))
                continue
            except Exception:
                pass
        if len(ws) > 4 and len(ws) % 4 == 0:
            for i in range(0, len(ws), 4):
                a = o['addr'] + 4 * i
                pieces.append(dict(addr=a, name=o['name'] if i == 0 else 'lbl_1_data_%X' % a, kind='raw', w=ws[i:i + 4]))
            continue
    pieces.append(dict(addr=o['addr'], name=o['name'], kind='raw', w=ws))

# reference map (internal names -> how they're referenced)
addr_of = {}
for p in pieces:
    addr_of[p['name']] = p['addr']

def is_float_word(v):
    if not v.startswith('0x'):
        return False
    x = int(v, 16)
    if x == 0:
        return False
    e = (x >> 23) & 0xFF
    return 100 < e < 160

# identify windows and items
kinds = {}
def mark(name, kind):
    if name in addr_of:
        old = kinds.get(name)
        if old and old != kind:
            print('conflict', name, old, kind)
        kinds[name] = kind

# a window: 0x2C bytes whose word 4 points to an object made of 0x18-byte items ending with type 0x24
def looks_items(name):
    p = next((q for q in pieces if q['name'] == name), None)
    if not p or p['kind'] != 'raw':
        return False
    ws = p['w']
    for i in range(0, len(ws) - 5, 6):
        if ws[i] == '0x00000024':
            return True
    return False

new = []
for p in pieces:
    if p['kind'] != 'raw':
        new.append(p)
        continue
    ws = p['w']
    # split window templates (+ tails)
    if len(ws) >= 11 and not ws[4].startswith('0x') and looks_items(ws[4].split('+')[0]):
        new.append(dict(addr=p['addr'], name=p['name'], kind='window', w=ws[:11]))
        rest = ws[11:]
        a = p['addr'] + 0x2C
        if a == 0x2CDD4:
            new.append(dict(addr=a, name='lbl_1_data_%X' % a, kind='buf', w=rest[:16]))
            rest = rest[16:]
            a += 0x40
        while rest:
            new.append(dict(addr=a, name='lbl_1_data_%X' % a, kind='raw', w=rest[:4] if len(rest) >= 4 else rest))
            a += 4 * min(4, len(rest))
            rest = rest[4:]
        continue
    # items arrays: cut after the END entry
    if len(ws) >= 6 and looks_items(p['name']):
        n = 0
        while n + 6 <= len(ws):
            n += 6
            if ws[n - 6] == '0x00000024':
                break
        new.append(dict(addr=p['addr'], name=p['name'], kind='items', w=ws[:n]))
        rest = ws[n:]
        a = p['addr'] + 4 * n
        while len(rest) >= 11 and not rest[4].startswith('0x'):
            new.append(dict(addr=a, name='lbl_1_data_%X' % a, kind='window', w=rest[:11]))
            rest = rest[11:]
            a += 0x2C
        while rest:
            new.append(dict(addr=a, name='lbl_1_data_%X' % a, kind='raw', w=rest[:4]))
            a += 4 * min(4, len(rest))
            rest = rest[4:]
        continue
    new.append(p)
pieces = new
addr_of = {p['name']: p['addr'] for p in pieces}
piece_by_name = {p['name']: p for p in pieces}

# limits types from item references
INT_TYPES = {1, 2, 3, 4, 5, 6, 10, 14, 15, 16, 17, 18, 19, 27}
FLOAT_TYPES = {7, 20}
for p in pieces:
    if p['kind'] != 'items':
        continue
    ws = p['w']
    for i in range(0, len(ws), 6):
        t = int(ws[i], 16) & 0xFF
        ptr, lim = ws[i + 4], ws[i + 5]
        if not lim.startswith('0x') and lim in piece_by_name:
            if t in FLOAT_TYPES:
                mark(lim, 'flim')
            elif t in INT_TYPES:
                mark(lim, 'ilim')
            elif t == 25:
                mark(lim, 'strtab')
        if not ptr.startswith('0x') and ptr in piece_by_name:
            if t == 26:
                mark(ptr, 'strtab')

for p in pieces:
    if p['kind'] == 'raw':
        k = kinds.get(p['name'])
        if k:
            p['kind'] = k
        elif len(p['w']) == 4 and all(w.startswith('0x') for w in p['w']):
            p['kind'] = 'flim' if is_float_word(p['w'][1]) or is_float_word(p['w'][2]) else 'ilim'
        elif all(not w.startswith('0x') for w in p['w']):
            p['kind'] = 'strtab'

# ---- string placement check ---------------------------------------------------------------
first_ref = {}
order = [p['name'] for p in pieces]
for p in pieces:
    if p['kind'] == 'str':
        continue
    for w in p.get('w', []):
        n = w.split('+')[0]
        if n in piece_by_name and piece_by_name[n]['kind'] == 'str' and n not in first_ref:
            first_ref[n] = p['name']
problems = []
pending = []
for p in pieces:
    if p['kind'] == 'str':
        pending.append(p['name'])
        continue
    for s in pending:
        if first_ref.get(s) != p['name']:
            problems.append((s, first_ref.get(s), p['name']))
    pending = [s for s in pending if first_ref.get(s) != p['name'] and False]
for pr in problems:
    print('STRING ORDER', pr)

# ---- emit ------------------------------------------------------------------------------
externs = {}
def cstr(b):
    try:
        s = b.decode('shift_jis')
        if s.encode('shift_jis') != b:
            raise ValueError
    except Exception:
        s = None
    if s is None:
        return '"' + ''.join('\\x%02X' % c for c in b) + '"'
    out = ''
    for ch in s:
        if ch == '\\':
            out += '\\\\'
        elif ch == '"':
            out += '\\"'
        elif ch == '\n':
            out += '\\n'
        elif ord(ch) < 0x20:
            out += '\\x%02X' % ord(ch)
        else:
            out += ch
    return '"' + out + '"'

def ref(w, ctype='void *'):
    if w.startswith('0x'):
        v = int(w, 16)
        return '0x%X' % v if v > 9 else str(v)
    n, _, off = w.partition('+')
    off = int(off, 16) if off else 0
    if n in piece_by_name:
        p = piece_by_name[n]
        if p['kind'] == 'str':
            assert off == 0
            return cstr(p['val'])
        base = '&' + n if p['kind'] in ('window',) else n
        if p['kind'] in ('ilim', 'flim'):
            base = '&' + n
    else:
        if not re.search(r'\b%s\b' % re.escape(n), known_src):
            externs[n] = True
        base = '&' + n
    if off:
        return '(void *)((u8 *)%s + 0x%X)' % (base, off)
    return base

def f32(w):
    x = int(w, 16)
    f = struct.unpack('>f', struct.pack('>I', x))[0]
    r = repr(f)
    if struct.unpack('>I', struct.pack('>f', float(r)))[0] != x:
        raise Exception(w)
    s = '%gf' % f
    if struct.unpack('>I', struct.pack('>f', float(s[:-1])))[0] == x:
        return s if '.' in s or 'e' in s else s[:-1] + '.0f'
    return r + 'f'

def sint(w):
    x = int(w, 16)
    if x >= 0x80000000:
        x -= 1 << 32
    return str(x) if -10 < x < 10 else ('-0x%X' % -x if x < 0 else '0x%X' % x)

lines = []
for p in pieces:
    k = p['kind']
    n = p['name']
    if k == 'str':
        continue
    ws = p['w']
    if k == 'window':
        lines.append('struct DebugWindow %s = { { %s, %s, %s, %s, %s, %s, %s, %s }, %s, %s, %s };' % (
            n, sint(ws[0]), sint(ws[1]), sint(ws[2]), sint(ws[3]), ref(ws[4]), sint(ws[5]), sint(ws[6]),
            sint(ws[7]), sint(ws[8]), sint(ws[9]), sint(ws[10])))
    elif k == 'items':
        lines.append('struct DebugMenuItem %s[] =\n{' % n)
        for i in range(0, len(ws), 6):
            e = ws[i:i + 6]
            t = int(e[0], 16)
            lim = e[5]
            if lim.startswith('0x'):
                limv = '(void *)%s' % sint(lim) if int(lim, 16) else 'NULL'
            else:
                limv = ref(lim)
            ptr = e[4]
            ptrv = ref(ptr) if not ptr.startswith('0x') else ('NULL' if int(ptr, 16) == 0 else '(void *)' + sint(ptr))
            fmt = e[3]
            fmtv = ref(fmt) if not fmt.startswith('0x') else ('NULL' if int(fmt, 16) == 0 else sint(fmt))
            lines.append('    { 0x%X, %s, %s, %s, %s, %s },' % (t, sint(e[1]), sint(e[2]), fmtv, ptrv, limv))
        lines.append('};')
    elif k == 'ilim':
        assert len(ws) == 4, n
        lines.append('struct DebugMenuIntLimits %s = { %s, %s, %s, %s };' % (n, sint(ws[0]), sint(ws[1]), sint(ws[2]), sint(ws[3])))
    elif k == 'flim':
        assert len(ws) == 4, n
        lines.append('struct DebugMenuFloatLimits %s = { %s, %s, %s, %s };' % (n, sint(ws[0]), f32(ws[1]), f32(ws[2]), f32(ws[3])))
    elif k == 'buf':
        lines.append('#pragma explicit_zero_data on\nchar %s[%d] = {0};\n#pragma explicit_zero_data reset' % (n, 4 * len(ws)))
    elif k == 'strtab':
        lines.append('char *%s[] = { %s };' % (n, ', '.join(ref(w) for w in ws)))
    else:
        if all(w.startswith('0x') for w in ws):
            lines.append('u32 %s[] = { %s };' % (n, ', '.join('0x%X' % int(w, 16) for w in ws)))
        else:
            lines.append('void *%s[] = { %s };' % (n, ', '.join(ref(w) if not w.startswith('0x') else '(void *)0x%X' % int(w, 16) for w in ws)))

hdr = ['/* Debug menu tables (window.c .data 0x245E4-0x2D2B0), generated from the original data. */', '']
for e in sorted(externs):
    hdr.append('extern u8 %s[];' % e)
hdr.append('')
# forward declarations for objects referenced before their definition
defined = set()
fwd = []
for p in pieces:
    if p['kind'] == 'str':
        continue
    for w in p.get('w', []):
        nn = w.split('+')[0]
        if nn in piece_by_name and piece_by_name[nn]['kind'] != 'str' and nn not in defined and nn != p['name']:
            k = piece_by_name[nn]['kind']
            decl = {'window': 'extern struct DebugWindow %s;', 'items': 'extern struct DebugMenuItem %s[];',
                    'ilim': 'extern struct DebugMenuIntLimits %s;', 'flim': 'extern struct DebugMenuFloatLimits %s;',
                    'strtab': 'extern char *%s[];'}.get(k, 'extern u32 %s[];') % nn
            if decl not in fwd:
                fwd.append(decl)
    defined.add(p['name'])
open(outpath, 'w').write('\n'.join(hdr + fwd + [''] + lines) + '\n')
print('pieces', len(pieces), 'externs', len(externs), 'fwd', len(fwd))
from collections import Counter
print(Counter(p['kind'] for p in pieces))
