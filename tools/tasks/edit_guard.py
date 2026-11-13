# The edit guard (the constitution 1.3.0, principle IV; a PreToolUse hook of .claude/settings.json): the method is held by what the
# session really did, read in its own transcript, not by what a record says. An edit of code (native/, project/, tools/) is refused
# unless the block is open on an item (block.py) and the session shows: CLAUDE.md read and city-method loaded since its last compaction;
# Context7 asked, the internet searched (WebSearch or WebFetch) and the tracer run (trace.py) since the item's preparation began; the
# file read to its last line (a new file needs none); after a failing check, systematic-debugging loaded since. A commit while an item
# is in hand is refused unless verification-before-completion was loaded and a photo of the change (dist/captures) opened since the
# item's code was opened: the tests come at the end, with the photos and their analysis. Exit 2 refuses, its reasons on stderr (the
# hook's contract: Context7 /anthropics/claude-code); anything else passes.
from __future__ import annotations

import argparse
import datetime
import json
import sys
from pathlib import Path

if str(Path(__file__).resolve().parents[1]) not in sys.path:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tasks import block, block_item  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
CODE = ("native", "project", "tools")
GAME = ("native", "project")  # the game's code: traced (C++, GDScript, shaders); the tools are Python, which the tracer does not read
EDITS = ("Edit", "Write", "MultiEdit", "NotebookEdit")
READ_DEFAULT_LINES = 2000  # the Read tool's lines when no limit is given
REFUSED = 2


def _when(text: str | None) -> datetime.datetime | None:
    return datetime.datetime.fromisoformat(text.replace("Z", "+00:00")) if text else None


def _calls(transcript: Path) -> tuple[list[tuple[datetime.datetime, str, dict]], datetime.datetime | None]:
    """The session's tool calls (time, name, input) in order, and the time of its last compaction (None: never compacted)."""
    calls, compacted = [], None
    if not transcript.is_file():
        return calls, compacted
    for line in transcript.read_text(encoding="utf-8").splitlines():
        try:
            e = json.loads(line)
        except json.JSONDecodeError:
            continue
        if e.get("type") == "system" and e.get("subtype") == "compact_boundary":
            compacted = _when(e.get("timestamp"))
        if e.get("type") != "assistant":
            continue
        for c in (e.get("message") or {}).get("content") or []:
            if isinstance(c, dict) and c.get("type") == "tool_use":
                calls.append((_when(e.get("timestamp")), c.get("name", ""), c.get("input") or {}))
    return calls, compacted


def _same(a: str, b: Path) -> bool:
    return Path(a).resolve(strict=False) == b.resolve(strict=False)


def _read_whole(calls, since, path: Path) -> bool:
    """Whether the Read calls since `since` cover every line of `path` as it is now."""
    lines = len(path.read_text(encoding="utf-8", errors="replace").splitlines())
    seen = set()
    for t, name, inp in calls:
        if name == "Read" and (since is None or t >= since) and _same(inp.get("file_path", ""), path):
            first = int(inp.get("offset") or 1)
            seen.update(range(first, first + int(inp.get("limit") or READ_DEFAULT_LINES)))
    return all(n in seen for n in range(1, lines + 1))


def _did(calls, since, test) -> bool:
    return any((since is None or t >= since) and test(name, inp) for t, name, inp in calls)


def _skill(word: str):
    return lambda name, inp: name == "Skill" and word in str(inp.get("skill", ""))


def edit_gaps(root: Path, path: Path, transcript: Path) -> list[str]:
    """Why an edit of `path` (code) may not be made now; empty when it may."""
    doc = block.load(root)
    if doc["state"] != block.OPEN:
        return [f"the block is {doc['state']}: code is written only while a block is open on an item "
                f"(python tools/tasks/cli.py block open <block> --item <ID>)"]
    calls, compacted = _calls(transcript)
    item_from = max(filter(None, (compacted, _when(doc.get("item_from")))), default=None)
    out = []
    if not _did(calls, compacted, lambda n, i: n == "Read" and str(i.get("file_path", "")).endswith("CLAUDE.md")):
        out.append("CLAUDE.md not read since the last compaction (Read, whole)")
    if not _did(calls, compacted, _skill("city-method")):
        out.append("the city-method skill not loaded since the last compaction")
    if not _did(calls, item_from, lambda n, i: "query-docs" in n):
        out.append("Context7 not asked for this item (resolve-library-id, then query-docs, for each API the code calls)")
    if not _did(calls, item_from, lambda n, i: n in ("WebSearch", "WebFetch")):
        out.append("the internet not searched for this item (WebSearch, then the pages opened with WebFetch)")
    game = path.relative_to(root).parts[0] in GAME  # the tracer reads the game's languages, not the tools' Python
    if game and not _did(calls, item_from, lambda n, i: n in ("Bash", "PowerShell") and "trace.py" in str(i.get("command", ""))):
        out.append("the tracer not run for this item (python tools/tracer/trace.py --feature ... in both games)")
    if path.is_file() and not _read_whole(calls, item_from, path):
        out.append(f"{path.name} not read to its last line for this item (Read, in chunks of about 400 lines to the end)")
    failed = [_when(f.get("at")) for f in doc.get("fails", {}).values() if f.get("at")]
    if failed and not _did(calls, max(failed), _skill("systematic-debugging")):
        out.append("a check failed: the superpowers:systematic-debugging skill not loaded since, for its cause")
    return out


def commit_gaps(root: Path, transcript: Path) -> list[str]:
    """Why a commit may not be made now (an item in hand, its end not checked with its photos); empty when it may."""
    doc = block.load(root)
    if doc["state"] == block.CLOSED or not doc.get("item_opened"):
        return []
    calls, _ = _calls(transcript)
    since = _when(doc["item_opened"])
    out = []
    if not _did(calls, since, _skill("verification-before-completion")):
        out.append("the superpowers:verification-before-completion skill not loaded since the item's code was opened")
    photo = lambda n, i: n == "Read" and "dist/captures" in str(i.get("file_path", "")).replace("\\", "/") and \
        str(i.get("file_path", "")).endswith(".png")  # noqa: E731
    if not _did(calls, since, photo):
        out.append("no photo of the change opened since the item's code was opened (a looks plan, each photo read and analysed)")
    return out


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(prog="edit_guard.py", description="the PreToolUse guard of the method")
    ap.add_argument("--root", default=str(ROOT), help=argparse.SUPPRESS)
    root = Path(ap.parse_args(argv).root).resolve()
    data = json.load(sys.stdin)
    tool, inp = data.get("tool_name", ""), data.get("tool_input") or {}
    transcript = Path(data.get("transcript_path") or "")
    gaps = []
    if tool in EDITS:
        path = Path(inp.get("file_path") or inp.get("notebook_path") or "").resolve(strict=False)
        try:
            top = path.relative_to(root).parts[0]
        except (ValueError, IndexError):
            return 0
        if top in CODE:
            gaps = edit_gaps(root, path, transcript)
    elif tool in ("Bash", "PowerShell") and "git commit" in str(inp.get("command", "")):
        gaps = commit_gaps(root, transcript)
    if not gaps:
        return 0
    print("refused by the method (tools/tasks/edit_guard.py; the constitution, principle IV):\n  - " + "\n  - ".join(gaps), file=sys.stderr)
    return REFUSED


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
