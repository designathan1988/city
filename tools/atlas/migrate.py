"""Every open item of the trackers kept before the register, entered once into it (specification 001 US3, T043, T050):
the trace ledgers under dist/trace (each open problem with its file, its cause or the reading the ledger planned, the
time it was found), the structured rows of log/pendencias.md (one problem per id; a row its last column calls done, or
marked a wrong row, is history and stays out), and the rows of log/specification.md's divergences and slowness the
method has not settled. Each enters through the register's one writer (problems.Register.add) with its origin
(`ledger:<slug>:<id>`, `log/pendencias.md:<line> <id>`, `log/specification.md:<line> <id>`); an origin already in the
register is passed over, so the migration may run again and adds nothing twice. A row citing a ledger's problem (trace
`<slug>` P<n>, ledger P<n>) is related to it. `--evidence` moves the photos an open problem's words name from its trace's
folder to dist/evidence/<id>/ and names them as its evidence. The counts per origin are printed.

    python tools/atlas/migrate.py [--evidence] [--dry-run]
"""
import argparse
import json
import os
import re
import shutil
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import areas  # noqa: E402
from atlas import problems as register  # noqa: E402

LEDGERS = "dist/trace"
EVIDENCE = "dist/evidence"
PENDING = "log/pendencias.md"
SPECIFICATION = "log/specification.md"
OPEN = ("open",)
TITLE = 100                                  # the most characters of a title made from a longer text
ROW = re.compile(r"^\|\s*([A-Z]+-?\d+)\s*\|(.*)\|\s*$")
CARRIED = "## Carried from before"           # where log/pendencias.md's structured rows end
DONE = re.compile(r"\bdone\b|\bWRONG ROW\b", re.I)
NOT_KNOWN = re.compile(r"^\s*(unknown|not read|not found|to read|\?|-|none)\b", re.I)
CITED = re.compile(r"(?:trace `([\w-]+)`\s*|ledger\s+)(P\d+)")
PATH = re.compile(r"`((?:native|project|tools|log)/[^`\s:]+)")
PHOTO = re.compile(r"[\w./-]+\.png")
DEFAULT_TRACE = "port-from-nov7"             # the trace a row means when it cites a ledger problem by id alone
NO_OWNER = "unknown"                         # an item that names no file: its owner is found with its cause
BY_PREFIX = {                                # a row's id prefix: (its area when no path says, its kind)
    "R": ("simulation", "copy"), "S": ("simulation", "defect"), "F": ("product", "missing-feature"),
    "M": ("performance", "performance"), "V": ("render", "defect"), "D": ("docs", "process"),
    "I": ("process", "process"), "A": ("tools", "architecture"), "T": ("tools", "missing-feature"),
    "L": ("process", "process"),
}
SETTLED = re.compile(r"^\s*to fix", re.I)    # a divergence of log/specification.md that CLAUDE.md settled (US1)


def short(text):
    plain = re.sub(r"[`*]", "", " ".join(str(text).split()))
    return plain if len(plain) <= TITLE else plain[:TITLE - 1].rsplit(" ", 1)[0] + "…"


def stamp(found):
    """A ledger's time ('2026-11-10 09:15') as the register writes it."""
    return found.replace(" ", "T")[:16] if found else None


def ledger_items(root):
    """(origin, fields, opened) of every open problem of every trace ledger."""
    base = os.path.join(root, LEDGERS)
    for slug in sorted(os.listdir(base)) if os.path.isdir(base) else ():
        path = os.path.join(base, slug, "ledger.json")
        if not os.path.isfile(path):
            continue
        with open(path, encoding="utf-8") as fh:
            ledger = json.load(fh)
        for p in ledger.get("problems") or []:
            if p.get("status") not in OPEN:
                continue
            what, symptom = p.get("what") or "", p.get("symptom") or ""
            cause = p.get("cause") or ""
            fields = {"title": short(what or symptom), "symptom": what + (f" ({symptom})" if symptom and symptom != what else ""),
                      "owner": p.get("path") or NO_OWNER, "area": areas.area_of(p.get("path") or ""),
                      "kind": "performance" if "performance" in slug else "defect",
                      "criterion": f"the symptom gone where the trace `{slug}` found it, checked as it planned: {p.get('fix') or 'a photo or a test of the place'}",
                      "spec": f"the {areas.area_of(p.get('path') or '')} specification"}
            if cause and not NOT_KNOWN.match(cause):
                fields["cause"] = cause
            else:
                fields["reading"] = f"{cause} {p.get('fix') or ''}".strip() or f"read {p.get('path')}"
            yield f"ledger:{slug}:{p['id']}", fields, stamp(p.get("found"))


def cells(line):
    return [c.strip() for c in line.strip().strip("|").split("|")]


def pending_items(root):
    """(origin, fields, related ledger origins) of the structured rows of log/pendencias.md not done."""
    path = os.path.join(root, PENDING)
    if not os.path.isfile(path):
        return
    with open(path, encoding="utf-8") as fh:
        lines = fh.read().split("\n")
    for number, line in enumerate(lines, 1):
        if line.startswith(CARRIED):
            break
        match = ROW.match(line)
        if not match:
            continue
        row = cells(line)
        if len(row) < 5 or DONE.search(row[4]) or DONE.search(row[1]):
            continue
        pid, symptom, cause, tried, resolved = row[:5]
        prefix = re.match(r"[A-Z]+", pid).group()
        area, kind = BY_PREFIX.get(prefix, ("process", "defect"))
        named = PATH.search(line)
        fields = {"title": f"{pid}: {short(symptom)}", "symptom": symptom, "criterion": resolved or "the row's symptom gone",
                  "area": areas.area_of(named.group(1)) if named else area, "kind": kind,
                  "owner": named.group(1) if named else NO_OWNER, "spec": f"the {area} specification"}
        if cause and not NOT_KNOWN.match(cause):
            fields["cause"] = cause
        else:
            fields["reading"] = f"what was tried: {tried}" if tried else "read the code the row names"
        cited = [f"ledger:{slug or DEFAULT_TRACE}:{p}" for slug, p in CITED.findall(line)]
        yield f"{PENDING}:{number} {pid}", fields, cited


def specification_items(root):
    """(origin, fields, related) of log/specification.md's divergences still open and its causes of slowness."""
    path = os.path.join(root, SPECIFICATION)
    if not os.path.isfile(path):
        return
    with open(path, encoding="utf-8") as fh:
        lines = fh.read().split("\n")
    for number, line in enumerate(lines, 1):
        match = ROW.match(line)
        if not match or match.group(1)[0] not in "DS":
            continue
        row, pid = cells(line), match.group(1)
        if pid.startswith("D") and len(row) >= 5:
            if SETTLED.match(row[4]):
                continue
            fields = {"title": f"{pid}: {short(row[2])} ({short(row[1])})", "symptom": f"{row[1]} says {row[2]}; true: {row[3]}",
                      "reading": row[4], "area": "docs", "kind": "process", "criterion": "the document says what is true",
                      "owner": NO_OWNER, "spec": "the documents' specification"}
        elif pid.startswith("S") and len(row) >= 4:
            fields = {"title": f"{pid}: {short(row[1])}", "symptom": row[1], "cause": row[2], "area": "process",
                      "kind": "process", "criterion": "the step takes seconds again, its cause gone, measured",
                      "owner": NO_OWNER, "spec": "specification 001 (US3, US5, US7)"}
        else:
            continue
        cited = [f"ledger:{slug or DEFAULT_TRACE}:{p}" for slug, p in CITED.findall(line)]
        yield f"{SPECIFICATION}:{number} {pid}", fields, cited


def migrate(reg, root):
    """Every item into the register once; the counts added and passed over, by origin."""
    have = {o: p for p in reg.problems for o in (p.get("origin") or [])}
    counts = {}
    pending = []
    for source, items in (("ledgers", ((o, f, None, w) for o, f, w in ledger_items(root))),
                          (PENDING, ((o, f, c, None) for o, f, c in pending_items(root))),
                          (SPECIFICATION, ((o, f, c, None) for o, f, c in specification_items(root)))):
        added = passed = 0
        for origin, fields, cited, when in items:
            if origin in have:
                passed += 1
                continue
            fields = {k: v for k, v in fields.items() if v not in (None, "")}
            problem = reg.add(origin=[origin], opened_at=when, traced_at=when, **fields)
            have[origin] = problem
            pending.append((problem, cited or []))
            added += 1
        counts[source] = (added, passed)
    for problem, cited in pending:
        related = [have[o]["id"] for o in cited if o in have and have[o] is not problem]
        if related:
            problem["related"] = list(dict.fromkeys((problem.get("related") or []) + related))
    return counts


def move_evidence(reg, root, dry_run=False):
    """The photos an open problem migrated from a trace names, moved to dist/evidence/<id>/ and named as its evidence."""
    moved = 0
    for problem in reg.problems:
        slugs = [o.split(":")[1] for o in problem.get("origin") or [] if o.startswith("ledger:")]
        if not slugs or problem.get("status") in register.FINAL:
            continue
        words = " ".join(str(problem.get(k) or "") for k in ("symptom", "reading", "criterion", "cause"))
        for name in sorted(set(os.path.basename(n) for n in PHOTO.findall(words))):
            for here, _dirs, files in os.walk(os.path.join(root, LEDGERS, slugs[0])):
                if name not in files:
                    continue
                target = os.path.join(EVIDENCE, problem["id"], name).replace("\\", "/")
                if not dry_run:
                    os.makedirs(os.path.join(root, EVIDENCE, problem["id"]), exist_ok=True)
                    shutil.copy2(os.path.join(here, name), os.path.join(root, target))
                    problem["evidence"] = list(dict.fromkeys((problem.get("evidence") or []) + [target]))
                moved += 1
                break
    return moved


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--evidence", action="store_true", help="move the photos open problems name to dist/evidence/<id>/")
    ap.add_argument("--dry-run", action="store_true", help="count, write nothing")
    ap.add_argument("--root", default=register.ROOT, help=argparse.SUPPRESS)
    a = ap.parse_args(argv)
    reg = register.Register(root=a.root)
    counts = migrate(reg, a.root)
    for source, (added, passed) in counts.items():
        print(f"{source}: {added} added, {passed} already in the register")
    if a.evidence:
        print(f"photos kept as evidence: {move_evidence(reg, a.root, a.dry_run)}")
    if not a.dry_run:
        reg.save()
    return 0


if __name__ == "__main__":
    sys.exit(main())
