"""The inventory's reports and views (tools/atlas/inventory.py): dist/inventory/inventory.json (every feature and pair, the
summary, the evidence's counts, what was left unextracted and why, where the unclassified come from) and inventory.md (per
area: the paired rows side by side, then what only the reference has and what only this game has, each with its source),
written whole and refused past the folder's bound (tools/tasks/suites.json lifecycle, read through tools/tasks/outputs.py);
the summary and the feature lines printed on the console."""
import json
import os
import sys
from pathlib import Path

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from tasks import files, outputs  # noqa: E402

OUT = "dist/inventory"
NAMES = ("inventory.json", "inventory.md")
CELL_CHARS = 90


class TooLarge(Exception):
    """The reports would pass the bound their folder is declared with."""


def kinds(features):
    counts = {}
    for f in features:
        counts[f["kind"]] = counts.get(f["kind"], 0) + 1
    return ", ".join(f"{k} {n}" for k, n in sorted(counts.items()))


def _cell(text, limit=CELL_CHARS):
    text = " ".join(str(text).split()).replace("|", "\\|")
    return text if len(text) <= limit else text[:limit - 3] + "..."


def _one(f):
    label = f' "{_cell(f["label"])}"' if f["label"] else ""
    return f"{f['kind']} `{_cell(f['name'])}`{label} ({f['source']})"


def _alone(features):
    out = []
    for kind in sorted({f["kind"] for f in features}):
        mine = [f for f in features if f["kind"] == kind]
        out += ["", f"#### {kind} ({len(mine)})", ""]
        for f in mine:
            what = f" - {_cell(f['description'], 2 * CELL_CHARS)}" if f["description"] else ""
            out.append(f"- {_one(f)}{what}")
    return out


def _head(doc):
    langs = doc["languages"]
    out = ["# Inventory: the reference and this game", "",
           f"Written by `python tools/atlas/cli.py inventory` (tools/atlas/inventory.py) at {doc['generated']}, read anew "
           f"from the reference at `{doc['roots']['reference']}` and from this repository: do not edit.", "",
           f"- Files read: reference {doc['files']['reference']}, city {doc['files']['city']}; seconds: "
           + ", ".join(f"{k} {v}" for k, v in doc["seconds"].items()),
           f"- Languages: reference {', '.join(langs['reference'])}; city {', '.join(langs['city'])}; both "
           f"{', '.join(langs['shared'])}",
           "- Evidence (pairs carrying each): " + ", ".join(f"{k} {v}" for k, v in doc["evidence"].items()), "",
           "| Area | Reference | paired | only | City | paired | only |", "|---|---|---|---|---|---|---|"]
    for area, row in doc["summary"].items():
        r, c = row["reference"], row["city"]
        out.append(f"| {area} | {r['features']} | {r['paired']} | {r['only']} | {c['features']} | {c['paired']} | {c['only']} |")
    out += ["", "## Not extracted, and why", ""] + [f"- {g['game']}: {g['what']}: {g['count']}" for g in doc["unextracted"]]
    out += ["", "## Unclassified places (no word of the area table names them)", ""]
    out += [f"- {u['game']} {u['kind']} `{u['place'] or '.'}`: {u['count']}" for u in doc["unclassified"]]
    return out


def markdown(doc):
    """The report, per area: paired rows (shown in the reference feature's area), reference only, city only."""
    by_id = {f["id"]: f for f in doc["features"]}
    paired = {p["reference"] for p in doc["pairs"]} | {p["city"] for p in doc["pairs"]}
    out = _head(doc)
    for area in doc["summary"]:
        rows = [p for p in doc["pairs"] if by_id[p["reference"]]["area"] == area]
        out += ["", f"## {area}", "", f"### Paired ({len(rows)})", "", "| Reference | Evidence | City |", "|---|---|---|"]
        out += [f"| {_one(by_id[p['reference']])} | {', '.join(p['evidence'])} | {_one(by_id[p['city']])} |" for p in rows]
        for game, title in (("reference", "Reference only"), ("city", "City only")):
            alone = [f for f in doc["features"] if f["area"] == area and f["game"] == game and f["id"] not in paired]
            out += ["", f"### {title} ({len(alone)})"] + _alone(alone)
    return "\n".join(out) + "\n"


def write(doc, root):
    """Both reports written whole under the repository's dist/inventory; refused, nothing written, past its bound."""
    texts = (json.dumps(doc, ensure_ascii=False, indent=1) + "\n", markdown(doc))
    size = sum(len(t.encode("utf-8")) for t in texts)
    bound = outputs.bound(OUT)["whole_mb"] * outputs.MB
    if size > bound:
        raise TooLarge(f"the reports hold {size} bytes, past {OUT}'s bound of {bound} (tools/tasks/suites.json lifecycle)")
    paths = []
    for name, text in zip(NAMES, texts):
        path = Path(root) / OUT / name
        files.write_atomic(path, text)
        paths.append(path.as_posix())
    return paths


def show_summary(doc, as_json):
    if as_json:
        print(json.dumps({k: doc[k] for k in ("summary", "evidence", "files", "seconds", "languages", "unextracted")},
                         ensure_ascii=False, indent=1))
        return
    print(f"{'area':<18} {'reference':>9} {'paired':>6} {'only':>5}   {'city':>6} {'paired':>6} {'only':>5}")
    for area, row in doc["summary"].items():
        r, c = row["reference"], row["city"]
        print(f"{area:<18} {r['features']:>9} {r['paired']:>6} {r['only']:>5}   {c['features']:>6} {c['paired']:>6} "
              f"{c['only']:>5}")
    print("evidence: " + ", ".join(f"{k} {v}" for k, v in doc["evidence"].items()))
    print("not extracted: " + ("; ".join(f"{g['game']} {g['what']} {g['count']}" for g in doc["unextracted"]) or "nothing"))
    print(f"unclassified places: {len(doc['unclassified'])} (listed in the reports)")


def show_features(features, as_json):
    if as_json:
        print(json.dumps(features, ensure_ascii=False, indent=1))
        return
    for f in features:
        partners = f" paired with {', '.join(f['paired'])}" if f["paired"] else ""
        print(f"{f['id']} {f['game']} [{f['area']}] {_one(f)}{partners}")
    print(f"{len(features)} feature(s)")
