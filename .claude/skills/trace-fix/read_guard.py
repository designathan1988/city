"""Hooks of the trace-fix skill on reading and editing: one script, called by Claude Code with the event on stdin.

PostToolUse Read: every Read of a file of this repository becomes one record (path, first and last line returned, the
  content's hash and line count, the session), the proof of a reading (proof.py).
PostToolUse Edit, Write: the file the session has just changed itself stays known to it, whole.
SessionStart, source compact: what the session read before the compaction left its context and no longer counts.
PreToolUse Edit, Write: REFUSED (exit 2, the reason goes to Claude) unless
  1. the file to change, when it exists, was read whole by this session since its last compaction, as it is now; and
  2. no file of the repository that this session opened is left read in part.
  The refusal lists the Read calls that are missing, ready to send in parallel.

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
SEEN = ".seen"  # beside a trace's photos: one empty file for each photo or crop the Read tool opened (commands.cmd_close asks for them)


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


def log_edit(event, proof, store):
    """The session's own change: it wrote these lines on a file it held whole, so it still holds the file whole."""
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


def unread_now(relative, seen, store):
    """The line ranges of a file, as it is now, that this reader does not hold."""
    data = store.file_bytes(relative)
    if b"\0" in data[:BINARY_PROBE]:
        return []  # not text: the Read tool gives no lines of it
    held = seen.get(relative, {}).get(hashlib.sha1(data).hexdigest(), [0, []])[1]
    return store.gaps({"ranges": held, "lines": lines_of(data)})


def left_in_part(versions, store):
    """A file is finished once some content of it this reader saw was read from its first line to its last."""
    return all(store.gaps({"ranges": ranges, "lines": total}) for total, ranges in versions.values())


def unrecorded(seen, store, session):
    """(trace, file, first line, last line) of what this reader read on the flow of an open trace it works on (it ran the
    ledger on it), with no note of it."""
    on_flow = {}  # file -> [first open trace that has it on its flow, every note any open trace holds of it]
    for slug in sorted(os.listdir(store.TRACES)) if os.path.isdir(store.TRACES) else []:
        if not os.path.exists(store.ledger_path(slug)):
            continue
        with open(store.ledger_path(slug), encoding="utf-8") as f:
            state = json.load(f)
        if state.get("closed") or session not in state.get("sessions", []):
            continue
        for entry in state.get("files", []):
            if entry["path"] in seen:
                on_flow.setdefault(entry["path"], [slug, []])[1].extend(entry.get("saw", []))
    out = []
    for file, (slug, notes) in sorted(on_flow.items()):  # a reading is recorded once one open trace holds its note
        mine = [r for _, ranges in seen[file].values() for r in ranges]
        out += [(slug, file, a, b) for a, b in store.unnoted(mine, notes)]
    return out


def ignored_by_git(paths, store):
    """The ones of these paths git ignores (builds, fetched dependencies, outputs): not the repository's own files."""
    import subprocess
    # bytes in and out: a text pipe on Windows ends every path with a carriage return, and git then matches none
    done = subprocess.run(["git", "-c", "core.quotepath=off", "check-ignore", "--stdin"], input="\n".join(paths).encode("utf-8"),
                          cwd=store.ROOT, capture_output=True, timeout=20)
    return set(done.stdout.decode("utf-8", "replace").splitlines())


def guard(event, proof, store):
    """0 to let the edit through, REFUSED with the missing Read calls on stderr otherwise."""
    asked = os.path.abspath((event.get("tool_input") or {}).get("file_path") or "")
    if os.path.normcase(asked).startswith(os.path.normcase(os.path.abspath(store.TRACES)) + os.sep):
        return 0  # the trace folder is the memory of the work (a record, an analysis): writing it is never held back
    path, relative = target(event, store)
    seen = proof.seen_by(reader_of(event))
    missing = []
    if path and os.path.isfile(path):
        gaps = unread_now(relative, seen, store)
        if gaps:
            missing.append((relative, gaps, "the file to change"))
    # outputs, builds and vendored code are looked up, not studied: only the repository's own files are read to the end
    opened = [other for other in sorted(seen)
              if other != relative and not set(other.split("/")[:-1]) & store.SKIP_DIRS
              and os.path.isfile(os.path.join(store.ROOT, other)) and left_in_part(seen[other], store)]
    mine = [other for other in opened if not os.path.isabs(other)]  # a reference tree's files are not this repository's to ask git of
    ignored = ignored_by_git(mine, store) if mine else set()
    for other in opened:
        gaps = [] if other in ignored else unread_now(other, seen, store)
        if gaps:
            missing.append((other, gaps, "opened and left in part"))
    bare = unrecorded(seen, store, str(event.get("session_id", "")))
    if bare:
        print("trace-fix: this edit is refused. You read these lines of a trace's flow and recorded nothing of what they do; a "
              "reading that is not written down is lost at the next compaction and has to be done again.\n"
              "Record them now, in one `ledger.py record <file.json>`: for each, `saw` (what it does and calls), `called_by` (who "
              "calls it, file:line, which thread) and `state` (what it reads and writes, which lock), and every problem seen "
              "there with its `fix`. Then send the edit again:\n"
              + "\n".join(f"  trace {slug}: {file} lines {a}-{b}" for slug, file, a, b in bare[:store.BATCH_CHUNKS])
              + (f"\n  ... and {len(bare) - store.BATCH_CHUNKS} more." if len(bare) > store.BATCH_CHUNKS else ""), file=sys.stderr)
        return REFUSED
    if not missing:
        return 0
    calls = []
    for file, gaps, why in missing:
        where = os.path.join(store.ROOT, file).replace("\\", "/")
        cut = [piece for gap in gaps for piece in store.pieces(file, gap[0], gap[1])]
        for a, b in cut:  # a file that fits one call whole is asked for by its path alone
            alone = len(cut) == 1 and a == 1 and b == store.line_count(file)
            calls.append(f"  {where}" + ("" if alone else f" offset={a} limit={b - a + 1}") + f"   ({why})")
    shown = calls[:store.BATCH_CHUNKS]
    print("trace-fix: this edit is refused. A file is read whole, with the Read tool, before it is changed, and every file "
          "opened is read to its end (what was read before a compaction no longer counts).\n"
          "Read these now, one Read call each, all in one message, then send the same edit again:\n" + "\n".join(shown)
          + (f"\n  ... and {len(calls) - len(shown)} more call(s): the next refusal lists them." if len(calls) > len(shown) else ""),
          file=sys.stderr)
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
    import proof  # the one owner of the Read records
    import store  # the one owner of the repository root and the trace folder

    if not os.path.normcase(here).startswith(os.path.normcase(store.ROOT)) and not os.environ.get("TRACE_FIX_DIR"):
        return 0  # a session of another project: this skill's hooks are of the repository it is installed in
    name, tool = event.get("hook_event_name"), event.get("tool_name")
    if name == "SessionStart":
        if event.get("source") == "compact":
            proof.forget(reader_of(event))
    elif name == "PreToolUse" and tool in EDITS:
        return guard(event, proof, store)
    elif tool == "Read":
        asked = os.path.abspath((event.get("tool_input") or {}).get("file_path") or "")
        if asked.lower().endswith(".png") and os.path.normcase(asked).startswith(os.path.normcase(os.path.abspath(store.TRACES)) + os.sep):
            os.makedirs(os.path.join(os.path.dirname(asked), SEEN), exist_ok=True)  # a photo of a trace that was looked at
            open(os.path.join(os.path.dirname(asked), SEEN, os.path.basename(asked)), "w").close()
        else:
            log_read(event, proof, store)
    elif tool in EDITS:
        log_edit(event, proof, store)
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
