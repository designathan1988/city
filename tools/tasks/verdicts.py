# Verdicts: what a finished process's log and outputs say beyond its exit code. A task names one (Task.verdict); it runs
# when the process exited 0 or 1 and returns PASS or FAIL with the reason. Exit 1 is a failure whatever the log says.
from __future__ import annotations

import re
from pathlib import Path

from . import godot_deps, inputs
from .task import FAIL, PASS

# The failure lines of the Godot tests and captures: a crash can leave exit code 0, and neither a SCRIPT ERROR nor a SHADER ERROR
# (a material drawn by the fallback) stops a scene.
GODOT_FAILED = re.compile(r"Program crashed|SCRIPT ERROR|SHADER ERROR|^FAIL\b|FAILURES: [1-9]", re.M)
SHOWN_LINES = 5


def _text(log: str) -> str:
    p = Path(log)
    return p.read_text(encoding="utf-8", errors="replace") if p.is_file() else ""


def failure_lines(log: str, limit: int = SHOWN_LINES) -> list[str]:
    """The lines of a log that tell a failure (the Godot rule, which also covers the tools' own FAIL lines)."""
    return [line for line in _text(log).splitlines() if GODOT_FAILED.search(line)][:limit]


def exit_code(task, log: str, code: int, root: Path):
    """The default: the exit code alone."""
    return (PASS, "") if code == 0 else (FAIL, f"exit {code}")


def godot(task, log: str, code: int, root: Path):
    """A Godot scene: exit 0 and not one failure line; and when its inputs are what it loads ({loads}, params `loads`, a
    --verbose run), every resource the log says it loaded among them."""
    bad = failure_lines(log)
    if code != 0 or bad:
        return FAIL, "; ".join(bad)[:300] or f"exit {code}"
    if task.params.get("loads"):
        return undeclared_loads(task, _text(log), root)
    return PASS, ""


def undeclared_loads(task, text: str, root: Path):
    """The loads check: a resource the run loaded that its declared inputs leave out is a link the closure missed, so the
    result would have been reused after that file changed. A run that loaded nothing proves nothing: it fails too."""
    seen = godot_deps.loaded(text, task.params["loads"])
    if not seen:
        return FAIL, "the --verbose log names no loaded resource: the declared inputs were not checked"
    declared = set(inputs.files(task.inputs, root=root))
    missing = [f for f in seen if f not in declared]
    if missing:
        more = f" (and {len(missing) - SHOWN_LINES} more)" if len(missing) > SHOWN_LINES else ""
        return FAIL, f"loaded outside its declared inputs: {', '.join(missing[:SHOWN_LINES])}{more}"
    return PASS, f"{len(seen)} loaded resources, all declared"


def _line(task, log: str) -> str | None:
    rx = re.compile(task.params["line"], re.M)
    m = next((rx.search(line) for line in _text(log).splitlines() if rx.search(line)), None)
    return m.string.strip() if m else None


def matches_golden(task, log: str, code: int, root: Path):
    """A Godot run whose line matching params.line must equal the file params.golden (written only by write_golden)."""
    verdict = godot(task, log, code, root)
    if verdict[0] != PASS:
        return verdict
    line = _line(task, log)
    if line is None:
        return FAIL, f"the run printed no line matching {task.params['line']!r}"
    golden = root / task.params["golden"]
    if not golden.is_file():
        return FAIL, f"no golden at {task.params['golden']}"
    want = golden.read_text(encoding="utf-8").strip()
    if line != want:
        return FAIL, f"moved:\n  golden {want}\n  now    {line}"
    return PASS, "equal to the golden"


def write_golden(task, log: str, code: int, root: Path):
    """--bless only: the line matching params.line becomes the golden (the task's declared output)."""
    verdict = godot(task, log, code, root)
    if verdict[0] != PASS:
        return verdict
    line = _line(task, log)
    if line is None:
        return FAIL, f"the run printed no line matching {task.params['line']!r}"
    golden = root / task.params["golden"]
    golden.parent.mkdir(parents=True, exist_ok=True)
    golden.write_text(line + "\n", encoding="utf-8")
    return PASS, f"golden written: {task.params['golden']}"


def no_machine_path(task, log: str, code: int, root: Path):
    """A build: exit 0, and none of its outputs holds the repository's absolute root (the folder of the machine that built it,
    a person's name with it), in UTF-8 or UTF-16 as a binary keeps text: native/CMakeLists.txt maps every source path the
    compiler writes from the root, and this holds it to that."""
    if code != 0:
        return FAIL, f"exit {code}"
    needles = {form.encode(enc) for form in (root.as_posix(), str(root)) for enc in ("utf-8", "utf-16-le")}
    found, seen = [], 0
    for out in task.outputs:
        path = root / out
        if not path.exists():
            return FAIL, f"no output at {out}"
        for f in ([path] if path.is_file() else sorted(p for p in path.rglob("*") if p.is_file())):
            seen += 1
            data = f.read_bytes()
            if any(n in data for n in needles):
                found.append(f.relative_to(root).as_posix())
    if not seen:
        return FAIL, "no output file to look at: an empty set proves nothing"
    if found:
        return FAIL, f"the build's root folder is written in {', '.join(found[:SHOWN_LINES])}"
    return PASS, f"{seen} output file(s), no machine folder in them"


VERDICTS = {"exit_code": exit_code, "godot": godot, "matches_golden": matches_golden, "write_golden": write_golden,
            "no_machine_path": no_machine_path}


def judge(task, log: str, code: int, root: Path) -> tuple[str, str]:
    fn = VERDICTS.get(task.verdict or "exit_code")
    if fn is None:
        raise KeyError(f"{task.name}: unknown verdict {task.verdict!r} (known: {', '.join(sorted(VERDICTS))})")
    out = fn(task, log, code, root)
    status, message = (out, "") if isinstance(out, str) else out
    if code == 1:
        status = FAIL
    if status not in (PASS, FAIL):
        raise ValueError(f"verdict {task.verdict} returned {status!r}")
    return status, message
