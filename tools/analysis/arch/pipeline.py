"""The stages after reading: summaries joined into a program, entry points enumerated, one slice per entry, every class's detector, the queue,
and the files status, next and explain read."""
import collections
import os

from . import catalogue, costmodel, work_queue
from .contracts import field_name
from .core import digest, write_json
from .entries import display, enumerate_entries
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
    write_json(os.path.join(base, "slices.json"), slices_of(a, run.api.values))
    return base


WRITES_KEPT = 60  # members named per slice: enough to say what state a flow touches


def slices_of(a, values=frozenset()):
    """Each entry's slice as a route: its functions in program order, each with the caller and line that first reaches it, and the members the
    whole run writes; and for every member of the program's state, the functions that read it and the functions that write it, as the compiler
    resolved each access; and the classes of the outside each function calls. Functions are named once, in a table the rest indexes."""
    prog, slices = a["program"], a["slices"]
    ids = sorted(prog.fn)  # every function, run by an entry or not: a writer no entry reaches is still a writer
    at = {f: i for i, f in enumerate(ids)}
    table = [[display(prog, f), prog.fn[f]["file"], prog.fn[f]["line"], prog.fn[f]["end"]] for f in ids]
    out = []
    for e in a["entries"]:
        sl = slices[e["id"]]
        reached = [[at[sl.first[f][-1][0]], sl.first[f][-1][1]] if sl.first[f] else None for f in sl.order]
        out.append({"id": digest(e["id"]), "entry": e["name"], "kind": e["kind"], "freq": e["freq"], "why": e["why"], "via": e.get("via", []),
                    "order": [at[f] for f in sl.order], "reached": reached, "jobs": len(sl.submits),
                    "writes": sorted(field_name(prog, m) for m in prog.fields_of(sl.written))[:WRITES_KEPT]})
    members = {}
    for i, f in enumerate(ids):
        reads, writes, _ = prog.direct(prog.fn[f])
        for kind, bits in (("r", reads), ("w", writes)):
            for m in prog.fields_of(bits):
                members.setdefault(field_name(prog, m), {"r": [], "w": []})[kind].append(i)
        for m in sorted({e[3] for e in prog.fn[f]["ev"] if e[0] == "wp"}):  # written in one of its parts: a writer of the whole
            row = members.setdefault(field_name(prog, m), {"r": [], "w": []})["w"]
            if i not in row:
                row.append(i)
    outside = {}
    for i, f in enumerate(ids):  # the classes of the outside (the engine, a library) a function calls, value types of the caller's memory left out
        owners = sorted({e[3][4:].split(".", 1)[0] for e in prog.fn[f]["ev"] if e[0] == "call" and e[3].startswith("ext:")} - set(values) - {""})
        if owners:
            outside[str(i)] = owners
    return {"functions": table, "slices": out, "members": dict(sorted(members.items())), "outside": outside}
