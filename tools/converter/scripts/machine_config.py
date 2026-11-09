"""The machine file (configs/roadcraft-machine.json) for Python tools, resolved by its one reader (src/common/machine.ts).

`machine()` returns every key with `${...}` expanded (`repo` is the game repository, `converter` this folder), read once per
process through `node scripts/machine.ts`; nothing here parses the file, so Python and TypeScript cannot disagree on a path.
Use: sys.path.insert(0, <repo>/tools/converter/scripts); from machine_config import machine
"""
import functools
import json
import os
import shutil
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
# The resolver is one short Node process; past this it is stopped and the caller gets the error.
RESOLVE_TIMEOUT_S = 60


@functools.lru_cache(maxsize=1)
def machine():
    node = shutil.which("node")
    if node is None:
        raise RuntimeError("node is not on the PATH: the machine file is resolved by tools/converter/scripts/machine.ts")
    run = subprocess.run([node, os.path.join(HERE, "machine.ts")], capture_output=True, text=True, encoding="utf8",
                         timeout=RESOLVE_TIMEOUT_S, check=False)
    if run.returncode != 0:
        raise RuntimeError(f"machine.ts failed ({run.returncode}): {run.stderr.strip()}")
    return json.loads(run.stdout)
