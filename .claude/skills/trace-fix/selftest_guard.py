"""The self-test's checks of the trace-fix hooks (read_guard.py, owed.py), of which trace a session works on, and of the photo
crops: an edit refused until the lines it replaces are read (a file written over: the whole of it), a file opened in part
that stops nothing, a reference tree outside the repository, what a session owes the trace it works on now before it edits
(its readings recorded by itself, the code it read on a flow, its problems planned, the engine's documentation looked up),
the session-start hook, and the crops.
Run by selftest.py, which owns the throwaway folders and the ways to call the ledger and the hook."""
import glob
import json
import os

CONTEXT7 = "mcp__plugin_context7_context7__query-docs"  # the tool's name as Claude Code gives it to a hook


def write_lines(path, count, text="line"):
    with open(path, "w", encoding="utf-8") as f:
        f.write("".join(f"{text} {i}\n" for i in range(1, count + 1)))


def run(check, hook, tool_read, ledger, edited, files, big, root, work, traces, reference, late):
    def edit(path, session="guard", old=None, **top):
        """An Edit of `old` in the file; with no `old`, a call that names no text: the file written over."""
        return hook("PreToolUse", "Edit", path, session, old=old, **top)

    # ---- an edit is refused until the lines it replaces are read; a file written over, until it is read whole (a session on no trace)
    check("a file never read is not written over: the refusal gives the Read call to make",
          (edit(edited), "edited.txt" in hook.err, "the file to change" in hook.err), (2, True, True))
    check("a new file is written freely", hook("PreToolUse", "Write", os.path.join(files, "new.txt"), "guard"), 0)
    tool_read(edited, session="another")
    check("another session's reading is not this one's", edit(edited), 2)
    tool_read(edited, 1, 10, session="guard")
    check("an edit of lines it read goes through, the rest of the file unread", edit(edited, old="line 5\n"), 0)
    check("an edit of lines it did not read is refused, with those lines alone to read",
          (edit(edited, old="line 25\nline 26\n"), "offset=25 limit=2" in hook.err, "the lines this edit replaces" in hook.err), (2, True, True))
    check("an edit of a text the file does not hold is left to the Edit tool", edit(edited, old="no such line\n"), 0)
    check("a file read in part is not written over: the refusal names the lines left", (edit(edited), "offset=11 limit=20" in hook.err), (2, True))
    tool_read(edited, 11, 20, session="guard")
    check("read whole, it is written over", edit(edited), 0)
    tool_read(big, 1, 400, session="guard")
    check("the lines left of a big file come in chunks", (edit(big), "offset=401 limit=400" in hook.err, "offset=801 limit=100" in hook.err),
          (2, True, True))
    doc = os.path.join(root, "CLAUDE.md")
    tool_read(doc, 1, 50, session="guard")
    check("a file opened and left in part stops no edit elsewhere", edit(edited), 0)
    with open(edited, "a", encoding="utf-8") as f:
        f.write("line 31\n")
    check("the session's own change keeps the file known", (hook("PostToolUse", "Edit", edited, "guard"), edit(edited)), (0, 0))
    with open(edited, "a", encoding="utf-8") as f:
        f.write("line 32\n")
    check("a file someone else changed is read again", edit(edited), 2)
    tool_read(edited, session="guard")
    check("read again, the edit goes through", edit(edited), 0)
    check("a compaction takes the reading away", (hook("SessionStart", None, edited, "guard", source="compact"), edit(edited)), (0, 2))
    check("a broken event never stops a tool", hook("PreToolUse", "Edit", None, "guard"), 0)

    # ---- a reference tree outside the repository is followed like the repository's own files
    tool_read(edited, session="watcher")
    tool_read(os.path.join(work, "elsewhere.txt"), 1, 5, session="watcher")
    check("a file outside the repository and the reference trees is not followed", edit(edited, "watcher"), 0)
    lab = os.path.join(reference, "lab.ts")
    tool_read(lab, 1, 5, session="watcher")
    check("a reference file opened in part stops no edit (a session on no trace owes nothing more)", edit(edited, "watcher"), 0)

    # ---- the trace a session works on: its commands go there with no prefix, whatever another session works on
    check("a session opens its own trace", ledger("open", "owed", "--request", "r", session="worker"), 0)
    check("another session opens another", ledger("open", "other", "--request", "r2", session="someone"), 0)
    check("each session's commands go to its own trace",
          (ledger("status", session="worker"), "TRACE owed" in ledger.out, ledger("status", session="someone"), "TRACE other" in ledger.out),
          (0, True, 0, True))
    check("at a session's start or compaction the hook gives it its own trace",
          (ledger("resume", "--hook", stdin=json.dumps({"session_id": "worker"}), session=""), "THIS SESSION WORKS ON TRACE owed" in ledger.out),
          (0, True))
    check("a session on no trace is shown the open traces, never told to continue one",
          (ledger("resume", "--hook", stdin=json.dumps({"session_id": "newcomer"}), session=""), "open traces" in ledger.out,
           "THIS SESSION" in ledger.out), (0, True, False))

    # ---- what a session that works on a trace owes before any edit
    ledger("flow", late, "--why", "called by small", session="worker")
    tool_read(late, session="worker")
    tool_read(edited, session="worker")
    check("a reading of the flow with no note of its own refuses every edit",
          (edit(edited, "worker"), "RECORD" in hook.err, "late.txt lines 1-22" in hook.err), (2, True, True))
    check("status says the same", (ledger("status", session="worker"), "OWED" in ledger.out), (0, True))
    check("the trace folder is always writable", hook("PreToolUse", "Write", os.path.join(traces, "owed", "rec.json"), "worker"), 0)
    ledger("switch", "owed", session="someone")
    ledger("read", late, "1", "22", "--saw", "another session's note", session="someone")
    check("a note another session wrote is not this one's", edit(edited, "worker"), 2)
    check("its own note settles it", (ledger("read", late, "1", "22", "--saw", "twenty-two lines", session="worker"), edit(edited, "worker")),
          (0, 0))
    tool_read(lab, session="worker")
    check("code it read on no flow refuses every edit", (edit(edited, "worker"), "FLOW" in hook.err, "lab.ts" in hook.err), (2, True, True))
    check("taken off with why, the edit goes through", (ledger("off", lab, "--why", "another feature's file", session="worker"),
                                                        edit(edited, "worker")), (0, 0))
    rules = os.path.join(reference, "lab_rules.ts")
    write_lines(rules, 30)
    tool_read(rules, session="worker")
    check("a reference file goes on the flow and is noted like the repository's own",
          (ledger("flow", rules, "--why", "the behaviour to reach", session="worker"),
           ledger("read", rules, "1", "30", "--saw", "what the game must do", session="worker"), edit(edited, "worker")), (0, 0, 0))
    ledger_file = os.path.join(traces, "owed", "ledger.json")
    with open(ledger_file, encoding="utf-8") as f:
        state = json.load(f)
    state["problems"].append({"id": f"P{len(state['problems']) + 1}", "path": os.path.relpath(edited, root).replace("\\", "/"), "line": 3,
                              "what": "w", "cause": "c", "symptom": "", "status": "open", "cause_id": "", "fix": "", "how": "", "commit": ""})
    with open(ledger_file, "w", encoding="utf-8") as f:
        json.dump(state, f)  # a problem recorded before a fix plan was asked for
    pid = state["problems"][-1]["id"]
    check("an open problem in the file to change with no fix planned refuses the edit", (edit(edited, "worker"), f"plan {pid}" in hook.err),
          (2, True))
    check("planned, the edit goes through", (ledger("plan", pid, "--fix", "change line 3; test: the count", session="worker"),
                                             edit(edited, "worker")), (0, 0))
    engine = os.path.join(files, "engine.gd")
    write_lines(engine, 3, "# a script")
    tool_read(engine, session="worker")
    check("a file that calls the engine is not changed before its documentation is looked up",
          (edit(engine, "worker"), "DOCS" in hook.err), (2, True))
    check("a context7 lookup settles it", (hook("PostToolUse", CONTEXT7, None, "worker", tool_input={"libraryId": "/godotengine/godot-docs",
                                                                                                     "query": "Node"}),
                                           edit(engine, "worker")), (0, 0))
    hook("SessionStart", None, None, "worker", source="compact")
    tool_read(engine, session="worker")
    check("a lookup made before a compaction still counts: its answer is in the record", edit(engine, "worker"), 0)
    plain = os.path.join(files, "plain.cpp")
    write_lines(plain, 3, "// pure C++")
    tool_read(plain, session="worker")
    check("C++ that includes no engine header asks for no lookup", edit(plain, "worker"), 0)
    vendored = sorted(glob.glob(os.path.join(root, "native", "godot-cpp", "include", "godot_cpp", "core", "*.hpp")))
    if vendored:  # `someone` works on the same trace and looked nothing up
        tool_read(engine, session="someone")
        check("another session of the trace owes its own lookup", (edit(engine, "someone"), "DOCS" in hook.err), (2, True))
        tool_read(vendored[0], 1, 5, session="someone")
        check("the engine's vendored source read is a lookup", edit(engine, "someone"), 0)

    # ---- a session is held to the trace it works on now: one it left asks nothing more of it
    check("a session opens a trace and reads a file of its flow",
          (ledger("open", "left", "--request", "r3", session="mover"), ledger("flow", late, "--why", "called by small", session="mover")), (0, 0))
    tool_read(late, session="mover")
    tool_read(edited, session="mover")
    check("it owes that trace the note of what it read", (edit(edited, "mover"), "trace left" in hook.err), (2, True))
    check("once it works on another trace, the one it left asks nothing",
          (ledger("open", "taken", "--request", "r4", session="mover"), edit(edited, "mover")), (0, 0))
    ledger("fix", pid, "--how", "the test's own problem", session="worker")
    shots = os.path.join(traces, "owed", "photos")
    os.makedirs(shots)
    open(os.path.join(shots, "00-view.png"), "wb").close()
    check("a trace closes only with its photos opened", ledger("close", session="worker"), 1)
    check("a change that cannot reach the screen closes with its reason", ledger("close", "--no-photos", "a text file", session="worker"), 0)

    for made in (engine, plain):
        os.remove(made)

    # ---- photos are cut into enlarged tiles
    try:
        from PIL import Image
    except ImportError:
        Image = None
    if Image:
        tiles = os.path.join(work, "shots")
        os.makedirs(tiles)
        Image.new("RGB", (1920, 1080), (40, 90, 60)).save(os.path.join(tiles, "a.png"))
        check("crops cuts a photo into its tiles", (ledger("crops", tiles), len(os.listdir(os.path.join(tiles, "crops")))), (0, 6))
        with Image.open(os.path.join(tiles, "crops", "a-r1c1.png")) as tile:
            check("a tile is enlarged and overlaps its neighbour", tile.size, (1408, 1188))
        check("crops of nothing fails", ledger("crops", os.path.join(work, "none")), 1)
