#!/usr/bin/env python3
"""Report which data labels the functions in a .text range reference, per section,
to help pick TU split boundaries. Usage: tu_refs.py <asm .s> <start> <end> [neighbour ranges...]"""
import re
import sys
from collections import defaultdict

FN = re.compile(r"^\.fn (\S+),")
INSN = re.compile(r"^/\* ([0-9A-F]{8}) ")
REF = re.compile(r"lbl_\d+_(rodata|data|bss)_([0-9A-F]+)")


def refs_by_range(path, ranges):
    out = {r: defaultdict(set) for r in ranges}
    for line in open(path):
        m = INSN.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        for r in ranges:
            if r[0] <= addr < r[1]:
                for sec, off in REF.findall(line):
                    out[r][sec].add(int(off, 16))
    return out


def main():
    path = sys.argv[1]
    ranges = [(int(a, 16), int(b, 16)) for a, b in zip(sys.argv[2::2], sys.argv[3::2])]
    result = refs_by_range(path, ranges)
    for r in ranges:
        print(f"text 0x{r[0]:X}-0x{r[1]:X}")
        for sec in ("rodata", "data", "bss"):
            offs = sorted(result[r][sec])
            if offs:
                print(f"  {sec:6} {len(offs):4} labels  min 0x{offs[0]:X}  max 0x{offs[-1]:X}")


if __name__ == "__main__":
    main()
