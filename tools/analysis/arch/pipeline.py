"""The stages after reading: summaries joined into a program, entry points enumerated, one slice per entry, every class's detector, the queue,
and the files status, next and explain read."""
import collections
import os

from . import catalogue, costmodel, work_queue
from .core import write_json
from .entries import enumerate_entries
from .slices import build_all
from .summaries import Program


def analyse(run):
    prog = Program(run.facts.summaries)
    run.log(f"program: {len(prog.fn)} functions, {len(prog.cls)} classes, {len(prog.binds)} bound, {len(prog.draws)} draws, {len(prog.readers)} readers")
    entries = enumerate_entries(prog, run.api, run.graph)
    run.log(f"entries: {dict(collections.Counter(e['kind'] for e in entries))}")
    slices = build_all(prog, entries)
    findings = catalogue.detect(prog, slices)
    run.log(f"findings: {dict(collections.Counter(f['class'] for f in findings))}")
    queue = work_queue.build(prog, entries, slices, findings, run.history, lambda e, sl: costmodel.cost_vector(prog, e, sl))
    return {"program": prog, "entries": entries, "slices": slices, "findings": findings, "queue": queue}


def publish(run, a, base):
    """Everything status, next and explain read. No time, no absolute path: two runs give the same bytes."""
    found = collections.Counter(f["class"] for f in a["findings"])
    write_json(os.path.join(base, "queue.json"), a["queue"])
    write_json(os.path.join(base, "findings.json"), sorted(a["findings"], key=lambda f: (f["class"], f["file"], f["line"], f["detail"])))
    write_json(os.path.join(base, "entries.json"), a["entries"])
    write_json(os.path.join(base, "coverage.json"), {
        "commit": run.repo.head, "functions": len(a["program"].fn), "classes_of_code": len(a["program"].cls),
        "entries": dict(sorted(collections.Counter(e["kind"] for e in a["entries"]).items())),
        "classes": [{"id": cid, "title": title, "found": found.get(cid, 0)} for cid, title, _ in catalogue.CLASSES],
        "adapters": {k: v for k, v in run.info.items() if k != "refused"}})
    return base
