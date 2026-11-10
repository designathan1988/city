"""What the trace-fix ledger shows: every answer bounded, the long ones as files of the trace.

One owner of the output limit. An answer longer than the harness shows is cut and stored where the session may
not read it, so nothing here prints more than OUTPUT_CAP: the views page (`--from`) and `export` writes the
whole notes and the report as files under the trace's folder, to be read with the Read tool.
"""
import os
import sys
from collections import OrderedDict

import owed
import proof
import store
from store import OPEN

# Characters one command may print. Source: the harness keeps about 30,000 characters of a command's output and
# stores the rest out of reach; 12,000 leaves room for two commands chained in one call.
OUTPUT_CAP = 12000
# Room kept for the closing line that says where the rest is.
TAIL_ROOM = 300
# Rows of the progress table; deeper folders are folded into their parents until it fits.
FOLDER_ROWS = 40
CLIP_HEAD = 700   # the request and the next action, in characters
CLIP_LINE = 110   # a problem or a cause in a one-line list


def clip(text, width):
    text = " ".join(str(text).split())
    return text if len(text) <= width else text[:width - 3] + "..."


class Out:
    """Lines of one answer, kept under the limit; `add` says whether the line fitted."""

    def __init__(self):
        self.lines, self.size, self.full = [], 0, False

    def add(self, text=""):
        if self.full or self.size + len(text) + 1 > OUTPUT_CAP - TAIL_ROOM:
            self.full = True
            return False
        self.lines.append(text)
        self.size += len(text) + 1
        return True

    def flush(self, rest=""):
        if self.full:
            self.lines.append(f"[cut at the output limit. {rest}]".strip())
        try:
            sys.stdout.write("\n".join(self.lines) + "\n")
            sys.stdout.flush()
        except OSError:
            pass  # the reader closed the pipe (`| head`): it has what it asked for


def by_folder(files):
    """Files grouped by folder, folded upwards until the table has at most FOLDER_ROWS rows."""
    depth = max((e["path"].count("/") for e in files), default=0)
    while True:
        groups = OrderedDict()
        for e in files:
            parts = e["path"].split("/")[:-1]
            groups.setdefault("/".join(parts[:depth]) or ".", []).append(e)
        if len(groups) <= FOLDER_ROWS or depth <= 1:
            return groups
        depth -= 1


def head(state, out):
    out.add(f"TRACE {state['slug']} (opened {state['opened']}) phase: {store.phase(state)}"
            + (" | reuse of earlier readings: on" if state.get("reuse") else ""))
    out.add(f"REQUEST: {clip(state['request'], CLIP_HEAD)}")
    if state["note"]:
        out.add(f"NEXT ACTION: {clip(state['note'], CLIP_HEAD)}")


def next_step(state, out):
    now = store.phase(state)
    if now == "trace":
        out.add("DO NOW: put the flow of the symptom on the trace, from its trigger to what the player sees: `trace --query \"...\"` "
                "(one for what the player does, one for what the player sees), `writers`, or `flow <file> --why \"...\"`.")
    elif now == "read":
        batch = store.next_chunks(state)
        bare = [(e["path"], a, b) for e in state["files"] for a, b in store.unnoted(e["ranges"], e["saw"])]
        if bare:
            out.add(f"RECORD NOW: {len(bare)} stretch(es) of the flow were read and nothing was written of them. A reading counts only "
                    "once recorded: one `record <file.json>` with a `saw` for each (who calls it, thread, lock, state read and written, "
                    "what it calls), and the problems seen there. Edits are refused until then.")
            for path, a, b in bare:
                if not out.add(f"  {path} lines {a}-{b}"):
                    break
        if not batch:
            return
        lines = sum(b - a + 1 for _, a, b in batch)
        out.add(f"DO NOW: read the next batch ({len(batch)} chunk(s), {lines} lines): one Read call each, in parallel.")
        for entry, a, b in batch:
            known = proof.prior_note(entry["path"], state["slug"]) if state.get("reuse") and os.path.isfile(
                os.path.join(store.ROOT, entry["path"])) else None
            on_route = [f"{u[2]}@{u[0]}" for u in entry.get("units", ()) if u[0] <= b and u[1] >= a]
            out.add(f"  {os.path.join(store.ROOT, entry['path']).replace(chr(92), '/')} offset={a} limit={b - a + 1}"
                    + (f"  [on the route: {clip(', '.join(on_route), 160)}]" if on_route else "")
                    + (f"  [read in trace {known['trace']}, unchanged: `notes {entry['path']} --prior`, then `carry`]"
                       if known else ""))
        out.add("  then record them in one call, before the next batch: `record <file.json>` (reads with their `saw`, docs of every outside "
                "call, problems with their `fix`, new flow files, the note), and `next`.")
    elif now == "fix":
        left = sum(1 for p in state["problems"] if p["status"] == OPEN)
        blind = [p["id"] for p in state["problems"] if p["status"] == OPEN and not p.get("fix")]
        if blind:
            out.add(f"PLAN FIRST: {len(blind)} open problem(s) have no fix planned ({', '.join(blind[:20])}): `plan P<n> --fix \"what "
                    "changes, where, who else to check, the test\"` for each, from the notes and the code, before changing anything.")
        out.add(f"DO NOW: fix the {left} open problem(s) in the code, at their cause (`card C<n>` gives a cause's exact lines, callers and "
                "tests); `fix <P or C> --how \"...\"` as each is done; test at the end.")
    elif now == "done":
        out.add("DO NOW: every problem is fixed. Run the tests of what changed, then `close`.")


def progress(state, out):
    files = state["files"]
    done = sum(1 for e in files if not store.gaps(e))
    saws = [s for e in files for s in e["saw"]]
    word = sum(1 for s in saws if not s.get("proved") and not s.get("carried"))
    carried = sum(1 for s in saws if s.get("carried"))
    out.add(f"READ: {done}/{len(files)} files, {sum(store.lines_read(e) for e in files)}/"
            f"{sum(e['lines'] for e in files)} lines | DOCUMENTATION: {len(state['docs'])} call(s) looked up")
    if word or carried:
        out.add(f"  ({word} chunk(s) stand on the session's word, recorded before the Read proof; {carried} carried "
                "over from another trace)")
    for folder, group in by_folder(files).items():
        full = sum(1 for e in group if not store.gaps(e))
        mark = "done" if full == len(group) else f"{full}/{len(group)} files"
        out.add(f"  {folder}: {mark}, {sum(store.lines_read(e) for e in group)}/{sum(e['lines'] for e in group)} lines")


def findings(state, out):
    problems, causes = state["problems"], state["causes"]
    open_n = sum(1 for p in problems if p["status"] == OPEN)
    out.add(f"PROBLEMS: {open_n} open of {len(problems)} | CAUSES: "
            f"{sum(1 for c in causes if c['status'] == OPEN)} open of {len(causes)}")
    for c in causes:
        mine = [p for p in problems if p["cause_id"] == c["id"]]
        out.add(f"  {c['id']} [{c['status']}] {len(mine)} problem(s) | owner {clip(c['owner'], 40)} | "
                f"{clip(c['what'], CLIP_LINE)}")
    loose = [p for p in problems if p["status"] == OPEN and not p["cause_id"]]
    for i, p in enumerate(loose):
        if not out.add(f"  {p['id']} {p['path']}:{p['line']} {clip(p['what'], CLIP_LINE)}"):
            return f"{len(loose) - i} more open problem(s): `problems --from {i + 1}`"
    return ""


def owed_block(out, session):
    """What a session owes before any edit (owed.py), added to an answer; nothing for a session that works on no trace."""
    lines = owed.owed(session) if session else []
    if lines:
        out.add(f"OWED by this session before any edit ({len(lines)}; the edit hook refuses every edit until it is done). "
                + owed.explain())
        for line in owed.shown(lines):
            if not out.add(line):
                break


def show(state, banner="", owed_session=""):
    out = Out()
    if banner:
        out.add(banner)
        out.add()
    head(state, out)
    owed_block(out, owed_session)
    next_step(state, out)
    out.add()
    progress(state, out)
    out.add()
    rest = findings(state, out)
    out.add()
    out.add("More: `notes <file or folder>` (what was read there), `problems` (full text), `export` (the flow and the problems "
            "whole, as files, with the Read calls that take them in).")
    out.flush(rest or "The whole is in the files `export` writes.")


def matching(state, prefix):
    prefix = store.rel(prefix).rstrip("/")
    return [e for e in state["files"] if prefix in (".", "") or e["path"] == prefix or e["path"].startswith(prefix + "/")]


def notes(state, prefix, start, prior):
    out = Out()
    files = matching(state, prefix)
    if not files:
        store.fail(f"nothing on the flow at {prefix}")
    for i, e in enumerate(files[start - 1:], start):
        rows = [f"{i}. {e['path']} [{store.lines_read(e)}/{e['lines']}] on the flow because: {clip(e['why'], 160)}"]
        rows += [f"     {s['from']}-{s['to']}: {s['text']}" for s in e["saw"]]
        if prior:
            note = proof.prior_note(e["path"], state["slug"])
            rows += ["     no reading of this file, as it is now, in another trace"] if not note else \
                [f"     read in trace {note['trace']} ({note['t']}), file unchanged since:"] + \
                [f"       {s['from']}-{s['to']}: {s['text']}" for s in note["saw"]] + \
                [f"       problem there: {p['path']}:{p['line']} {clip(p['what'], 200)}" for p in note["problems"]]
        if out.size + sum(len(r) + 1 for r in rows) > OUTPUT_CAP - TAIL_ROOM and i > start:
            out.full = True
            out.flush(f"Continue: `notes {prefix} --from {i}`" + (" --prior" if prior else ""))
            return
        for r in rows:
            out.add(r)
        if out.full:
            out.flush(f"File {i} is cut here (whole in the files of `export`). Continue: `notes {prefix} --from {i + 1}`")
            return
    out.flush()


def problems(state, everything, start):
    out = Out()
    rows = [p for p in state["problems"] if everything or p["status"] == OPEN]
    out.add(f"{len(rows)} problem(s)" + ("" if everything else " open") + f", from {start}:")
    for i, p in enumerate(rows[start - 1:], start):
        text = [f"{i}. {p['id']} [{p['status']}{', ' + p['cause_id'] if p['cause_id'] else ''}] {p['path']}:{p['line']}",
                f"     what: {p['what']}", f"     cause seen: {p['cause']}"]
        if p["symptom"]:
            text.append(f"     symptom: {p['symptom']}")
        if p.get("fix"):
            text.append(f"     fix planned: {p['fix']}")
        if p["how"]:
            text.append(f"     {p['status']}: {p['how']} {p.get('commit', '')}".rstrip())
        if out.size + sum(len(r) + 1 for r in text) > OUTPUT_CAP - TAIL_ROOM and i > start:
            out.full = True
            out.flush(f"Continue: `problems --from {i}`" + (" --all" if everything else ""))
            return
        for r in text:
            out.add(r)
        if out.full:
            out.flush(f"Problem {i} is cut here (whole in the files of `export`). Continue: `problems --from {i + 1}`")
            return
    out.flush()
