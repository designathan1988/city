"""An edit as the hooks judge it (called by port_gate.py and port_state.py): what it may not leave (a Python or JSON file
unreadable, a work's note with no `## Now`), read before it is written; and, after it, the gates of the file just written, so
a defect a reading shows is fixed where it was written."""

import json
import subprocess
import sys
from pathlib import Path

import port_note

ROOT = Path(__file__).resolve().parents[2]
GATE_SECONDS = 15  # the gates of one file answer in about three
GATE_SHOWN = 12     # findings of a file shown after an edit
SOUND = (".py", ".json")                     # files an edit may not leave unreadable


def unreadable(path: str, text: str) -> str:
    """Why a Python or JSON file's text cannot be read by its own reader; nothing when it can."""
    try:
        if path.endswith(".py"):
            compile(text, path, "exec")
        else:
            json.loads(text)
    except (SyntaxError, ValueError) as fault:
        return f"{type(fault).__name__}: {fault}"
    return ""


def texts(tool: str, data: dict, path: str) -> tuple:
    """A file's text before an edit and after it; None for one that is not there or cannot be told (the tool answers that
    itself)."""
    file = Path(path)
    try:
        before = file.read_text(encoding="utf-8").replace("\r\n", "\n") if file.is_file() else None
    except (OSError, UnicodeDecodeError):
        return None, None
    if tool == "Write":
        return before, str(data.get("content", ""))
    old, new = str(data.get("old_string", "")).replace("\r\n", "\n"), str(data.get("new_string", "")).replace("\r\n", "\n")
    if tool != "Edit" or before is None or not old or old not in before:
        return before, None
    return before, before.replace(old, new) if data.get("replace_all") else before.replace(old, new, 1)


def left_broken(tool: str, data: dict, path: str) -> str:
    """Why this edit would leave a Python or JSON file unreadable, or a work's note with no state to go on from; nothing
    when it would not, when the file was unreadable before it, or when what the edit leaves cannot be told."""
    noted = port_note.key(path).startswith(port_note.key(port_note.WORK) + "/") and path.endswith(".md")
    if not noted and not path.endswith(SOUND):
        return ""
    before, after = texts(tool, data, path)
    if after is None:
        return ""
    if noted:
        return port_note.now_fault(after)
    return "" if before is not None and unreadable(path, before) else unreadable(path, after)


def after_edit(payload: dict) -> str:
    """What the gates say of a file of the project right after an edit of it (PostToolUse of Edit and Write): the findings no
    open problem of the register names, to be fixed at once, in the code, where they were just written. A defect a reading
    shows is never left for a build or a test to find. Nothing for a file outside the project, a clean one, or gates that
    do not answer in time."""
    if payload.get("tool_name") not in ("Edit", "Write", "MultiEdit"):
        return ""
    written = str((payload.get("tool_input") or {}).get("file_path", "")).replace("\\", "/")
    root = ROOT.as_posix()
    if not written.lower().startswith(root.lower() + "/") or not Path(written).is_file():
        return ""
    inside = written[len(root) + 1:]
    if inside.startswith(("dist/", ".git/")):
        return ""
    try:
        done = subprocess.run([sys.executable, str(ROOT / "tools" / "gates" / "run_gates.py"), "--file", inside, "--register",
                               str(ROOT / "doc" / "problems.jsonl")], cwd=str(ROOT), capture_output=True, text=True,
                              encoding="utf-8", errors="replace", timeout=GATE_SECONDS)
    except (OSError, subprocess.TimeoutExpired):
        return ""
    found = [line for line in done.stdout.splitlines() if line.startswith("NEW ")]
    if done.returncode == 0 or not found:
        return ""
    return (f"The gates of {inside}, read right after this edit ({len(found)} finding(s) no open problem names). Fix each one "
            "now, in the code, before the next file: nothing is built or run to find what a reading shows.\n" + "\n".join(found[:GATE_SHOWN]))
