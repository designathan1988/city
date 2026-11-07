"""A recorded execution, never run here: what an instrumented build counted. Found by the format of the file, read by the toolchain's tool."""
import math
import os
import shutil
import subprocess
import tempfile

from .adapters.cpp_build import _entries as compile_entries
from .adapters.cpp_build import _split_words as command_words
from .core import natural_break
from .model import RECORDED

MAGIC = b"\xfflprofi\x81"


def _tool(repo):
    found = shutil.which("llvm-profdata")
    if found:
        return found
    for entries in compile_entries(repo).values():
        for entry in entries:
            words = command_words(entry)
            base = os.path.dirname(words[0]) if words else ""
            for name in ("llvm-profdata.exe", "llvm-profdata"):
                path = os.path.join(base, name)
                if os.path.exists(path):
                    return path
    return None


def find(repo, tracked):
    """Tracked files whose first bytes say they are an indexed profile."""
    out = []
    for path in sorted(tracked):
        if path.endswith((".profdata", ".prof", ".profraw")) or "prof" in path.rsplit("/", 1)[-1].lower():
            data = repo.blob(tracked[path])
            if data[:8] == MAGIC:
                out.append(path)
    return out


def _text(repo, tracked, path, tool):
    with tempfile.TemporaryDirectory() as tmp:
        raw = os.path.join(tmp, "profile")
        out = os.path.join(tmp, "profile.txt")
        with open(raw, "wb") as f:
            f.write(repo.blob(tracked[path]))
        subprocess.run((tool, "merge", "--text", "-o", out, raw), capture_output=True, check=False)
        if not os.path.exists(out):
            return ""
        with open(out, encoding="utf8", errors="replace") as f:
            return f.read()


def parse(text):
    """name -> (largest counter, [targets per indirect site]) from the tool's text form."""
    out = {}
    blocks = text.split("\n\n")
    for block in blocks:
        lines = [x for x in block.split("\n") if x]
        if len(lines) < 4 or lines[0].startswith(":"):
            continue
        name = lines[0]
        i = 1
        counts = []
        sites = []
        while i < len(lines):
            line = lines[i]
            if line.startswith("# Counter Values"):
                i += 1
                while i < len(lines) and not lines[i].startswith("#"):
                    counts.append(int(lines[i]))
                    i += 1
                continue
            if "IPVK_IndirectCallTarget" in line:
                i += 1
                while i < len(lines) and not lines[i].startswith("# NumValueSites"):
                    i += 1
                i += 2
                while i < len(lines) and not lines[i].startswith("#"):
                    n = int(lines[i])
                    targets = [t.rsplit(":", 1) for t in lines[i + 1:i + 1 + n]]
                    sites.append([(a, int(b)) for a, b in targets if b.isdigit()])
                    i += 1 + n
                continue
            i += 1
        out[name] = (max(counts) if counts else 0, sites)
    return out


def _unit_by_name(graph):
    table = {}
    for uid, u in graph.units.items():
        mangled = u["attrs"].get("mangled")
        if mangled:
            table[mangled.lstrip("_") if False else mangled] = uid
    return table


def _key(name):
    """A profile name may carry the path of the folder it was built in: `path;symbol`. Only the symbol is the program's."""
    return name.rsplit(";", 1)[-1]


def read(run):
    """Adds to the graph what was recorded: which units ran how often, and who an indirect call really reached."""
    repo, graph = run.repo, run.graph
    result = {"profiles": [], "units": 0, "hot": 0, "indirect_resolved": 0, "lost": []}
    paths = find(repo, run.tracked)
    tool = _tool(repo) if paths else None
    if not tool:
        return result
    table = _unit_by_name(graph)
    counts = {}
    for path in paths:
        result["profiles"].append(path)
        records = parse(repo.profile(_text(repo, run.tracked, path, tool)))
        for name, (count, sites) in sorted(records.items()):
            uid = table.get(_key(name))
            if uid is None:
                continue
            counts[uid] = max(counts.get(uid, 0), count)
            for site in sites:
                for target, n in site:
                    other = table.get(_key(target))
                    if other is None or n == 0:
                        continue
                    known = (uid, other, "call") in graph.edges or (uid, other, "virtual") in graph.edges or (uid, other, "override") in graph.edges
                    graph.add_edge(uid, other, "recorded_call", RECORDED, 1.0, graph.units[uid]["file"], graph.units[uid]["line"], {"count": n})
                    result["indirect_resolved"] += 1
                    if not known:
                        result["lost"].append((uid, other, n))
    for uid, c in counts.items():
        graph.units[uid]["attrs"]["ran"] = c
    result["units"] = len(counts)
    return result


def _last_recorded(run, result):
    """The last commit that changed a recording: what changed after it, it never saw."""
    last = -1
    for i, (_, changes) in enumerate(run.history):
        if any(c[1] in result["profiles"] for c in changes):
            last = i
    return last


def rate(run, result, when, lineage):
    """What the recording means once the program is known: code only tests reach was hot for the training, not for a player; code changed
    after the recording is marked stale; what ran hot and no entry reaches is what static analysis lost. Not having run proves nothing."""
    graph = run.graph
    counts = {uid: u["attrs"]["ran"] for uid, u in graph.units.items() if "ran" in u["attrs"]}
    played = {uid: c for uid, c in counts.items() if uid not in when.tests}
    moving = [math.log(c) for c in played.values() if c > 0]
    result["only_in_tests"] = len(counts) - len(played)
    if moving:
        cut = math.exp(natural_break(moving)) if len(set(moving)) > 1 else 0
        for uid, c in played.items():
            if c >= cut and c > 0:
                graph.units[uid]["attrs"]["hot"] = True
                result["hot"] += 1
    last = _last_recorded(run, result)
    stale = [uid for uid in counts if uid in lineage and any(i > last for i in lineage[uid].touched)]
    for uid in stale:
        graph.units[uid]["attrs"]["stale"] = True
    result["stale"] = len(stale)
    result["hot_static_missed"] = sorted(uid for uid in played if graph.units[uid]["attrs"].get("hot") and when.label(uid) == "cold")
