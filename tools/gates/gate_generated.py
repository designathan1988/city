"""Generated files: each output listed in rules.json/generated is what its generator writes. The generator is asked with
--check (it compares and writes nothing; exit 0 when the outputs on disk are its own) through tools/tasks/proc, the tools'
one owner of processes (its own tree, killed at the deadline, on the private desktop). A table edited by hand, or a
generator that no longer runs on its inputs, fails here."""
import os
import sys
import tempfile

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from tasks import proc  # noqa: E402

GENERATOR_DEADLINE_S = 300  # one generator's check; a generator that gives no answer by then is an error, not a wait
REPORT_TAIL = 400  # characters of the generator's own words kept in the finding


def _entries(ctx):
    return ctx.rules.get("generated", [])


def inputs(ctx, path):
    return any(path == g["generator"] or path in g["outputs"] or path in g.get("inputs", []) for g in _entries(ctx))


def _ask(ctx, generator):
    """(exit code or None when the deadline passed, the generator's last words)."""
    with tempfile.TemporaryDirectory() as tmp:
        log = os.path.join(tmp, "check.log")
        code, late = proc.run([sys.executable, generator, "--check"], GENERATOR_DEADLINE_S, cwd=ctx.root, log=log, hidden=True)
        with open(log, encoding="utf-8", errors="replace") as fh:
            words = fh.read().strip()
    return (None if late else code), words[-REPORT_TAIL:]


def check_generated(ctx, err):
    for g in _entries(ctx):
        generator = g["generator"]
        if not os.path.isfile(os.path.join(ctx.root, generator)):
            err("GENERATED_DRIFT", generator, "the generator of " + ", ".join(g["outputs"]) + " is missing")
            continue
        code, words = _ask(ctx, generator)
        if code is None:
            err("GENERATED_DRIFT", generator, f"--check gave no answer within {GENERATOR_DEADLINE_S} s")
        elif code != 0:
            for out in g["outputs"]:
                err("GENERATED_DRIFT", out, f"not what {generator} writes (exit {code}): {words}")
