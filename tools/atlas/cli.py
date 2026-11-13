#!/usr/bin/env python3
"""The atlas, the project's one traceability tool: python tools/atlas/cli.py <command> [args] [--json]
Exit 0 on an answer, 1 when the answer is a failure (a check that found something, a change the rules refuse), 2 on a usage
error. Commands: `problems` (the register: tools/atlas/problems_cli.py); `inventory` (the features of the reference and of this
game, paired by evidence: tools/atlas/inventory.py)."""
import argparse
import os
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import inventory  # noqa: E402
from atlas import problems as register  # noqa: E402
from atlas import problems_cli  # noqa: E402

USAGE = 2


def parser():
    """The options every command takes, wherever they are written: each subcommand's parser shares them with defaults
    suppressed, so a subcommand never overwrites what the top level read (argparse's defaults of a subparser otherwise do)."""
    common = argparse.ArgumentParser(add_help=False)
    common.add_argument("--json", action="store_true", default=argparse.SUPPRESS, help="the answer as JSON")
    common.add_argument("--root", default=argparse.SUPPRESS, help="the repository (the tests give a folder of their own)")
    ap = argparse.ArgumentParser(prog="atlas", description=__doc__.splitlines()[0], parents=[common])
    ap.set_defaults(json=False, root=register.ROOT)
    commands = ap.add_subparsers(dest="command", required=True)
    problems_cli.add_parser(commands, common)
    inventory.add_parser(commands, common)
    return ap


def utf8_out():
    """Problems are written in UTF-8 whatever the console's code page; an output a caller replaced (a test's buffer) is left."""
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")


def main(argv=None):
    utf8_out()
    try:
        args = parser().parse_args(argv)
    except SystemExit as e:  # argparse's own exit: 0 for --help, 2 for a usage error
        return e.code if isinstance(e.code, int) else USAGE
    try:
        if args.command == "problems":
            return problems_cli.run(args, root=args.root)
        if args.command == "inventory":
            return inventory.run(args, root=args.root)
    except register.RegisterError as e:  # a register that cannot be read
        print(f"atlas: {e}")
        return 1
    return USAGE


if __name__ == "__main__":
    sys.exit(main())
