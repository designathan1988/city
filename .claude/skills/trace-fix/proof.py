"""Proof and memory of a reading, for the trace-fix skill.

Proof: every Read tool call of this repository is one record under dist/trace/reads/<key of the path>/, written by
the read_guard.py hook; a line counts as read only where these records cover it, for the file as it is now. The same
record is kept under dist/trace/readers/<session>/, with the session's own edits: what one session holds of each file
(`seen_by`), which is what its edits are allowed by.
Memory: the notes of a file read in whole are kept by the hash of its content under dist/trace/notes/, so another
trace may carry them over (`carry`) while the file has not changed.
"""
import hashlib
import json
import os
import re
import time

import store

READS = "reads"
READERS = "readers"
SINCE = "since.json"  # a reader's last compaction: what it read before left its context
EDIT = "edit"  # a record of the reader's own change: it knows the file whole, and it is not a reading of the flow
NOTES = "notes"


def reads_folder(path):
    """Where the Read records of one repository path are kept."""
    return os.path.join(store.TRACES, READS, hashlib.sha1(path.encode("utf-8")).hexdigest()[:16])


def write_whole(path, value):
    """A JSON file written under a private name and renamed into place: a reader never sees half of it."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    tmp = f"{path}.{os.getpid()}.tmp"
    with open(tmp, "w", encoding="utf-8") as f:
        json.dump(value, f, ensure_ascii=False)
    os.replace(tmp, path)


def reader_folder(reader):
    """Where what one session (or one of its agents) read and changed is kept."""
    return os.path.join(store.TRACES, READERS, re.sub(r"[^A-Za-z0-9_.-]", "_", reader) or "_")


def write_read(row):
    """One Read tool call (or one edit) as a record of its own: parallel calls never share a file, so none is torn or lost."""
    name = f"{time.time_ns()}-{os.getpid()}.json"
    write_whole(os.path.join(reads_folder(row["path"]), name), row)
    if row.get("reader"):
        write_whole(os.path.join(reader_folder(row["reader"]), name), row)


def forget(reader):
    """A compaction: what this reader read before now is no longer in its context."""
    write_whole(os.path.join(reader_folder(reader), SINCE), {"ns": time.time_ns()})


def seen_by(reader):
    """What a reader holds since its last compaction: {path: {content hash: [lines of that content, merged ranges]}}."""
    folder = reader_folder(reader)
    if not os.path.isdir(folder):
        return {}
    since = 0
    if os.path.exists(os.path.join(folder, SINCE)):
        with open(os.path.join(folder, SINCE), encoding="utf-8") as f:
            since = json.load(f)["ns"]
    out = {}
    for name in os.listdir(folder):
        if not name[0].isdigit() or not name.endswith(".json") or int(name.split("-")[0]) <= since:
            continue  # the compaction mark, a record still being written, or a reading the compaction took away
        with open(os.path.join(folder, name), encoding="utf-8") as f:
            row = json.load(f)
        version = out.setdefault(row["path"], {}).setdefault(row["sha"], [row.get("total", row["to"]), []])
        version[1].append([row["from"], row["to"]])
    for versions in out.values():
        for version in versions.values():
            version[1] = store.merged(version[1])
    return out


def proven(path, since="", since_ns=0):
    """Line ranges of a file, as it is now, that the Read tool returned, from a moment on."""
    folder = reads_folder(path)
    if not os.path.isdir(folder):
        return []
    sha = store.sha_of(path)
    out = []
    for name in os.listdir(folder):
        if not name.endswith(".json"):
            continue  # a record still being written: it counts once it is renamed into place
        with open(os.path.join(folder, name), encoding="utf-8") as f:
            row = json.load(f)
        if row.get("path") == path and row.get("sha") == sha and row.get("by") != EDIT and (
                row["ns"] > since_ns if since_ns and "ns" in row else row.get("t", "") > since):
            out.append([row["from"], row["to"]])
    return store.merged(out)


def unproven(path, start, end):
    """The lines of start..end the Read tool did not return."""
    out, at = [], start
    for a, b in proven(path):
        if b < at:
            continue
        if a > end:
            break
        if a > at:
            out.append([at, a - 1])
        at = b + 1
    if at <= end:
        out.append([at, end])
    return out


def absorb(state):
    """Counts as read every line of the flow the Read tool returned since this trace opened (for the file as it is now): the reading is recorded by reading, with no command of its own; a reading made for another trace is not this one's. What is absorbed stays read when the file is edited later."""
    changed = 0
    for entry in state["files"]:
        if not store.gaps(entry) or not os.path.isdir(reads_folder(entry["path"])) or not os.path.isfile(os.path.join(store.ROOT, entry["path"])):
            continue
        entry["lines"] = store.line_count(entry["path"])
        before = store.lines_read(entry)
        entry["ranges"] = store.merged(entry["ranges"] + [[a, min(b, entry["lines"])] for a, b in proven(entry["path"], state.get("since") or state["opened"], state.get("since_ns", 0)) if a <= entry["lines"]])
        changed += store.lines_read(entry) != before
    return changed


def note_path(sha):
    return os.path.join(store.TRACES, NOTES, sha + ".json")


def keep_note(state, entry):
    """The notes and problems of a file read in whole, kept by the hash of the content they describe."""
    if store.gaps(entry) or not entry["saw"]:
        return
    write_whole(note_path(store.sha_of(entry["path"])), {
        "path": entry["path"], "trace": state["slug"], "t": store.now(), "saw": entry["saw"],
        "problems": [p for p in state["problems"] if p["path"] == entry["path"]]})


def prior_note(path, slug):
    """What another trace read of this file, if the file is still the one it read."""
    where = note_path(store.sha_of(path))
    if not os.path.exists(where):
        return None
    with open(where, encoding="utf-8") as f:
        note = json.load(f)
    return note if note.get("trace") != slug and note.get("path") == path else None
