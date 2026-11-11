"""Writes out.txt; --check exits 1 when the file on disk is not what it writes."""
import os
import sys

TEXT = "written by gen.py\n"
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "out.txt")
if "--check" in sys.argv:
    with open(OUT, encoding="utf-8") as fh:
        sys.exit(0 if fh.read() == TEXT else 1)
with open(OUT, "w", encoding="utf-8") as fh:
    fh.write(TEXT)
