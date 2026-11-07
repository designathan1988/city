"""The gold of the analysed repository and the settings of the selfcheck, read from data beside the program (only this package reads them), and
the judge: a gold item is found when a finding of its class lies in its file within its line range. Exact, never a match of text."""
import json
import os

EVAL = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))), "arch_eval")


def _load(name):
    with open(os.path.join(EVAL, name), encoding="utf8") as f:
        return json.load(f)


def gold():
    return _load("gold.json")


def settings():
    return _load("checks.json")


def hits(item, findings):
    lo, hi = item["lines"]
    return [f for f in findings if f["class"] == item["class"] and f["file"] == item["file"] and lo <= f["line"] <= hi]


def recall(findings, classes):
    """Per class: gold items found over gold items. A class with no gold item has not been proven."""
    items = gold()["defects"]
    out = {}
    for cid in classes:
        mine = [d for d in items if d["class"] == cid]
        found = [d for d in mine if hits(d, findings)]
        out[cid] = {"gold_found": len(found), "gold_total": len(mine), "missed": [f"{d['file']}:{d['lines'][0]}-{d['lines'][1]}" for d in mine if d not in found]}
    return out


def eval_files():
    return sorted(os.path.join(EVAL, n) for n in os.listdir(EVAL))
