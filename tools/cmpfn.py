#!/usr/bin/env python3
"""Compare a function's instruction words between the target split object and our
compiled object, ignoring bits that relocations fill in. With no function names, compares
every function both objects define. Usage: cmpfn.py <target.o> <ours.o> [fn...]"""
import struct
import sys


def load(path):
    data = open(path, "rb").read()
    shoff, = struct.unpack(">I", data[0x20:0x24])
    shentsize, shnum, shstrndx = struct.unpack(">HHH", data[0x2E:0x34])
    secs = [struct.unpack(">IIIIIIIIII", data[shoff + i * shentsize:shoff + (i + 1) * shentsize]) for i in range(shnum)]
    symtab = next(s for s in secs if s[1] == 2)
    strtab = secs[symtab[6]]
    syms = {}
    for i in range(symtab[5] // 16):
        name, value, size, info, other, shndx = struct.unpack(">IIIBBH", data[symtab[4] + i * 16:symtab[4] + i * 16 + 16])
        if info & 0xF == 2:  # STT_FUNC
            end = data.index(b"\0", strtab[4] + name)
            syms[data[strtab[4] + name:end].decode()] = (shndx, value, size)
    relocated = {}
    for s in secs:
        if s[1] == 4:  # SHT_RELA
            for i in range(s[5] // 12):
                off, info, _ = struct.unpack(">IIi", data[s[4] + i * 12:s[4] + i * 12 + 12])
                relocated[(s[7], off & ~3)] = True
    return data, secs, syms, relocated


def words(obj, fn):
    data, secs, syms, relocated = obj
    shndx, value, size = syms[fn]
    base = secs[shndx][4] + value
    out = []
    for off in range(0, size, 4):
        w, = struct.unpack(">I", data[base + off:base + off + 4])
        out.append((w & 0xFFFF0000) if relocated.get((shndx, value + off)) or ((w >> 26) == 18 and w & 1) else w)
    return out


def main():
    target, ours = load(sys.argv[1]), load(sys.argv[2])
    names = sys.argv[3:] or sorted((n for n in ours[2] if n in target[2]), key=lambda n: target[2][n][1])
    matched = 0
    for fn in names:
        a, b = words(target, fn), words(ours, fn)
        diffs = [i for i in range(max(len(a), len(b))) if i >= len(a) or i >= len(b) or a[i] != b[i]]
        matched += not diffs
        print(f"{fn}: {'MATCH' if not diffs else f'{len(diffs)} differing words, first at +0x{diffs[0] * 4:X}'} ({len(a)} vs {len(b)} words)")
    if not sys.argv[3:]:
        for fn in sorted(n for n in target[2] if n not in ours[2]):
            print(f"{fn}: missing from ours")
    print(f"{matched}/{len(names)} match")


if __name__ == "__main__":
    main()
