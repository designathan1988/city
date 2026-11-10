"""The self-test's checks of the trace-fix hooks (read_guard.py) and of the photo crops: an edit refused until its file is read
whole, a file opened and left in part, a reference tree outside the repository, a reading that counts only once recorded.
Run by selftest.py, which owns the throwaway folders and the ways to call the ledger and the hook."""
import os


def run(check, hook, tool_read, ledger, edited, files, big, root, work, traces, reference, late):
    # ---- the edit guard: a file is read whole before it is changed, and every file opened is read to its end
    def edit(path, session="guard"):
        return hook("PreToolUse", "Edit", path, session)

    check("an edit of a file never read is refused, with the Read call to make",
          (edit(edited), "edited.txt" in hook.err, "the file to change" in hook.err), (2, True, True))
    check("a new file is written freely", hook("PreToolUse", "Write", os.path.join(files, "new.txt"), "guard"), 0)
    tool_read(edited, session="another")
    check("another session's reading is not this one's", edit(edited), 2)
    tool_read(edited, 1, 10, session="guard")
    check("a file read in part is not read: the refusal names the lines left",
          (edit(edited), "offset=11 limit=20" in hook.err), (2, True))
    tool_read(edited, 11, 20, session="guard")
    check("read whole, the edit goes through", edit(edited), 0)
    tool_read(big, 1, 400, session="guard")
    check("the lines left of a big file come in chunks", (edit(big), "offset=401 limit=400" in hook.err, "offset=801 limit=100" in hook.err),
          (2, True, True))
    check("an output left in part does not stop an edit elsewhere", edit(edited), 0)
    doc = os.path.join(root, "CLAUDE.md")
    tool_read(doc, 1, 50, session="guard")
    check("a file of the repository opened and left in part stops every edit",
          (edit(edited), "CLAUDE.md" in hook.err, "opened and left in part" in hook.err), (2, True, True))
    tool_read(doc, 51, 400, session="guard")
    check("read to its end, the edit goes through", edit(edited), 0)
    with open(edited, "a", encoding="utf-8") as f:
        f.write("line 31\n")
    check("the session's own change keeps the file known",
          (hook("PostToolUse", "Edit", edited, "guard"), edit(edited)), (0, 0))
    with open(edited, "a", encoding="utf-8") as f:
        f.write("line 32\n")
    check("a file someone else changed is read again", edit(edited), 2)
    tool_read(edited, session="guard")
    check("read again, the edit goes through", edit(edited), 0)
    check("a compaction takes the reading away",
          (hook("SessionStart", None, edited, "guard", source="compact"), edit(edited)), (0, 2))
    check("a broken event never stops a tool", hook("PreToolUse", "Edit", None, "guard"), 0)
    tool_read(edited, session="reader")
    tool_read(os.path.join(work, "elsewhere.txt"), 1, 5, session="reader")
    check("a file outside the repository and the reference trees is not followed", edit(edited, "reader"), 0)
    tool_read(os.path.join(reference, "lab.ts"), 1, 5, session="reader")
    check("a reference file opened and left in part stops every edit",
          (edit(edited, "reader"), "lab.ts offset=6 limit=25" in hook.err), (2, True))
    tool_read(os.path.join(reference, "lab.ts"), 6, 25, session="reader")
    check("the reference file read to its end, the edit goes through", edit(edited, "reader"), 0)

    # ---- a reading counts only once it is recorded
    check("a trace of its own for it", (ledger("open", "unwritten", "--request", "r"), ledger("flow", late, "--why", "w")), (0, 0))
    tool_read(late, session="reader")
    check("the flow read and nothing written: the trace still asks for the record",
          (ledger("status"), "RECORD NOW" in ledger.out, "phase: read" in ledger.out), (0, True, True))
    check("and every edit is refused until it is", (edit(edited, "reader"), "recorded nothing" in hook.err, "late.txt lines 1-22" in hook.err),
          (2, True, True))
    check("the record itself can always be written", hook("PreToolUse", "Write", os.path.join(traces, "unwritten", "rec.json"), "reader"), 0)
    check("recorded, the trace moves on and the edit goes through",
          (ledger("read", late, "1", "22", "--saw", "twenty-two lines, called by nobody"), edit(edited, "reader")), (0, 0))
    check("a change that cannot reach the screen closes with its reason", ledger("close", "--no-photos", "a text file"), 0)

    # ---- photos are cut into enlarged tiles
    try:
        from PIL import Image
    except ImportError:
        Image = None
    if Image:
        shots = os.path.join(work, "shots")
        os.makedirs(shots)
        Image.new("RGB", (1920, 1080), (40, 90, 60)).save(os.path.join(shots, "a.png"))
        check("crops cuts a photo into its tiles", (ledger("crops", shots), len(os.listdir(os.path.join(shots, "crops")))), (0, 6))
        with Image.open(os.path.join(shots, "crops", "a-r1c1.png")) as tile:
            check("a tile is enlarged and overlaps its neighbour", tile.size, (1408, 1188))
        check("crops of nothing fails", ledger("crops", os.path.join(work, "none")), 1)
