"""One analysis of some roots, timed by its own CPU time, with the tracer of a given folder: what the bench runs for both
versions of a time it doubts (bench/run.py `against_before`), each in a process of its own.

    python bench/timed.py <folder holding the tracer package> <base or ""> <root>...

Prints the seconds. Not part of the tracer.
"""
from __future__ import annotations

import importlib
import sys
import time


def main(argv=None) -> int:
    argv = sys.argv[1:] if argv is None else argv
    folder, base, roots = argv[0], argv[1] or None, argv[2:]
    sys.path.insert(0, folder)
    sys.setrecursionlimit(100000)
    tracer = importlib.import_module("tracer")       # the copy the folder holds: known only as the bench runs
    began = time.process_time()
    tracer.analyse(roots, base)
    print(round(time.process_time() - began, 1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
