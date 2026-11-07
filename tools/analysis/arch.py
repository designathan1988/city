#!/usr/bin/env python3
"""Entry of the architecture analyser: scan, explain, next, predict, done, status and selfcheck over one git repository."""
import sys

from arch.cli import main

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
