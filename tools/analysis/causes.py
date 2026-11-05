# Generic root-cause analyser: scans any copy of a project (any version, the whole tree but builds and dependencies) for the
# code shapes that cause performance and resource effects (detectors.py and the ones here), and with --build turns every
# compiler warning into a finding. It knows no class, file or fix of the project: it reports cause, effect, where and proof.
# What the code costs while it runs is sampler.py's: run both, the shapes here explain the hot frames there.
# Use: python tools/analysis/causes.py [--root DIR] [--json OUT] [--build CMAKE_BUILD_DIR]
import argparse
import json
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import detectors as d  # noqa: E402

FRAME_CALLBACKS = r"_process|_draw|_physics_process"
MUTATORS = r"(?:\.|->)(sync|submit|step\w*|mark_\w*|touch|publish\w*)\s*\("  # any receiver: classes change, the verbs stay


def query_names(root):
    """The names a script table calls 'queries' (a QUERIES dictionary in any script): read-only by contract."""
    names = set()
    for p in d.files(root, d.GD_EXT):
        block = re.search(r"QUERIES\s*:?=\s*\{(.*?)\n\}", d.read(p), re.S)
        if block:
            names |= set(re.findall(r'&?"\w+"\s*:\s*"(?:\w+/)?(\w+)"', block.group(1)))  # "method" or "area/method"
    return names


def mutating_queries(root, sources):
    """A 'query' whose native method changes state: the per-frame work is driven by whoever reads, a second owner of it."""
    out, names = [], query_names(root)
    for p, t in sources.items():
        for name, line, body, const in d.functions(t):
            short = name.split("::")[-1]
            if short in names and not const and re.search(MUTATORS, body):
                out.append(d.finding("MUTATING_QUERY", f"the query '{short}' changes state (not const, calls sync/submit/publish)",
                                     "per-frame work driven by a reader; it stops when the screen that reads is gone",
                                     f"{d.rel(root, p)}:{line}", "QUERIES method, not const, with a mutating call"))
    return out


def costly_natives(sources):
    """The native methods that lock or walk something: what a frame must not ask for nothing."""
    return {name.split("::")[-1] for t in sources.values() for name, _, body, _ in d.functions(t)
            if re.search(r"lock|mutex|\bfor\s*\(|\bwhile\s*\(|Array\s+\w+;", body)}


def polling_per_frame(root, sources):
    """A frame callback that asks the native side every frame, with no revision nor timer, for something that costs."""
    out, costly = [], costly_natives(sources)
    for p in d.files(root, d.GD_EXT):
        if re.search(r"[\\/]tests?[\\/]", p):
            continue
        t = d.read(p)
        for m in re.finditer(rf"^func ({FRAME_CALLBACKS})\(.*?(?=^func |\Z)", t, re.S | re.M):
            body = m.group(0)
            for q in re.finditer(r"\.query\(\s*&?\"(\w+)\"", body):
                gated = re.search(r"\breturn\b", body[:q.start()])  # something decides first whether to ask
                if q.group(1) in costly and not gated and not re.search(r"revision|_seen|cached\(", body):
                    line = t[:m.start()].count("\n") + body[:q.start()].count("\n") + 1
                    out.append(d.finding("POLLING_PER_FRAME", f"{m.group(1)} asks '{q.group(1)}' every frame",
                                         "the native side's cost and locks paid every frame even when nothing changed",
                                         f"{d.rel(root, p)}:{line}", "query inside a frame callback, no revision"))
    return out


def incomplete_cache_keys(root, sources):
    """A cache kept by a revision that does not move on every path that replaces the state (new world, load, reset)."""
    out, bumps, setters = [], {}, {}
    for t in sources.values():  # one-line setters: whoever calls them moves the revision
        for f, r in re.findall(r"\b(\w+)\s*\(\)\s*\{\s*(\w*revision)\s*(?:\+\+|\+=)", t):
            setters[f] = r
    for p, t in sources.items():
        for name, line, body, _ in d.functions(t):
            for r in re.findall(r"\b(\w*revision)\s*(?:\+\+|\+=)", body):
                bumps.setdefault(r, set()).add(name.split("::")[-1])
            for f, r in setters.items():
                if re.search(rf"[.>]{f}\s*\(", body):
                    bumps.setdefault(r, set()).add(name.split("::")[-1])
    for p, t in sources.items():
        for m in re.finditer(r"if\s*\(\s*[\w.>-]*?(\w*revision)\s*!=\s*([\w.>-]+)\s*\)", t):
            who = bumps.get(m.group(1), set()) | bumps.get(m.group(2).split(".")[-1].split(">")[-1], set())  # the kept copy and the source
            if not any(re.search(r"found|load|reset|new|generate|clear|init", f, re.I) for f in who):
                out.append(d.finding("INCOMPLETE_CACHE_KEY", f"a cache kept by '{m.group(1)}', which only moves in: {', '.join(sorted(who)) or 'nowhere'}",
                                     "stale values after a new world, a load or a change outside the commands",
                                     f"{d.rel(root, p)}:{t[:m.start()].count(chr(10)) + 1}", "no function that replaces the state moves the revision"))
    return out


def build_warnings(root, build):
    """Compiler warnings are dead code or hidden intent: each one is a finding."""
    r = subprocess.run(["cmake", "--build", build], capture_output=True, text=True, cwd=root)
    return [d.finding("COMPILER_WARNING", m.group(3), "dead code or an ignored result with no stated intent",
                      f"{os.path.relpath(m.group(1), root)}:{m.group(2)}", "build warning")
            for m in re.finditer(r"^(.*?):(\d+):\d+: warning: (.*)$", r.stdout + r.stderr, re.M)]


def analyse(root):
    sources = {p: d.read(p) for p in d.files(root, d.CPP_EXT)}
    found = []
    for detector in (d.worker_locks_in_readers, d.realloc_per_frame, d.unpruned_maps, d.idle_wakeups, d.duplicated_constants,
                     d.engine_call_in_loop, mutating_queries, incomplete_cache_keys):
        found += detector(root, sources)
    return found + polling_per_frame(root, sources)


def report(found):
    by = {}
    for a in found:
        by.setdefault(a["rule"], []).append(a)
    print(f"Causes found: {len(found)} in {len(by)} rules")
    for rule, items in sorted(by.items()):
        print(f"\n[{rule}] {len(items)} - effect: {items[0]['effect']}")
        for a in items:
            print(f"  {a['where']}: {a['cause']} ({a['proof']})")


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", default=os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..")))
    ap.add_argument("--json")
    ap.add_argument("--build", help="a CMake build folder: its warnings become findings")
    a = ap.parse_args()
    found = analyse(a.root) + (build_warnings(a.root, a.build) if a.build else [])
    report(found)
    if a.json:
        with open(a.json, "w", encoding="utf-8") as f:
            json.dump(found, f, ensure_ascii=False, indent=1)


if __name__ == "__main__":
    main()
