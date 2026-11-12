"""The tracer's command: reads the code under the roots and shows, from a start, what runs after it and what reaches
it.

    python trace.py <start> [--root DIR]... [--depth N] [--back N] [--exact] [--json] [--out FILE]
    python trace.py --list [--root DIR]...

A start is a label (`Shop.open`), the end of one (`open`) or `path:line`. Exit 1 when the roots hold no code the
tracer reads or the start names nothing.
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from tracer import analyse  # noqa: E402
from tracer.report import as_data, as_text, listing  # noqa: E402
from tracer.vocab import BY_NAME, RUNNING  # noqa: E402
from tracer.walk import backward, find, forward  # noqa: E402

FORWARD, BACKWARD = "What runs from it:", "What reaches it:"


def arguments(argv):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("start", nargs="?", help="a label, the end of one, or path:line")
    parser.add_argument("--root", action="append", help="a folder or a file to read (default: the current folder)")
    parser.add_argument("--depth", type=int, help="how far forward (default: to the end)")
    parser.add_argument("--back", type=int, help="how far backward (default: to the end)")
    parser.add_argument("--exact", action="store_true", help="leave out what was found by name alone")
    parser.add_argument("--json", action="store_true", help="the trace as data")
    parser.add_argument("--out", help="write the trace to this file")
    parser.add_argument("--list", action="store_true", help="every definition code is written in")
    return parser.parse_args(argv)


def main(argv=None) -> int:
    args = arguments(argv)
    began = time.monotonic()

    def say(words: str) -> None:
        print(f"[{time.monotonic() - began:6.1f} s] {words}", file=sys.stderr, flush=True)
    result = analyse(args.root or ["."], say=say)
    program = result.program
    say(f"{sum(len(found) for found in result.out.values())} links")
    if not program.files:
        print("the roots hold no code the tracer reads", file=sys.stderr)
        return 1
    if args.list or not args.start:
        text = listing(result, RUNNING)
    else:
        starts = find(result, args.start)
        if not starts:
            print(f"nothing is named {args.start}: --list shows every name", file=sys.stderr)
            return 1
        keep = (lambda link: link.how != BY_NAME) if args.exact else (lambda link: True)
        ahead, behind = forward(result, starts, args.depth, keep), backward(result, starts, args.back, keep)
        if args.json:
            text = json.dumps({"start": args.start, "forward": as_data(result, ahead),
                               "backward": as_data(result, behind)}, indent=1)
        else:
            text = as_text(result, FORWARD, ahead, True) + "\n\n" + as_text(result, BACKWARD, behind, False)
    if args.out:
        Path(args.out).write_bytes((text + "\n").encode("utf-8"))
    else:
        sys.stdout.buffer.write((text + "\n").encode("utf-8"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
