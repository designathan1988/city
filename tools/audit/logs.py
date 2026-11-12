"""What the game itself said was wrong, read from the logs of its runs (tests, captures, audits, the game).

A run that ends with its checks passed can still have printed errors: the engine's (`ERROR:`, `WARNING:`, each followed by
`   at: function (file:line)`), a script's (`SCRIPT ERROR:`), the user-level ones (`USER ERROR:`, `USER WARNING:`), what was
left alive at exit (`Leaked instance:`, `Resource still in use:`, `... was leaked`, `... leaked at exit`, `still in use at
exit`), a test's failed check (`FAIL ...`) and the findings an audit scene prints (`FINDING {json}`). Each becomes a finding
owned by the place the log names: the `at:` line (the project's own file when the stack shows one, the engine's function
otherwise), the leaked class or resource, the task for a failed check.

A run that provokes an error on purpose says so before it does: a line `EXPECTED <text>` in the same log marks every later
line holding <text> as expected. Expected findings are listed apart and never counted; nothing is hidden by a list kept here.
"""
from __future__ import annotations

import re
from pathlib import Path

from .findings import Finding, from_json

HEADS = {
    "SCRIPT ERROR:": ("log", "script error"),
    "USER SCRIPT ERROR:": ("log", "script error"),
    "USER ERROR:": ("log", "error"),
    "ERROR:": ("log", "error"),
    "USER WARNING:": ("log", "warning"),
    "WARNING:": ("log", "warning"),
}
AT = re.compile(r"^\s+at: (?P<function>.+?) \((?P<file>.+?):(?P<line>\d+)\)\s*$")
LEAKS = [
    (re.compile(r"^Leaked instance: (?P<owner>[A-Za-z0-9_]+):"), "leaked instance"),
    (re.compile(r"^Resource still in use: (?P<owner>\S+)"), "resource in use at exit"),
    (re.compile(r"^(?:WARNING: )?\d+ RID of type \"(?P<owner>[^\"]+)\" was leaked"), "leaked RID"),
]
CHECK_FAILED = re.compile(r"^FAIL\s+(?P<what>.+)$")
FINDING = "FINDING "
EXPECTED = "EXPECTED "


def _own(file: str) -> bool:
    """Whether a stack line names the project's own code (the core's sources, a res:// script) and not the engine's."""
    return file.startswith("res://") or "native" in Path(file.replace("\\", "/")).parts


def read_log(path: Path, task: str) -> list[Finding]:
    """The findings of one log; `task` names the run in each finding's `where`."""
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    out: list[Finding] = []
    expected: list[str] = []
    i = 0
    while i < len(lines):
        line = lines[i]
        i += 1
        if line.startswith(EXPECTED):
            expected.append(line[len(EXPECTED):].strip())
            continue
        if line.startswith(FINDING):
            f = from_json(line[len(FINDING):])
            f.where = f.where or [task]
            out.append(f)
            continue
        head = next((h for h in HEADS if line.startswith(h)), None)
        if head is not None:
            domain, rule = HEADS[head]
            what = line[len(head):].strip()
            places = []
            while i < len(lines) and AT.match(lines[i]):
                places.append(AT.match(lines[i]))
                i += 1
            own = [m for m in places if _own(m["file"])]
            at = (own or places or [None])[0]
            owner = "%s (%s)" % (at["file"].replace("\\", "/"), at["function"]) if at else "unknown: %s" % task
            leak = "leaked" in what or "still in use at exit" in what
            out.append(Finding("resources" if leak else domain, "left at exit" if leak else rule, owner, what, 1, [task], line.strip(),
                               any(e and e in line for e in expected)))
            continue
        for pattern, rule in LEAKS:
            m = pattern.match(line)
            if m:
                out.append(Finding("resources", rule, m["owner"], line.strip(), 1, [task], line.strip(), any(e and e in line for e in expected)))
                break
        else:
            m = CHECK_FAILED.match(line)
            if m:
                out.append(Finding("test", "check failed", task, m["what"], 1, [task], line.strip(), any(e and e in line for e in expected)))
    return out


def read_run(folder: Path) -> list[Finding]:
    """Every *.log of a run folder (dist/tasks/<run>), each named by its file."""
    found: list[Finding] = []
    for log in sorted(folder.glob("*.log")):
        found.extend(read_log(log, log.stem))
    return found
