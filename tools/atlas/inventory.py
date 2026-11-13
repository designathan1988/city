"""The inventory of the two games (`python tools/atlas/cli.py inventory`): what features the reference game has, what this
game has, how each stands, and the pairing between them, read anew from both trees each run by rules (the reference's:
inventory_reference.py; this game's: inventory_game.py), never from a list written by hand. A feature is {game, area, kind,
name, label, description, source}; its label is its text, or the text of the key it is labelled by. Two features of the same
kind pair by evidence only: the same player's text in a language both games hold (inventory_rules.norm_text), the same
text key, the same id in the same area (an action's or a data entry's, inventory_rules.norm_id); a pair made by one
language's text says `differs:<lang>` when the other language's texts differ (a doubt shown, not a pair taken back); what no
evidence pairs is listed by area as reference only or city only. An id pair holds only where the two tables share a word (the same
entry name in two unrelated tables is no pair). Systems pair through the features they hold or use (inventory_systems.py).
The reports are written whole under dist/inventory (inventory_report.py)."""
import os
import sys
import time

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import inventory_game  # noqa: E402
from atlas import inventory_reference  # noqa: E402
from atlas import inventory_report as report  # noqa: E402
from atlas import inventory_rules as rules  # noqa: E402
from atlas import inventory_systems as systems  # noqa: E402
from tasks import locations  # noqa: E402

GAMES = ("reference", "city")
LABELLED = ("text", "action")  # the kinds whose texts pair them
IDENTIFIED = ("action", "data")  # the kinds whose ids pair them, within one area


def add_parser(commands, common):
    p = commands.add_parser("inventory", parents=[common], help="the features of the reference and of this game, paired by "
                            "evidence; reports in dist/inventory")
    p.add_argument("--area", help="only this area's features (an area name, or unclassified)")
    p.add_argument("--game", choices=GAMES, help="only this game's features")
    p.add_argument("--unpaired", action="store_true", help="only the features no evidence pairs")
    p.add_argument("--reference", help="the reference's code folder (default: tools/gates/rules.json lab_reference, found "
                   "by tools/tasks/locations.py)")
    return p


def resolve_labels(features):
    """An action's label: the text of the first of its keys its own game holds (`key` given, else `keys` in order)."""
    by_game = {g: {f["name"]: f for f in features if f["game"] == g and f["kind"] == "text"} for g in GAMES}
    for f in features:
        if f["kind"] == "text":
            continue
        texts = by_game[f["game"]]
        key = f.get("key") or next((k for k in f.pop("keys", []) if k in texts), "")
        f.pop("keys", None)
        if key in texts:
            f.update(label=texts[key]["label"], key=key, texts=texts[key].get("texts", {}))


def _evidence(f, shared):
    """The evidence a feature can be paired by: (what, the value both sides must share)."""
    out = []
    if f["kind"] in LABELLED:
        for lang in shared:
            text = rules.norm_text(f.get("texts", {}).get(lang, ""))
            if text:
                out.append((f"text:{lang}", (f["kind"], lang, text)))
    if f["kind"] == "text":
        out.append(("key", ("text", f["name"])))
    elif f["kind"] in IDENTIFIED and f.get("match"):
        out.append(("id", (f["kind"], f["area"], f["match"])))
    return out


def _differs(ref, city, shared):
    """The shared languages in which both features have a text and the texts differ: said beside a pair made by another
    language's text (the same English word under two Portuguese ones may be two features)."""
    out = []
    for lang in shared:
        a = rules.norm_text(ref.get("texts", {}).get(lang, ""))
        b = rules.norm_text(city.get("texts", {}).get(lang, ""))
        if a and b and a != b:
            out.append(f"differs:{lang}")
    return out


def pair(features, shared):
    """Every pair a piece of evidence makes: [{"reference": id, "city": id, "evidence": [...]}] in the reference's order."""
    index, by_id = {}, {f["id"]: f for f in features}
    for f in features:
        if f["game"] == "city":
            for what, value in _evidence(f, shared):
                index.setdefault((what, value), []).append(f["id"])
    pairs = {}
    for f in features:
        if f["game"] != "reference":
            continue
        for what, value in _evidence(f, shared):
            for other in index.get((what, value), ()):
                pairs.setdefault((f["id"], other), []).append(what)
    for (r, c), evidence in list(pairs.items()):
        if evidence == ["id"] and not same_table(by_id[r], by_id[c]):
            del pairs[(r, c)]
        elif any(what.startswith("text:") for what in evidence):
            evidence += _differs(by_id[r], by_id[c], shared)
    return [{"reference": r, "city": c, "evidence": ev} for (r, c), ev in pairs.items()]


def same_table(ref, city):
    """Whether two entries' tables may hold the same thing: a word of their names in common (each in the singular), or a
    table unnamed."""
    a, b = (set(map(rules.singular, rules.words(f.get("table", "")))) for f in (ref, city))
    return not a or not b or bool(a & b)


def systems_summary(features, pairs):
    """Per area and game: its systems and how many of them are paired (the measure of the port by what runs)."""
    paired = {p["reference"] for p in pairs} | {p["city"] for p in pairs}
    out = {}
    for f in features:
        if f["kind"] == "system":
            row = out.setdefault(f["area"], {g: {"systems": 0, "paired": 0} for g in GAMES})[f["game"]]
            row["systems"] += 1
            row["paired"] += f["id"] in paired
    return dict(sorted(out.items()))


def summarize(features, pairs):
    """Per area: each game's features, how many of them are paired and how many stand alone; the evidence's counts."""
    paired = {p["reference"] for p in pairs} | {p["city"] for p in pairs}
    areas = sorted({f["area"] for f in features}, key=lambda a: (a == rules.UNCLASSIFIED, a))
    summary = {}
    for area in areas:
        row = {}
        for game in GAMES:
            mine = [f for f in features if f["area"] == area and f["game"] == game]
            both = sum(1 for f in mine if f["id"] in paired)
            row[game] = {"features": len(mine), "paired": both, "only": len(mine) - both}
        summary[area] = row
    evidence = {"pairs": len(pairs)}
    for p in pairs:
        for what in p["evidence"]:
            evidence[what] = evidence.get(what, 0) + 1
        only = "only " + "+".join(sorted({e for e in p["evidence"] if not e.startswith("differs:")}))
        evidence[only] = evidence.get(only, 0) + 1
    return summary, evidence


def unclassified_places(features):
    """Where the unclassified features come from: each game's folders (a text: its key's first word), with how many."""
    out = {}
    for f in features:
        if f["area"] != rules.UNCLASSIFIED:
            continue
        place = f["name"].split(".")[0] if f["kind"] == "text" else os.path.dirname(f["source"].split(":")[0])
        out[(f["game"], f["kind"], place)] = out.get((f["game"], f["kind"], place), 0) + 1
    return [{"game": g, "kind": k, "place": p, "count": n} for (g, k, p), n in sorted(out.items())]


def build(repo, reference=None, progress=print):
    """The whole inventory: both games read, labelled, paired and summed up, with the seconds each step took."""
    start = time.perf_counter()
    root, dirs, skip = (reference, ["."], set()) if reference else inventory_reference.locate(repo)
    progress(f"reference: reading {root} ({', '.join(dirs)})")
    ref = inventory_reference.read(root, dirs, skip)
    t_ref = time.perf_counter()
    progress(f"reference: {ref['files']} files read in {t_ref - start:.2f} s, {report.kinds(ref['features'])}")
    progress(f"city: reading {repo}")
    city = inventory_game.read(repo)
    t_city = time.perf_counter()
    progress(f"city: {city['files']} files read in {t_city - t_ref:.2f} s, {report.kinds(city['features'])}")
    features = ref["features"] + city["features"]
    for game, prefix in zip(GAMES, "rc"):  # the extractors' `id` is the normal id the pairing compares: kept as `match`
        for n, f in enumerate((f for f in features if f["game"] == game), 1):
            f["match"] = f.pop("id", None)
            f["id"] = f"{prefix}{n}"
    resolve_labels(features)
    shared = sorted(set(ref["languages"]) & set(city["languages"]))
    pairs = pair(features, shared)
    pairs += systems.pair_systems(features, pairs)
    summary, evidence = summarize(features, pairs)
    t_pair = time.perf_counter()
    progress(f"paired: {len(pairs)} pairs in {t_pair - t_city:.2f} s (languages both games hold: {', '.join(shared)})")
    return {"generated": time.strftime("%Y-%m-%dT%H:%M:%S"), "roots": {"reference": root, "city": repo},
            "languages": {"reference": ref["languages"], "city": city["languages"], "shared": shared},
            "files": {"reference": ref["files"], "city": city["files"]}, "summary": summary, "evidence": evidence,
            "systems": systems_summary(features, pairs),
            "unclassified": unclassified_places(features), "unextracted": ref["gaps"] + city["gaps"],
            "seconds": {"reference": round(t_ref - start, 3), "city": round(t_city - t_ref, 3),
                        "pairing": round(t_pair - t_city, 3)},
            "features": features, "pairs": pairs}


def select(doc, area=None, game=None, unpaired=False):
    """The features the filters keep, each with the ids of its pairs."""
    partners = {}
    for p in doc["pairs"]:
        partners.setdefault(p["reference"], []).append(p["city"])
        partners.setdefault(p["city"], []).append(p["reference"])
    return [dict(f, paired=partners.get(f["id"], [])) for f in doc["features"]
            if (not area or f["area"] == area) and (not game or f["game"] == game)
            and (not unpaired or f["id"] not in partners)]


def run(args, root):
    """Exit 0 with the inventory written and shown; 1 when the reference cannot be found or the reports pass their bound;
    2 for an area no feature has."""
    start = time.perf_counter()
    say = (lambda line: print(line, file=sys.stderr, flush=True)) if args.json else (lambda line: print(line, flush=True))
    if args.reference and not os.path.isdir(args.reference):
        print(f"inventory: the reference folder {args.reference} does not exist: nothing read")
        return 1
    try:
        doc = build(root, args.reference, progress=say)
    except locations.Missing as e:
        print(f"inventory: {e}")
        return 1
    if args.area and args.area not in doc["summary"]:
        print(f"inventory: no feature of the area {args.area!r}; the areas: {', '.join(doc['summary'])}")
        return 2
    try:
        paths = report.write(doc, root)
    except report.TooLarge as e:
        print(f"inventory: {e}")
        return 1
    if args.area or args.game or args.unpaired:
        report.show_features(select(doc, args.area, args.game, args.unpaired), args.json)
    else:
        report.show_summary(doc, args.json)
    say(f"reports: {', '.join(paths)}; {time.perf_counter() - start:.2f} s in all")
    return 0
