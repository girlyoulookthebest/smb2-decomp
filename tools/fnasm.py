#!/usr/bin/env python3
"""Print the target disassembly of functions (address + instruction only).
--fold collapses inlined camera_clear() bodies. Usage: tools/fnasm.py <split .s file> [--fold] <fn>..."""
import re
import sys

INSN = re.compile(r"^/\* ([0-9A-F]+) [0-9A-F]+  [0-9A-F ]{11} \*/\t(.*)$")


def main():
    lines = open(sys.argv[1]).read().splitlines()
    fold = "--fold" in sys.argv
    for fn in [a for a in sys.argv[2:] if a != "--fold"]:
        inside = False
        folding = False
        for line in lines:
            if line.startswith(f".fn {fn},"):
                inside = True
            if inside:
                m = INSN.match(line)
                if m and fold and re.match(r"lwz r\d+, 0x20c\(r\d+\)", m.group(2)) and not folding:
                    folding = True
                    print("          ... inlined camera_clear(camera) ...")
                if m and folding:
                    if re.match(r"stfs f0, 0x110\(r3\)", m.group(2)):
                        folding = False
                    continue
                if m:
                    print(f"{m.group(1)}  {m.group(2)}")
                elif line.strip() and not line.startswith("#"):
                    print(line)
            if inside and line.startswith(f".endfn {fn}"):
                break


if __name__ == "__main__":
    main()
