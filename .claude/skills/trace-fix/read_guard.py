"""Hooks of the trace-fix skill on reading and editing: one script, called by Claude Code with the event on stdin.

PostToolUse Read: every Read of a file of this repository or of a reference tree becomes one record (path, first and last
  line returned, the content's hash and line count, the session), the proof of a reading (proof.py); a photo of a trace
  that is opened leaves a mark beside it, which the trace's close asks for (photos.py).
PostToolUse Edit, Write: the file the session has just changed itself stays known to it.
PostToolUse of context7's query-docs: the lookup is kept, the proof the documentation was read (owed.py).
SessionStart, source compact: what the session read before the compaction left its context and no longer counts.
PreToolUse Edit, Write: REFUSED (exit 2, the reason goes to Claude) unless
  1. the lines the edit replaces (where its old text stands in the file, as it is now) were read by this session since its
     last compaction; a file written over whole, when it exists, was read whole; and
  2. it owes nothing to the trace it works on (owed.py: its readings recorded by itself, the code it read on a flow, the
     plan of the problems in the file, the documentation of a file that calls the engine).
  A file opened in part stops nothing: what was understood of it is in the trace's notes, and only what is changed is read
  again. The refusal lists everything missing at once. The trace folder (records, analysis) is always writable.

Only the PreToolUse answer can stop a tool; a failure of the hook itself never does (it is kept in
read_guard_errors.txt beside this file). A session of another project is left alone.
"""
import sys

sys.dont_write_bytecode = True  # the skill's files are swapped in place: no stale bytecode beside them

import hashlib  # noqa: E402
import json  # noqa: E402
import os  # noqa: E402
import time  # noqa: E402

# The Read tool's default window when no limit is given. Source: the Read tool's description ("up to 2000 lines").
READ_DEFAULT_LIMIT = 2000
EDITS = ("Edit", "Write", "MultiEdit")  # the tools that change a file by its path (`file_path`)
REFUSED = 2  # Claude Code's hooks reference: exit code 2 of a PreToolUse hook blocks the call and gives stderr to Claude
# How much of a file is looked at to tell text from binary. Source: git's own rule (a NUL in the first 8000 bytes).
BINARY_PROBE = 8000
SEEN = ".seen"  # beside a trace's photos: one empty file for each photo the Read tool opened (photos.not_looked_at)
DOCS_TOOLS = ("query-docs", "get-library-docs")  # context7's tool that returns documentation (its MCP server's names)


def lines_of(data):
    return data.count(b"\n") + (1 if data and not data.endswith(b"\n") else 0)


def reader_of(event):
    """Who read: the session, and inside it each agent by itself (an agent's reading is not its caller's)."""
    return str(event.get("session_id", "")) + (":" + str(event["agent_id"]) if event.get("agent_id") else "")


def target(event, store):
    """(absolute path, name in the records) of the file a tool call names: a file of this repository or of a reference
    tree (store.place); (None, None) anywhere else."""
    path = (event.get("tool_input") or {}).get("file_path")
    if not path:
        return None, None
    name = store.place(path)
    return (None, None) if name is None else (os.path.join(store.ROOT, name), name)


def in_traces(path, store):
    return os.path.normcase(os.path.abspath(path)).startswith(os.path.normcase(os.path.abspath(store.TRACES)) + os.sep)


def log_read(event, proof, store):
    path, relative = target(event, store)
    if not path or not os.path.isfile(path):
        return
    with open(path, "rb") as f:
        data = f.read()
    total = lines_of(data)
    tool_input = event.get("tool_input") or {}
    response = event.get("tool_response")
    shown = response.get("file") if isinstance(response, dict) else None
    if isinstance(shown, dict) and shown.get("numLines") is not None:
        if "content" not in shown:
            return  # an image, a PDF or a notebook: not lines of text
        start, count = int(shown.get("startLine") or 1), int(shown["numLines"])
    else:
        start, count = int(tool_input.get("offset") or 1), int(tool_input.get("limit") or READ_DEFAULT_LIMIT)
    start = max(start, 1)
    end = min(start + count - 1, total)
    if end < start:
        return
    proof.write_read({"t": time.strftime("%Y-%m-%d %H:%M:%S"), "session": event.get("session_id", ""), "reader": reader_of(event),
                      "path": relative, "from": start, "to": end, "total": total, "sha": hashlib.sha1(data).hexdigest(),
                      "ns": time.time_ns()})


def log_photo(asked):
    """A photo or crop of a trace opened with the Read tool: an empty mark beside it."""
    folder = os.path.join(os.path.dirname(asked), SEEN)
    os.makedirs(folder, exist_ok=True)
    open(os.path.join(folder, os.path.basename(asked)), "w").close()


def log_edit(event, proof, store):
    """The session's own change: the file as it left it stays known to it (an edit names the exact text it replaces)."""
    path, relative = target(event, store)
    if not path or not os.path.isfile(path):
        return
    with open(path, "rb") as f:
        data = f.read()
    total = lines_of(data)
    if total:
        proof.write_read({"t": time.strftime("%Y-%m-%d %H:%M:%S"), "session": event.get("session_id", ""), "reader": reader_of(event),
                          "path": relative, "from": 1, "to": total, "total": total, "sha": hashlib.sha1(data).hexdigest(),
                          "ns": time.time_ns(), "by": proof.EDIT})


def replaced_lines(data, tool_input):
    """The line ranges of a file an edit replaces: where each text it names stands (every place of it when it replaces all).
    None for a call that names no text to replace (a Write over the file): the whole of it. A text the file does not hold
    asks for nothing here: the Edit tool itself refuses it."""
    edits = tool_input.get("edits") or [tool_input]
    if not all(isinstance(e, dict) and isinstance(e.get("old_string"), str) and e["old_string"] for e in edits):
        return None
    text = data.decode("utf-8", "replace").replace("\r\n", "\n")
    out = []
    for e in edits:
        old = e["old_string"].replace("\r\n", "\n")
        at = text.find(old)
        while at >= 0:
            first = text.count("\n", 0, at) + 1
            out.append([first, first + old.count("\n") - (1 if old.endswith("\n") else 0)])
            at = text.find(old, at + len(old)) if e.get("replace_all") else -1
    return out


def unread_calls(relative, path, tool_input, seen, store):
    """The Read calls this reader still has to make before this edit: the lines it replaces, of the file as it is now (the
    whole file when it is written over)."""
    if not path or not os.path.isfile(path):
        return []
    data = store.file_bytes(relative)
    if b"\0" in data[:BINARY_PROBE]:
        return []  # not text: the Read tool gives no lines of it
    held = seen.get(relative, {}).get(hashlib.sha1(data).hexdigest(), [0, []])[1]
    unheld = store.gaps({"ranges": held, "lines": lines_of(data)})
    asked = replaced_lines(data, tool_input)
    why = "the file to change" if asked is None else "the lines this edit replaces"
    if asked is not None:  # of what it does not hold, only what stands in the lines it replaces
        unheld = store.merged([[max(a, c), min(b, d)] for a, b in unheld for c, d in asked if max(a, c) <= min(b, d)])
    where = os.path.join(store.ROOT, relative).replace("\\", "/")
    cut = [piece for gap in unheld for piece in store.pieces(relative, gap[0], gap[1])]
    calls = []
    for a, b in cut:  # a file that fits one call whole is asked for by its path alone
        alone = len(cut) == 1 and a == 1 and b == store.line_count(relative)
        calls.append(f"  {where}" + ("" if alone else f" offset={a} limit={b - a + 1}") + f"   ({why})")
    return calls


def guard(event, proof, store, owed):
    """0 to let the edit through; REFUSED with everything missing on stderr otherwise."""
    tool_input = event.get("tool_input") or {}
    if tool_input.get("file_path") and in_traces(tool_input["file_path"], store):
        return 0  # the trace folder is the memory of the work (a record, an analysis): writing it is never held back
    reader = reader_of(event)
    path, relative = target(event, store)
    calls = unread_calls(relative, path, tool_input, proof.seen_by(reader), store)
    content = str(tool_input.get("content") or tool_input.get("new_string") or "").encode("utf-8")
    debts = owed.owed(reader, relative, content)
    if not calls and not debts:
        return 0
    text = ["trace-fix: this edit is refused. Do all of this first, then send the same edit again."]
    if calls:
        text.append("READ, one Read call each, all in one message (the lines an edit replaces are read before it, as the file is "
                    "now; a file written over is read whole; what was read before a compaction no longer counts):")
        text += calls[:store.BATCH_CHUNKS] + ([f"  ... and {len(calls) - store.BATCH_CHUNKS} more call(s): the next refusal lists them."]
                                              if len(calls) > store.BATCH_CHUNKS else [])
    if debts:
        text.append("OWED to your trace (python .claude/skills/trace-fix/ledger.py; `status` lists the same). " + owed.explain())
        text += owed.shown(debts)
    print("\n".join(text), file=sys.stderr)
    return REFUSED


def main():
    try:
        event = json.load(sys.stdin)
    except (json.JSONDecodeError, ValueError):
        return 0
    cwd = event.get("cwd")
    if cwd and os.path.isdir(cwd):
        os.chdir(cwd)
    here = os.path.dirname(os.path.abspath(__file__))
    sys.path.insert(0, here)
    import owed  # what a session owes its traces
    import proof  # the one owner of the Read records
    import store  # the one owner of the repository root and the trace folder

    if not os.path.normcase(here).startswith(os.path.normcase(store.ROOT)) and not os.environ.get("TRACE_FIX_DIR"):
        return 0  # a session of another project: this skill's hooks are of the repository it is installed in
    name, tool = event.get("hook_event_name"), str(event.get("tool_name") or "")
    asked = str((event.get("tool_input") or {}).get("file_path") or "")
    if name == "SessionStart":
        if event.get("source") == "compact":
            proof.forget(reader_of(event))
    elif name == "PreToolUse" and tool in EDITS:
        return guard(event, proof, store, owed)
    elif tool == "Read" and asked.lower().endswith(".png") and in_traces(asked, store):
        log_photo(asked)
    elif tool == "Read":
        log_read(event, proof, store)
    elif tool in EDITS:
        log_edit(event, proof, store)
    elif "context7" in tool and tool.endswith(DOCS_TOOLS):
        owed.record_lookup(reader_of(event), event.get("tool_input") or {})
    return 0


if __name__ == "__main__":
    try:
        code = main()
    except Exception:  # noqa: BLE001 - a hook that breaks never stops the tool it watches; the failure is kept, not hidden
        import traceback
        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "read_guard_errors.txt"), "a", encoding="utf-8") as f:
            f.write(time.strftime("%Y-%m-%d %H:%M:%S") + "\n" + traceback.format_exc() + "\n")
        code = 0
    sys.exit(code)
