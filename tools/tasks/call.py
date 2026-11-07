# Runs a Python function as a task's process: python tools/tasks/call.py module:function spec.json. The function takes the
# spec (a dict) and returns the exit code (None is 0). An exception prints its traceback and exits 3: broken, never a verdict.
import importlib
import json
import sys
import traceback
from pathlib import Path

BROKEN_EXIT = 3


def main(argv: list[str]) -> int:
    if len(argv) != 2 or ":" not in argv[0]:
        print("usage: call.py module:function spec.json", file=sys.stderr)
        return 2
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
    module, function = argv[0].split(":", 1)
    spec = json.loads(Path(argv[1]).read_text(encoding="utf-8"))
    try:
        code = getattr(importlib.import_module(module), function)(spec)
    except Exception:
        traceback.print_exc()
        return BROKEN_EXIT
    return 0 if code is None else int(code)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
