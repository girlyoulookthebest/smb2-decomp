# Parse a dtk data asm file into objects: (addr, size, name, kind, words/bytes, refs)
import re
def parse(path):
    d=open(path).read()
    out=[]
    for blk in re.split(r'\n(?=# \.data:)', d):
        h=re.match(r'# \.data:0x[0-9A-F]+ \| 0x([0-9A-F]+) \| size: 0x([0-9A-F]+)\n\.obj (\S+), (\w+)', blk)
        if not h: continue
        addr=int(h.group(1),16); size=int(h.group(2),16); name=h.group(3)
        body=blk[h.end():blk.index('.endobj')]
        items=[]
        for t,v in re.findall(r'\.(4byte|2byte|byte|string|float|double) (.+)',body):
            items.append((t,v.strip()))
        out.append(dict(addr=addr,size=size,name=name,items=items,body=body))
    return out
