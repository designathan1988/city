"""What the target game still lacks of the source game's behaviours, measured, never translated.

The source is read only for its vocabulary: each top-level declaration (function, class, const, exported name) of each source
file is a behaviour unit named by its words. The target (C++, GDScript, JSON keys, file names) is indexed the same way. A unit is
present when its distinctive words (rare in the source, not plumbing) stand together in one target file; partial when only some do;
missing when none of its distinctive words is anywhere in the target. Nothing of the source's code is written anywhere: the report
names behaviours and where to read what they do.

    python gaps.py --source <source>/src --target <target repo> --out gaps.md [--area people,render]
"""
import argparse
import math
import os
import re
from collections import Counter, defaultdict

PLUMBING = set("""
get set make build update add remove clear reset init create new read write load save from to of in on at by for with the a an and or
is has can do run step tick frame frames pool worker workers job jobs lane lanes cache cached kept keep bake baked baker ledger gpu cull
culling instance instances buffer buffers upload uploads draw drawn mesh meshes geometry material materials shader shaders texture
textures uniform uniforms index indices count size sizes list lists map maps value values data state states key keys id ids type types
default defaults config options option params param result results info debug log test tests spec specs util utils helper helpers wiring
switches switch handle handler handlers event events emit main core base box boxes bounds point points vec vector matrix quat temp tmp
scratch out input output start end begin done pending queue queued apply applied current next prev last first time times dt now
gzip gunzip gunzipped zip inflated decoder encoder desktop ktx2 transcoder transcode f32 f64 snorm snorm16 align8 migrate migrated port
message profiler profiled optimizer hooked hook staging staged residency soup mipmapped tmp1 sub2 fmt dispose backend provider replica
packable watcher reroot dedupe hypot2 hypot3 normalizer batched canvas3d unnumbered structured coalesced periodic manifest
""".split())
SOURCE_SKIP = re.compile(r"(\.worker\.ts$|\.d\.ts$|/lab/|/peopleLab/|/tests?/|Pool\.ts$|Ledger|gpu/|/jobs/)")
DECL = re.compile(r"^(?:export\s+)?(?:default\s+)?(?:async\s+)?(?:function\*?|class|const|let|interface|type|enum)\s+([A-Za-z_$][\w$]*)", re.M)
IDENT = re.compile(r"[A-Za-z][A-Za-z0-9]+")


def words(name):
    parts = re.sub(r"([a-z0-9])([A-Z])", r"\1 \2", re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1 \2", name)).replace("_", " ").split()
    out = []
    for p in parts:
        w = p.lower()
        if len(w) < 3 or w.isdigit():
            continue
        if w.endswith("ies") and len(w) > 4:
            w = w[:-3] + "y"
        elif w.endswith("s") and not w.endswith("ss") and len(w) > 3:
            w = w[:-1]
        out.append(w)
    return out


def source_units(root, areas):
    units = []
    for dirpath, _, files in os.walk(root):
        for f in files:
            if not f.endswith(".ts"):
                continue
            path = os.path.join(dirpath, f).replace("\\", "/")
            rel = os.path.relpath(path, root).replace("\\", "/")
            if SOURCE_SKIP.search("/" + rel):
                continue
            area = rel.split("/")[0] if "/" in rel else "(top)"
            if areas and area not in areas:
                continue
            text = open(path, encoding="utf-8", errors="replace").read()
            stem = words(os.path.splitext(f)[0])
            for m in DECL.finditer(text):
                name = m.group(1)
                ws = [w for w in words(name) if w not in PLUMBING]
                if ws:
                    units.append((area, rel, name, ws, stem, text.count("\n", 0, m.start()) + 1))
    return units


def target_index(repo):
    by_file = {}
    roots = ["native/src", "project/game", "project/ui", "project/render", "project/data", "project/tests"]
    for r in roots:
        for dirpath, _, files in os.walk(os.path.join(repo, r)):
            if "godot-cpp" in dirpath:
                continue
            for f in files:
                if not f.endswith((".cpp", ".h", ".gd", ".json", ".gdshader", ".gdshaderinc")):
                    continue
                path = os.path.join(dirpath, f)
                try:
                    text = open(path, encoding="utf-8", errors="replace").read()
                except OSError:
                    continue
                ws = set(words(os.path.splitext(f)[0]))
                for ident in IDENT.findall(text):
                    ws.update(words(ident))
                by_file[os.path.relpath(path, repo).replace("\\", "/")] = ws
    everywhere = set().union(*by_file.values()) if by_file else set()
    return by_file, everywhere


def judge(units, by_file, everywhere):
    df = Counter(w for u in units for w in set(u[3]))
    n = max(1, len(units))
    rows = []
    for area, rel, name, ws, stem, line in units:
        rare = sorted(set(ws), key=lambda w: df[w])[:3]  # its most distinctive words
        absent = [w for w in rare if w not in everywhere]
        if len(absent) == len(rare):
            verdict = "missing"
        else:
            together = any(all(w in fw for w in rare) for fw in by_file.values())
            verdict = "present" if together else "partial"
        weight = sum(math.log(n / df[w]) for w in rare)
        rows.append((area, rel, name, line, verdict, rare, absent, weight))
    return rows


def report(rows, out):
    by_area = defaultdict(list)
    for r in rows:
        by_area[r[0]].append(r)
    lines = ["# Behaviours of the source not found in the target (measured by gaps.py)", "",
             "missing: none of the unit's distinctive words is anywhere in the target; partial: they never stand together in one target",
             "file. Read the source unit for what it does; write it here in this game's architecture.", ""]
    summary = []
    for area in sorted(by_area):
        rs = by_area[area]
        miss = [r for r in rs if r[4] == "missing"]
        part = [r for r in rs if r[4] == "partial"]
        summary.append((area, len(rs), len(miss), len(part)))
    lines += ["| area | units | missing | partial |", "|---|---|---|---|"]
    lines += [f"| {a} | {t} | {m} | {p} |" for a, t, m, p in summary]
    for area in sorted(by_area):
        rs = sorted((r for r in by_area[area] if r[4] != "present"), key=lambda r: (r[4] != "missing", -r[7]))
        if not rs:
            continue
        lines += ["", f"## {area}", ""]
        files = defaultdict(list)
        for r in rs:
            files[r[1]].append(r)
        for rel in sorted(files, key=lambda k: -sum(1 for r in files[k] if r[4] == "missing")):
            items = files[rel]
            m = sum(1 for r in items if r[4] == "missing")
            lines.append(f"- `{rel}`: {m} missing, {len(items) - m} partial: " + ", ".join(
                f"{r[2]}:{r[3]}{' (' + ' '.join(r[6]) + ')' if r[6] else ''}" for r in items[:12]) + (" ..." if len(items) > 12 else ""))
    open(out, "w", encoding="utf-8", newline="\n").write("\n".join(lines) + "\n")
    return summary


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--source", required=True)
    ap.add_argument("--target", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--area", default="")
    a = ap.parse_args()
    areas = set(x for x in a.area.split(",") if x)
    units = source_units(a.source, areas)
    if not units:
        raise SystemExit("no source units: check --source")
    by_file, everywhere = target_index(a.target)
    if not by_file:
        raise SystemExit("no target files: check --target")
    summary = report(judge(units, by_file, everywhere), a.out)
    for area, t, m, p in summary:
        print(f"{area:12s} units {t:5d} missing {m:5d} partial {p:5d}")


if __name__ == "__main__":
    main()
