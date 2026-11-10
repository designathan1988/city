"""The changes a trace accepts, for the trace-fix skill: a file put on the flow, a chunk recorded as read (only where the
Read tool returned it), a problem recorded where it was read, what the
documentation says of an outside call, problems put under a cause. Each changes the state it is
given or raises Refused with what to do instead; nothing is saved here, so a batch can be tried whole (commands.py).
"""
import glob
import os

import proof
import store
from store import OPEN, Refused


def do_flow(state, paths, why, after=None):
    if not str(why or "").strip():
        raise Refused("a file on the flow says why it is there: --why \"who calls it, which data it reads or writes\"")
    found = []
    for item in paths:
        full = item if os.path.isabs(item) else os.path.join(store.ROOT, item)
        if os.path.isdir(full):
            for base, dirs, names in os.walk(full):
                dirs[:] = sorted(d for d in dirs if d not in store.SKIP_DIRS)
                found += [os.path.join(base, n) for n in sorted(names)
                          if os.path.splitext(n)[1] in store.SOURCE_EXT]
        elif os.path.isfile(full):
            found.append(full)
        else:
            hits = [h for h in sorted(glob.glob(full, recursive=True)) if os.path.isfile(h)]
            if not hits:
                raise Refused(f"nothing at {item}")
            found += hits
    at = len(state["files"])
    if after:
        anchor = store.find_file(state, after)
        if not anchor:
            raise Refused(f"--after {after}: that file is not on the flow")
        at = state["files"].index(anchor) + 1
    added = []
    for full in found:
        path = store.rel(full)
        if path.startswith(".."):
            raise Refused(f"{path} is outside the repository")
        if store.find_file(state, path):
            continue
        state["files"].insert(at, {"path": path, "lines": store.line_count(path), "why": str(why).strip(),
                                   "ranges": [], "saw": []})
        at += 1
        added.append(path)
    return added


def do_read(state, path, start, end, saw, called_by="", uses=""):
    entry = store.find_file(state, path)
    if not entry:
        raise Refused(f"{store.rel(path)} is not on the flow: add it with `flow <path> --why \"who calls it\"`")
    if not all(str(v or "").strip() for v in (saw, called_by, uses)):
        raise Refused(f"{entry['path']} {start}-{end}: a chunk is recorded with the three things a fix needs later, so the file is not "
                      "opened again to find them: `saw` (what each function there does and what it calls), `called_by` (who calls "
                      "it or what triggers it, with file:line, and on which thread) and `state` (what it reads and writes, under "
                      "which lock). For a document or data: who reads it, and what it sets")
    saw = f"{str(saw).strip()} CALLED BY: {str(called_by).strip()} STATE: {str(uses).strip()}"
    start, end = int(start), int(end)
    entry["lines"] = store.line_count(entry["path"])
    if not (1 <= start <= end <= entry["lines"]):
        raise Refused(f"{entry['path']}: range {start}-{end} outside 1-{entry['lines']}")
    if end - start + 1 > store.CHUNK_LINES:
        raise Refused(f"{entry['path']}: a chunk is at most {store.CHUNK_LINES} lines: read and record it in pieces")
    missing = proof.unproven(entry["path"], start, end)
    if missing:
        raise Refused(
            f"{entry['path']} lines " + ", ".join(f"{a}-{b}" for a, b in missing) + " were not returned by the Read "
            f"tool for the file as it is now. Read them with the Read tool (at most {store.CHUNK_LINES} lines a call; "
            "cat, sed, grep and a subagent's reading do not count; a file changed since is read again), then record.")
    entry["ranges"] = store.merged(entry["ranges"] + [[start, end]])
    entry["saw"].append({"from": start, "to": end, "text": str(saw).strip(), "t": store.now(), "proved": True})
    return entry


def do_problem(state, where, what, cause, symptom="", of="", fix=""):
    path, _, line = str(where).rpartition(":")
    if not path or not line.isdigit():
        raise Refused(f"{where}: a problem is recorded at <file>:<line>")
    entry = store.find_file(state, path)
    if not entry:
        raise Refused(f"{store.rel(path)} is not on the flow: a problem is recorded where it was read")
    if not all(str(v or "").strip() for v in (what, cause, fix)):
        raise Refused(f"{where}: a problem says what it is (what), the design decision behind it (cause) and the fix planned (fix: what "
                      "will change, where, who else must be checked, the test that will prove it), while the code is in front of you")
    if of:
        of = store.cause_of(state, of)["id"]
    pid = f"P{len(state['problems']) + 1}"
    state["problems"].append({"id": pid, "path": entry["path"], "line": int(line), "what": str(what).strip(),
                              "cause": str(cause).strip(), "symptom": str(symptom or "").strip(), "status": OPEN,
                              "cause_id": of or "", "fix": str(fix or "").strip(), "how": "", "commit": "", "found": store.now()})
    return pid


def do_plan(state, pid, fix):
    """The fix planned for a problem recorded before one was asked, or planned again after more reading."""
    if not str(fix or "").strip():
        raise Refused("the fix planned says what will change, where, who else must be checked and the test that will prove it")
    problem = store.problem_of(state, pid)
    problem["fix"] = str(fix).strip()
    return problem


def do_off(state, path, why):
    """Takes off the flow a file the tracer brought by a name alone: no call and no data of this problem reaches it."""
    entry = store.find_file(state, path)
    if not entry:
        raise Refused(f"{store.rel(path)} is not on the flow")
    if not str(why or "").strip():
        raise Refused("a file leaves the flow with what shows that no call or data of this problem reaches it (--why)")
    if any(p["path"] == entry["path"] for p in state["problems"]):
        raise Refused(f"{entry['path']} has a problem recorded on it: it is of this flow")
    state["files"].remove(entry)
    state["off"].append({"path": entry["path"], "why": str(why).strip(), "was": entry["why"], "t": store.now()})
    return entry


def do_doc(state, api, source, says):
    """What the official documentation says of one outside call (an engine's, a library's) the flow or the fix uses."""
    if not all(str(v or "").strip() for v in (api, source, says)):
        raise Refused("a documentation note names the call (api), where it was read (source: context7 id, URL or vendored file) and "
                      "what it says (says: guarantees, thread, cost, known issues)")
    state["docs"].append({"api": str(api).strip(), "source": str(source).strip(), "says": str(says).strip(), "t": store.now()})


def do_link(state, cid, pids):
    cause = store.cause_of(state, cid)
    for pid in pids:
        store.problem_of(state, pid)["cause_id"] = cause["id"]
    return cause


def keep_notes(state, entries):
    for entry in entries:
        proof.keep_note(state, entry)
