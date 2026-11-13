# The method's records a block's close asks of its work file, doc/work/<block>.md (the constitution, principles III and IV;
# specification 003, contracts/block-close.md): a file read to its last line, the research (Context7 and a source on the internet)
# when code changed, the skills at their moments, and every failing run with its cause. `gaps` names what is missing, nothing else.
# Git is asked with no console window (tasks.proc).
from __future__ import annotations

import re
import subprocess
from pathlib import Path

from . import proc

WORK = Path("doc/work")
CODE_ROOTS = ("native/", "project/", "tools/")
GIT_TIMEOUT_S = 60
READ_WHOLE = re.compile(r"\b(\d+)-(\d+) of (\d+)\b")
SECTION = re.compile(r"^## (\w+).*$", re.MULTILINE)  # a heading's first word: the template's carry a note, `## Read (path: ...)`
SKILLS_ALWAYS = ("city-method", "verification-before-completion")
SKILL_AFTER_FAIL = "systematic-debugging"
FAILED = "FAIL"
CAUSE = "cause"


def sections(text: str) -> dict[str, str]:
    """The work file's `## ` sections by their heading's first word, each with its text."""
    marks = list(SECTION.finditer(text))
    return {m.group(1): text[m.end():marks[i + 1].start() if i + 1 < len(marks) else len(text)] for i, m in enumerate(marks)}


def _git(root: Path, *args: str) -> list[str]:
    done = subprocess.run(["git", "-C", str(root), *args], capture_output=True, text=True, timeout=GIT_TIMEOUT_S, check=True,
                          creationflags=proc.NO_WINDOW)
    return [line for line in done.stdout.splitlines() if line]


def changed_code(root: Path, head: str | None) -> bool:
    """Whether the block changed code: a changed or new file under native/, project/ or tools/ since `head`, the commit the block
    opened on (none recorded: taken as changed, so the research is asked)."""
    if not head:
        return True
    paths = _git(root, "diff", "--name-only", head) + _git(root, "ls-files", "--others", "--exclude-standard")
    return any(p.startswith(CODE_ROOTS) for p in paths)


def gaps(root: Path, doc: dict) -> list[str]:
    """What the block's work file lacks of the method, each with the line to add; empty when it holds everything."""
    path = Path(root) / WORK / f"{doc['name']}.md"
    if not path.is_file():
        return [f"no work file {WORK.as_posix()}/{doc['name']}.md (the goal, Read, Research, Checks, Skills)"]
    parts = sections(path.read_text(encoding="utf-8"))
    out = []
    if not any(b == n for _a, b, n in READ_WHOLE.findall(parts.get("Read", ""))):
        out.append("## Read: no file read to its last line (a line `path: 1-N of N`)")
    research = parts.get("Research", "")
    if changed_code(Path(root), doc.get("head")):
        if "Context7" not in research:
            out.append("## Research: no Context7 line (library id, question, answer) for the code this block changed")
        if "http://" not in research and "https://" not in research:
            out.append("## Research: no source on the internet (its URL) for the code this block changed")
    skills = parts.get("Skills", "")
    checks = parts.get("Checks", "")
    for skill in SKILLS_ALWAYS + ((SKILL_AFTER_FAIL,) if FAILED in checks else ()):
        if skill not in skills:
            out.append(f"## Skills: {skill} not named")
    for line in checks.splitlines():
        if FAILED in line and CAUSE not in line.lower():
            out.append(f"## Checks: a failing run without its cause: {line.strip()}")
    return out
