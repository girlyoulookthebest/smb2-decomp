#!/usr/bin/env python3
"""Rename auto-generated symbols (fn_*/lbl_*) in dtk symbols.txt files using an
SMB2PracticeMod-style .lst map ("ADDR:name" or "moduleId,sectionIdx,offset:name").

Data names at addresses dtk has no symbol for are added as new objects.
Only names dtk invented are replaced, so detected SDK/runtime names and gap_ fillers
stay untouched. Names that aren't C identifiers, Ghidra placeholders, or that would
collide with an existing name are skipped.

Usage: tools/import_lst.py <mkb2.us.lst> [config/GM2E8P]
"""
import re
import struct
import sys
from pathlib import Path

AUTO_NAME = re.compile(r"^(fn|lbl)_(\d+_)?([a-z0-9]+_)?[0-9A-F]+$")
IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
GHIDRA_PLACEHOLDER = re.compile(r"^(FUN|DAT|LAB|PTR|switchD|switchdataD|caseD|prt|s|u)_")
SYMBOL_LINE = re.compile(r"^(\S+) = (\S+):0x([0-9A-Fa-f]+);(.*)$")


def load_lst(path):
    absolute, relative = {}, {}
    for raw in Path(path).read_text().splitlines():
        line = raw.split("//", 1)[0].strip()
        if ":" not in line:
            continue
        loc, name = (s.strip() for s in line.split(":", 1))
        if not IDENT.match(name) or GHIDRA_PLACEHOLDER.match(name):
            continue
        parts = loc.split(",")
        if len(parts) == 3:
            key = tuple(int(p, 16) for p in parts)
            relative.setdefault(key, name)
        else:
            absolute.setdefault(int(loc, 16), name)
    return absolute, relative


def rel_section_names(rel_path, splits_path):
    """Map REL section index -> dtk section name (dtk lists the non-empty sections in order)."""
    data = rel_path.read_bytes()
    count, table = struct.unpack(">II", data[0xC:0x14])
    nonempty = [i for i in range(count) if struct.unpack(">I", data[table + i * 8 + 4:table + i * 8 + 8])[0]]
    sections = re.findall(r"^\t(\S+)", splits_path.read_text().split("Sections:", 1)[1], re.M)
    return dict(zip(nonempty, sections))


def rename(symbols_path, lookup, used, additions=()):
    lines = symbols_path.read_text().splitlines()
    renamed = 0
    # (section, start, end) of every existing symbol; new names must not land inside one.
    occupied = []
    for line in lines:
        m = SYMBOL_LINE.match(line)
        if m:
            start = int(m.group(3), 16)
            size = re.search(r"size:0x([0-9A-Fa-f]+)", m.group(4))
            occupied.append((m.group(2), start, start + (int(size.group(1), 16) if size else 1)))
    for section, addr, name in additions:
        if name in used or any(s == section and lo <= addr < hi for s, lo, hi in occupied):
            continue
        used.add(name)
        lines.append(f"{name} = {section}:0x{addr:08X}; // type:object")
        renamed += 1
    for i, line in enumerate(lines):
        m = SYMBOL_LINE.match(line)
        if not m or not AUTO_NAME.match(m.group(1)):
            continue
        name = lookup(m.group(2), int(m.group(3), 16))
        if name is None or name in used:
            continue
        used.add(name)
        lines[i] = f"{name} = {m.group(2)}:0x{m.group(3)};{m.group(4)}"
        renamed += 1
    symbols_path.write_text("\n".join(lines) + "\n")
    return renamed


DATA_SECTIONS = {".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2"}


def dol_data_additions(config_dir, absolute):
    """Place absolute DOL addresses into dtk's data sections using splits.txt-free section bounds from symbols."""
    bounds = {}
    for line in (config_dir / "symbols.txt").read_text().splitlines():
        m = SYMBOL_LINE.match(line)
        if m and m.group(2) in DATA_SECTIONS:
            addr = int(m.group(3), 16)
            lo, hi = bounds.get(m.group(2), (addr, addr))
            bounds[m.group(2)] = (min(lo, addr), max(hi, addr))
    return [(sec, addr, name) for addr, name in absolute.items()
            for sec, (lo, hi) in bounds.items() if lo <= addr <= hi]


def main():
    lst = sys.argv[1]
    config_dir = Path(sys.argv[2] if len(sys.argv) > 2 else "config/GM2E8P")
    absolute, relative = load_lst(lst)
    config = config_dir / "config.yml"
    object_base = Path(re.search(r"^object_base: (\S+)", config.read_text(), re.M).group(1))

    symbol_files = [config_dir / "symbols.txt"] + sorted(config_dir.glob("*/symbols.txt"))
    used = set()
    for path in symbol_files:
        for line in path.read_text().splitlines():
            m = SYMBOL_LINE.match(line)
            if m and not AUTO_NAME.match(m.group(1)):
                used.add(m.group(1))

    total = rename(config_dir / "symbols.txt", lambda sec, addr: absolute.get(addr), used,
                   dol_data_additions(config_dir, absolute))
    print(f"main: {total}")
    for module in re.finditer(r"^- object: (\S+).*\n(?:  .*\n)*?  name: (\S+)", config.read_text(), re.M):
        rel_path, name = object_base / module.group(1), module.group(2)
        module_id = struct.unpack(">I", rel_path.read_bytes()[:4])[0]
        section_of = rel_section_names(rel_path, config_dir / name / "splits.txt")
        index_of = {v: k for k, v in section_of.items()}
        additions = [(sec, off, n) for (mid, idx, off), n in relative.items()
                     if mid == module_id and (sec := section_of.get(idx)) in DATA_SECTIONS]
        count = rename(config_dir / name / "symbols.txt",
                       lambda sec, off: relative.get((module_id, index_of.get(sec, -1), off)), used, additions)
        print(f"{name}: {count}")
        total += count
    print(f"total renamed: {total}")


if __name__ == "__main__":
    main()
