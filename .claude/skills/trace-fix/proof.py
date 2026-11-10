"""Proof and memory of a reading, for the trace-fix skill.

Proof: every Read tool call of this repository is one record under dist/trace/reads/<key of the path>/, written by
the read_log.py hook; a chunk counts as read only where these records cover it, for the file as it is now.
Memory: the notes of a file read in whole are kept by the hash of its content under dist/trace/notes/, so another
trace may carry them over (`carry`) while the file has not changed.
"""
import hashlib
import json
import os
import time

import store

READS = "reads"
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


def write_read(row):
    """One Read tool call as a record of its own: parallel Reads never share a file, so none is torn or lost."""
    write_whole(os.path.join(reads_folder(row["path"]), f"{time.time_ns()}-{os.getpid()}.json"), row)


def proven(path):
    """Line ranges of a file, as it is now, that the Read tool returned in calls of at most CHUNK_LINES."""
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
        if row.get("path") == path and row.get("sha") == sha and row["to"] - row["from"] + 1 <= store.CHUNK_LINES:
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
