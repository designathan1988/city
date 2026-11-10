"""The trace as files on disk, for the trace-fix skill: the flow with every chunk's note (notes.md) and the problems, causes and
fixes (report.md), written whole after every command (ledger.py) and wrapped so any chunk of them fits one Read call.
"""
import os
import textwrap

import store
from store import OPEN
from views import Out


def wrapped(rows):
    """The rows as text with no line longer than store.WRAP: a chunk of the file is then bounded like any source file."""
    out = []
    for row in rows:
        out += textwrap.wrap(str(row), store.WRAP, subsequent_indent="  ", break_long_words=False, break_on_hyphens=False) or [""]
    return "\n".join(out) + "\n"


def report_md(state):
    rows = [f"# Trace {state['slug']}", "", f"Request: {state['request']}", "",
            f"Opened {state['opened']}; closed {state.get('closed') or 'no'}.",
            f"Files read in whole: {sum(1 for e in state['files'] if not store.gaps(e))} of {len(state['files'])}.", ""]

    def listed(items):
        out = []
        for p in items:
            out += [f"### {p['id']} [{p['status']}] {p['path']}:{p['line']}", f"- Problem: {p['what']}", f"- Cause seen: {p['cause']}"]
            out += [f"- Symptom: {p['symptom']}"] if p["symptom"] else []
            out += [f"- Fix planned: {p['fix']}"] if p.get("fix") else ["- Fix planned: NONE YET (`plan`)"] if p["status"] == OPEN else []
            out += [f"- Fix made: {(p['how'] + ' ' + p.get('commit', '')).strip()}"] if p["how"] else []
            out.append("")
        return out

    for c in state["causes"]:
        rows += [f"## {c['id']} [{c['status']}] {c['what']}", "", f"Owner: {c['owner']}", "", f"Plan: {c['plan']}", ""]
        if c.get("how"):
            rows += [f"Fixed: {c['how']} {c.get('commit', '')}".rstrip(), ""]
        rows += listed([p for p in state["problems"] if p["cause_id"] == c["id"]])
    loose = [p for p in state["problems"] if not p["cause_id"]]
    if loose:
        rows += ["## Problems without a cause", ""] + listed(loose)
    return wrapped(rows)


def notes_md(state):
    rows = [f"# The flow of trace {state['slug']}, in the order the code runs: what each chunk read does", "",
            f"Request: {state['request']}", ""]
    for i, e in enumerate(state["files"], 1):
        rows += [f"## {i}. {e['path']} ({store.lines_read(e)}/{e['lines']} lines)", "",
                 f"On the flow because: {e['why']}", ""]
        rows += [f"- {s['from']}-{s['to']}: {s['text']}" for s in e["saw"]] + [""]
    if state["off"]:
        rows += ["## Taken off the flow", ""] + [f"- {o['path']} (was there because: {o['was']}): {o['why']}" for o in state["off"]] + [""]
    if state["docs"]:
        rows += ["## Documentation consulted", ""] + [f"- {d['api']} ({d['source']}): {d['says']}" for d in state["docs"]] + [""]
    return wrapped(rows)


def read_calls(path):
    """The Read calls that take a file of the trace in whole, each within the Read tool's size."""
    inside = store.rel(path)
    total = store.line_count(inside)
    cut = store.pieces(inside, 1, total) if total else []
    return [f"  {path}" + ("" if len(cut) == 1 else f" offset={a} limit={b - a + 1}") for a, b in cut]


def write_files(state):
    """The trace as two files, whole: the flow with every chunk's note (notes.md) and the problems, causes and fixes (report.md).
    Written after every command (ledger.py), so what was found is on disk in full at any moment; returns (path, lines) of each."""
    folder, done = store.trace_folder(state["slug"]), []
    for name, text in (("report.md", report_md(state)), ("notes.md", notes_md(state))):
        path = os.path.join(folder, name)
        with open(path + ".tmp", "w", encoding="utf-8", newline="\n") as f:
            f.write(text)
        os.replace(path + ".tmp", path)
        done.append((path.replace(chr(92), "/"), text.count(chr(10))))
    return done


def export(state):
    out = Out()
    out.add("Written whole. Read them with these calls (each within the Read tool's size), in parallel:")
    for path, _lines in write_files(state):
        for call in read_calls(path):
            out.add(call)
    out.flush()
