# The gates of one item of a block (the constitution 1.3.0, principles III and IV): they stop the cycle of trial and error the owner
# forbade on 2026-11-13 (code written before the problem was read, traced and researched; a check run again with no cause read; a
# second failure answered with a third run instead of research). What the session really did is the edit guard's (edit_guard.py).
#   Before its code (`block open [<block>] --item <ID>`): the work file's `## Item ... <ID> ...` section records a file read to its last
#   line (`path: 1-N of N`), a `- Trace:` line with tools/tracer/trace.py and a `- Research:` line with Context7 and a source's URL.
#   While it is checked (at the end, with its photos): a failing check is remembered, with its time and the item's causes (`- Cause:`
#   lines naming a file:line) and sources (URLs on `- Research:` lines) counted then; it runs again only once a new cause is written,
#   after its second failure a new source too.
from __future__ import annotations

import datetime
import re
from pathlib import Path

WORK = Path("doc/work")
ITEM = re.compile(r"^## Item\b.*$", re.MULTILINE)
HEADING = re.compile(r"^## ", re.MULTILINE)
READ_WHOLE = re.compile(r"\b(\d+)-(\d+) of (\d+)\b")
TRACE = re.compile(r"^- Trace:.*trace\.py", re.MULTILINE)
STAMP = "%Y-%m-%dT%H:%M:%S.000Z"  # the session transcript's own (UTC): the guard compares the two
CAUSE = re.compile(r"^- Cause:.*\b[\w./-]+\.\w+:\d+", re.MULTILINE)  # a cause says where it is: file:line
RESEARCH = re.compile(r"^- Research:.*$", re.MULTILINE)
URL = re.compile(r"https?://\S+")
FAILED = ("FAIL", "TIMEOUT", "BROKEN")
SECOND = 2  # failures of one check after which a new source is asked besides a new cause


def section(root: Path, block: str, item: str) -> str | None:
    """The text under the work file's `## Item` heading that names `item`, to the next `## ` heading; None without one."""
    path = Path(root) / WORK / f"{block}.md"
    if not path.is_file():
        return None
    text = path.read_text(encoding="utf-8")
    for m in ITEM.finditer(text):
        if re.search(rf"(?<![\w-]){re.escape(item)}(?![\w-])", m.group(0)):
            nxt = HEADING.search(text, m.end())
            return text[m.end():nxt.start() if nxt else len(text)]
    return None


def utc_now() -> str:
    """Now, as the session transcript stamps its entries."""
    return datetime.datetime.now(datetime.timezone.utc).strftime(STAMP)


def opening_gaps(root: Path, block: str, item: str) -> list[str]:
    """What item `item` lacks before its code may be opened, each with the line to write; empty when it may be opened."""
    text = section(root, block, item)
    if text is None:
        return [f"no `## Item ... ({item})` section in {WORK.as_posix()}/{block}.md"]
    out = []
    if not any(b == n for _a, b, n in READ_WHOLE.findall(text)):
        out.append("no file read to its last line (`path: 1-N of N`): every file the problem goes through, read whole")
    if not TRACE.search(text):
        out.append("no `- Trace:` line with the tracer's command (python tools/tracer/trace.py ..., in both games)")
    research = "\n".join(RESEARCH.findall(text))
    if "Context7" not in research or not URL.search(research):
        out.append("no `- Research:` line with Context7 (library id, question, answer) and a source's URL")
    return out


def counts(root: Path, doc: dict) -> tuple[int, int]:
    """The causes written in the item now (`- Cause:` lines naming a file:line) and its sources (URLs on `- Research:` lines)."""
    text = section(root, doc["name"], doc["item"]) or ""
    return len(CAUSE.findall(text)), len(URL.findall("\n".join(RESEARCH.findall(text))))


def run_refusal(root: Path, doc: dict, tasks: list[str]) -> str | None:
    """Why the checks `tasks` of the item under verification may not run again now; None when they may."""
    if not doc.get("item"):
        return None
    causes, sources = counts(root, doc)
    for task in tasks:
        f = doc.get("fails", {}).get(task)
        if not f:
            continue
        if causes <= f["causes"]:
            return (f"refused: `{task}` failed {f['n']} time(s) for item {doc['item']} and no new cause is written. Read the failure "
                    f"to its cause (superpowers:systematic-debugging), write it in the item (`- Cause: <what> at <file:line>`), fix "
                    f"the code, then run it (the constitution: never a change run again without a cause read).")
        if f["n"] >= SECOND and sources <= f["sources"]:
            return (f"refused: `{task}` failed {f['n']} times for item {doc['item']}: back to research, never another run. Write the "
                    f"source the next cause rests on in the item (`- Research: ... https://...`).")
    return None


def remember(root: Path, doc: dict, results: dict[str, str]) -> dict:
    """The item's failure memory after a run: a failing check counted with the causes and sources written then; a passing one
    forgotten."""
    if not doc.get("item"):
        return doc
    causes, sources = counts(root, doc)
    fails = dict(doc.get("fails", {}))
    for task, status in results.items():
        if status in FAILED:
            fails[task] = {"n": fails.get(task, {}).get("n", 0) + 1, "causes": causes, "sources": sources, "at": utc_now()}
        else:
            fails.pop(task, None)
    return {**doc, "fails": fails}
