#!/usr/bin/env python3
"""Show a compact side-by-side instruction diff for one function via objdiff-cli.
Usage: tools/fdiff.py <unit> <function> [--all]"""
import json
import subprocess
import sys
import tempfile


def main():
    unit, symbol = sys.argv[1], sys.argv[2]
    show_all = "--all" in sys.argv
    with tempfile.NamedTemporaryFile(suffix=".json") as out:
        subprocess.run(["build/tools/objdiff-cli", "diff", "-p", ".", "-u", unit, "-o", out.name,
                        "--format", "json", symbol], check=True, capture_output=True)
        data = json.load(open(out.name))
    def find(side):
        return next((s for s in data[side]["symbols"] if s.get("name") == symbol), {})

    left = find("left").get("instructions", [])
    right = find("right").get("instructions", [])
    shown = 0
    for i in range(max(len(left), len(right))):
        l = left[i] if i < len(left) else {}
        r = right[i] if i < len(right) else {}
        kind = l.get("diff_kind") or r.get("diff_kind") or "DIFF_NONE"
        if kind == "DIFF_NONE" and not show_all:
            continue
        mark = " " if kind == "DIFF_NONE" else "*"
        lt = l.get("instruction", {}).get("formatted", "")
        rt = r.get("instruction", {}).get("formatted", "")
        print(f"{i:4} {mark} {lt:40} | {rt:40} {kind.replace('DIFF_', '') if mark == '*' else ''}")
        shown += 1
    print(f"{shown} lines shown; target {len(left)} / ours {len(right)} instructions")


if __name__ == "__main__":
    main()
