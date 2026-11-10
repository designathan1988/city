"""What a session owes before it changes a file, for the trace-fix skill: read by the edit hook (read_guard.py), which refuses
the edit while anything is owed, and shown by `status` and `resume` (resume.py).

A session that works on a trace (it opened it, switched to it or ran a command on it) is held to that trace's record:
- RECORD: every line of the flow it read since its last compaction has a note it wrote itself (saw, called_by, state). A
  note an earlier session wrote does not count: what this session understood is lost at its next compaction otherwise.
- FLOW: every code file it read (the repository's or a reference tree's) is on the flow of one of its traces, or taken off
  one with why (`off`): a reading on no flow is a reading nobody will find again.
- PLAN: an open problem recorded in the file it changes has its fix planned.
- DOCS: a file that calls the engine (a script, a shader, a scene, C++ that includes godot-cpp or Terrain3D) is changed only
  after the official documentation was looked up since its last compaction: context7's query-docs (the hook keeps each
  lookup), or the engine's own vendored source read with the Read tool.
A session that works on no trace owes nothing here.
"""
import os
import time

import proof
import sessions
import store

# The files a reading of which is a reading of code (the flow is made of them); documents and data are read for context.
CODE = {".h", ".hpp", ".cpp", ".c", ".inc", ".gd", ".gdshader", ".gdshaderinc", ".py", ".ts", ".js"}
# A file that calls the engine: Godot's own formats, or C++ that includes the engine's or Terrain3D's headers (CLAUDE.md, the
# layers: sim/ is pure C++ without Godot; render/ and api/ include godot-cpp).
ENGINE_FILES = {".gd", ".gdshader", ".gdshaderinc", ".tscn", ".tres"}
ENGINE_INCLUDES = (b"godot_cpp/", b"terrain_3d")
VENDORED = ("native/godot-cpp/", "project/addons/terrain_3d/")  # the engine's and Terrain3D's sources, in the tree
LOOKUP = "L"  # a documentation lookup's record is named L<ns>-<pid>.json among the reader's Read records (named <ns>-<pid>)
SHOWN = 40  # debts listed at once; the rest are counted


def session_of(reader):
    """A hook's reader is the session, with an agent's id after a colon: the agent works for its session."""
    return reader.split(":")[0]


def record_lookup(reader, tool_input):
    """A documentation lookup (context7's query-docs), kept as the proof the documentation was read."""
    row = {"t": time.strftime("%Y-%m-%d %H:%M:%S"), "library": str(tool_input.get("libraryId") or tool_input.get("context7CompatibleLibraryID") or ""),
           "query": str(tool_input.get("query") or tool_input.get("topic") or "")}
    proof.write_whole(os.path.join(proof.reader_folder(reader), f"{LOOKUP}{time.time_ns()}-{os.getpid()}.json"), row)


def looked_up(reader, rows):
    """Whether this reader read the engine's documentation since its last compaction."""
    if any(r["path"].startswith(VENDORED) for r in rows if r.get("by") != proof.EDIT):
        return True
    folder, since = proof.reader_folder(reader), proof.since_of(reader)
    return os.path.isdir(folder) and any(n.startswith(LOOKUP) and n.endswith(".json") and int(n[1:].split("-")[0]) > since
                                         for n in os.listdir(folder))


def calls_engine(path, content=b""):
    ext = os.path.splitext(path)[1].lower()
    if ext in ENGINE_FILES:
        return True
    if ext not in CODE:
        return False
    full = os.path.join(store.ROOT, path)
    data = content + (store.file_bytes(path) if os.path.isfile(full) else b"")
    return any(mark in data for mark in ENGINE_INCLUDES)


def is_code(path):
    return (os.path.splitext(path)[1].lower() in CODE and not path.startswith(".claude/")
            and not set(path.split("/")[:-1]) & store.SKIP_DIRS)


def owed(reader, relative=None, content=b""):
    """What this reader must do before an edit of `relative` (None: before any edit): lines to show, empty when nothing is owed."""
    session = session_of(reader)
    traces = sessions.open_traces(session)
    if not traces:
        return []
    rows = proof.rows_of(reader)
    read = {}  # what the Read tool returned to this reader since its compaction (its own edits are not readings)
    for row in rows:
        if row.get("by") != proof.EDIT:
            read.setdefault(row["path"], []).append([row["from"], row["to"]])
    flows, taken_off = {}, set()
    for slug, state in traces:
        taken_off.update(o["path"] for o in state.get("off", []))
        for entry in state["files"]:
            flows.setdefault(entry["path"], [slug, []])[1].extend(s for s in entry.get("saw", []) if s.get("session") == session)
    out = []
    for path, ranges in sorted(read.items()):
        if path in flows:
            slug, notes = flows[path]
            # a file may have lost lines since they were read (the tree taken back to an older commit): what is no longer there
            # can be given no note (`record` refuses a range past the file's end), so only the lines that stand are owed
            standing = ranges
            if os.path.isfile(os.path.join(store.ROOT, path)):
                last = store.line_count(path)
                standing = [[a, min(b, last)] for a, b in ranges if a <= last]
            out += [f"  RECORD {path} lines {a}-{b} (trace {slug}): read, with no note of yours" for a, b in store.unnoted(standing, notes)]
        elif is_code(path) and path not in taken_off:
            out.append(f"  FLOW   {path}: code you read, on no flow of your traces: `flow` it (who calls it) and record it, or `off` "
                       "it with why")
    if relative:
        for slug, state in traces:
            out += [f"  PLAN   {p['id']} at {relative}:{p['line']} (trace {slug}): `plan {p['id']} --fix \"what changes, where, who else "
                    "to check, the test\"`" for p in state["problems"]
                    if p["status"] == store.OPEN and p["path"] == relative and not p.get("fix")]
        if calls_engine(relative, content) and not looked_up(reader, rows):
            out.append(f"  DOCS   {relative} calls the engine: look up the calls you change first (context7: resolve-library-id, then "
                       "query-docs; or Read the vendored source in native/godot-cpp or project/addons/terrain_3d), and write what it "
                       "says in the record's `docs`")
    return out


def shown(lines):
    """The debts as the hook and the ledger print them: the first SHOWN, then how many more."""
    return lines[:SHOWN] + ([f"  ... and {len(lines) - SHOWN} more (`status` lists them)"] if len(lines) > SHOWN else [])


def explain():
    return ("RECORD: one `record <file.json>`, each read with saw (what it does and calls), called_by (who calls it, file:line, "
            "which thread) and state (what it reads and writes, which lock), every problem with its fix, and the note. "
            "FLOW: `flow <file> --why` then record it, or `off <file> --why`. PLAN: `plan`. DOCS: context7 or the vendored source.")
