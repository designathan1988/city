# rename_english.py <repo> [--apply]: every Portuguese file and folder name under tools/process and tools/ports put in
# English (one map, below), with `git mv` from the leaves up, and every reference to an old name rewritten where it is a path
# (beside a slash or backslash, in quotes, or a file name with its extension), never in prose. Without --apply: the plan only.
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(sys.argv[1])
APPLY = "--apply" in sys.argv
WORDS = {
    "process": "process", "ports": "ports", "tools": "tools", "records": "records", "reading": "reading",
    "map40": "map40", "reviewed": "reviewed", "verified": "verified", "cycles": "cycles", "converter": "converter",
    "extractor-package": "extractor-package", "vegetation-terrain": "vegetation-terrain", "features-4.0": "features-4.0",
    "whole-files": "whole-files", "traced": "traced", "untraced": "untraced", "analysis": "analysis",
    "people-analysis": "people-analysis", "converter-old": "converter-old", "gate": "gate", "gate-check": "gate-check",
    "integration": "integration", "elevation": "elevation", "specifications": "specifications", "sources": "sources",
    "photos-game": "photos-game", "photos-occlusion": "photos-occlusion", "landmarks": "landmarks", "parity": "parity",
    "roadcraft-reading": "roadcraft-reading", "roadcraft-port": "roadcraft-port",
}
FILES = {
    "01-tree-of-the-4.0.txt": "01-tree-of-the-4.0.txt", "02-real-dynamic-channels.txt": "02-real-dynamic-channels.txt",
    "03-real-examples-of-the-4.0.txt": "03-real-examples-of-the-4.0.txt", "04-current-extractor.txt": "04-current-extractor.txt",
    "05-features-and-graph.txt": "05-features-and-graph.txt", "GODOT-PORT-GUIDE.md": "GODOT-PORT-GUIDE.md",
    "PHOTOS-INDEX.md": "PHOTOS-INDEX.md", "READ-ME.txt": "READ-ME.txt", "addendum-final.md": "addendum-final.md",
    "addendum-wave2.md": "addendum-wave2.md", "addendum-wave3.md": "addendum-wave3.md", "analysis.md": "analysis.md",
    "c2-real-session.log": "c2-real-session.log", "c2-stop-proof.log": "c2-stop-proof.log", "c2-test.log": "c2-test.log",
    "sidewalk-analysis.md": "sidewalk-analysis.md", "cycle2-convert-road-elevation.log": "cycle2-convert-road-elevation.log",
    "cycle2-tests.log": "cycle2-tests.log", "cycle2-typecheck.log": "cycle2-typecheck.log", "coverage.md": "coverage.md",
    "coverage.py": "coverage.py", "coverage_rules.json": "coverage_rules.json", "commands.py": "commands.py",
    "check.json": "check.json", "constants.json": "constants.json", "contract-port6.md": "contract-port6.md",
    "conversations.py": "conversations.py", "coordination.md": "coordination.md", "gates_dispatch.py": "gates_dispatch.py",
    "claude-code-diagnosis.md": "claude-code-diagnosis.md", "elements.md": "elements.md", "state.json": "state.json",
    "state.py": "state.py", "queue.md": "queue.md", "source-settings.proposed.json": "source-settings.proposed.json",
    "photos-4.0-list-round-4.md": "photos-4.0-list-round-4.md", "photos-4.0-list-round-5.md": "photos-4.0-list-round-5.md",
    "photos-4.0.md": "photos-4.0.md", "photos-list-pair.md": "photos-list-pair.md", "photos-list.md": "photos-list.md",
    "photos.md": "photos.md", "photos_analysis.md": "photos_analysis.md", "features.json": "features.json",
    "features.cjs": "features.cjs", "extract_feature.cjs": "extract_feature.cjs",
    "extract_feature_cpp.cjs": "extract_feature_cpp.cjs", "graph.json": "graph.json",
    "hooks-and-locks.md": "hooks-and-locks.md", "index.md": "index.md", "infrastructure.md": "infrastructure.md",
    "inventory.json": "inventory.json", "inventory.py": "inventory.py", "reading.md": "reading.md",
    "reading_lib.py": "reading_lib.py", "survey.md": "survey.md", "list.md": "list.md", "list2.md": "list2.md",
    "look-night.json": "look-night.json", "look-people.json": "look-people.json", "look-people2.json": "look-people2.json",
    "look-people3.json": "look-people3.json", "look-all.json": "look-all.json", "look-roads.json": "look-roads.json",
    "look-roads2.json": "look-roads2.json", "look-roads3.json": "look-roads3.json", "look-roads4.json": "look-roads4.json",
    "measure_view.py": "measure_view.py", "required.json": "required.json", "palette-sources.md": "palette-sources.md",
    "pairs-analysis.md": "pairs-analysis.md", "pairs-list.md": "pairs-list.md", "parity-defects.md": "parity-defects.md",
    "steps.py": "steps.py", "coordinator-pending.md": "coordinator-pending.md", "research.md": "research.md",
    "people.md": "people.md", "plan-furniture.md": "plan-furniture.md", "plan.md": "plan.md", "poses-names.json": "poses-names.json",
    "process.py": "process.py", "coordinator-prompt.md": "coordinator-prompt.md",
    "feature-extractor-prompt.md": "feature-extractor-prompt.md", "session-rules.md": "session-rules.md",
    "report.md": "report.md", "pair-answer.md": "pair-answer.md", "roads-light.md": "roads-light.md",
    "final-round-list.md": "final-round-list.md", "converter_sessions.txt": "converter_sessions.txt",
    "coordinator_sessions.txt": "coordinator_sessions.txt", "symbols.py": "symbols.py", "test.py": "test.py",
    "tests.log": "tests.log", "vegetation-terrain.md": "vegetation-terrain.md", "message_watch.py": "message_watch.py",
    "session_watch.py": "session_watch.py", "graph.cjs": "graph.cjs",
    "01_field_half.jpg": "01_field_half.jpg", "02_scrub_edge.jpg": "02_scrub_edge.jpg", "03_scrub_near.jpg": "03_scrub_near.jpg",
    "04_scrub_near_zoom.png": "04_scrub_near_zoom.png", "05_gardens.jpg": "05_gardens.jpg", "06_gardens_zoom.png": "06_gardens_zoom.png",
    "07_downtown_street_trees.jpg": "07_downtown_street_trees.jpg", "08_park_day.jpg": "08_park_day.jpg",
    "09_park_night.jpg": "09_park_night.jpg", "10_park_rain.jpg": "10_park_rain.jpg", "11_city_far.jpg": "11_city_far.jpg",
    "12_horizon_20degrees.jpg": "12_horizon_20degrees.jpg", "13_hill_lake.jpg": "13_hill_lake.jpg",
    "14_lake_near.jpg": "14_lake_near.jpg", "15_cliff.jpg": "15_cliff.jpg", "16_cliff_night.jpg": "16_cliff_night.jpg",
    "n_forward_plus_near.png": "n_forward_plus_near.png", "n_gl_compatibility_near.png": "n_gl_compatibility_near.png",
    "w2_forward_plus_near.png": "w2_forward_plus_near.png", "w2_gl_compatibility_near.png": "w2_gl_compatibility_near.png",
}
PLAN = {"arvores": "trees", "ceu": "sky", "cubo": "cube", "dinheiro": "money", "luz": "light", "luz2": "light2", "luz3": "light3",
        "mar": "sea", "noite": "night", "ordem": "order", "poses": "poses", "rampa": "ramp", "referencia": "reference",
        "referencia2": "reference2", "rio": "river", "soltas": "loose", "sombra": "shadow"}


def english(name: str) -> str:
    if name in WORDS:
        return WORDS[name]
    if name in FILES:
        return FILES[name]
    m = re.fullmatch(r"plano-([a-z0-9]+)\.json", name)
    if m and m.group(1) in PLAN:
        return f"plan-{PLAN[m.group(1)]}.json"
    m = re.fullmatch(r"ciclo2-passo(\d)\.json", name)
    if m:
        return f"cycle2-step{m.group(1)}.json"
    m = re.fullmatch(r"passo(\d)\.json", name)
    if m:
        return f"step{m.group(1)}.json"
    m = re.fullmatch(r"lista-rodada-(\d)\.md", name)
    if m:
        return f"list-round-{m.group(1)}.md"
    m = re.fullmatch(r"fotos-(par|luz|t|v)(\d+)(-.*)?", name)
    if m:
        kind = {"par": "pair", "luz": "light", "t": "t", "v": "v"}[m.group(1)]
        tail = {"-dll-antiga": "-old-dll", "-sem-efeito": "-no-effect", "-luz": "-light", None: ""}[m.group(3)]
        return f"photos-{kind}{m.group(2)}{tail}"
    m = re.fullmatch(r"look-par(\d+)\.json", name)
    if m:
        return f"look-pair{m.group(1)}.json"
    m = re.fullmatch(r"par(\d+)", name)
    if m:
        return f"pair{m.group(1)}"
    return name


git = lambda *a: subprocess.run(["git", "-C", str(ROOT), *a], capture_output=True, text=True, check=True).stdout
files = [f for f in git("ls-files", "tools/process", "tools/ports").splitlines()]
# Every path (files and their folders) whose own name changes, deepest first.
paths = set()
for f in files:
    parts = f.split("/")
    for i in range(1, len(parts) + 1):
        paths.add("/".join(parts[:i]))
moves = sorted((p for p in paths if english(p.split("/")[-1]) != p.split("/")[-1]), key=lambda p: -p.count("/"))
# The references are rewritten for every old name the last commit holds, not only for what is still to move: a run that
# moved part of the tree before stopping leaves the rest of its names to rewrite.
committed = git("ls-tree", "-r", "--name-only", "HEAD", "tools/process", "tools/portes").splitlines()
renamed = {c: english(c) for f in committed for c in f.split("/") if english(c) != c}
print(f"{len(moves)} paths renamed, {len(renamed)} distinct names")
for old, new in sorted(renamed.items()):
    print(f"  {old} -> {new}")
if not APPLY:
    sys.exit(0)
for p in moves:
    parent, name = p.rsplit("/", 1) if "/" in p else ("", p)
    target = f"{parent}/{english(name)}" if parent else english(name)
    git("mv", p, target)
# References: a name where it is a path, never in prose.
TEXT = {".md", ".py", ".mjs", ".cjs", ".js", ".ts", ".json", ".jsonl", ".sh", ".ps1", ".txt", ".gd", ".cfg", ".toml", ".yml", ".yaml", ""}
# One pass a file: every name in one alternation (longest first), the replacement looked up. A folder name counts beside a
# slash or backslash or alone in quotes; a file name (it has its extension) wherever it stands as a whole name.
by_length = lambda names: "|".join(re.escape(n) for n in sorted(names, key=len, reverse=True))
folders = by_length(n for n in renamed if "." not in n)
files_ = by_length(n for n in renamed if "." in n)
rx = re.compile(rf"(?<=[/\\])(?P<a>{folders})(?![\w.-])|(?<![\w.-])(?P<b>{folders})(?=[/\\])|(?<=['\"`])(?P<c>{folders})(?=['\"`])"
                rf"|(?<![\w.-])(?P<d>{files_})(?![\w-])")


def swap(m: re.Match) -> str:
    name = m.group(m.lastgroup)
    # tools/analise is another session's folder, not moved here: its references are left alone.
    if name == "analysis" and m.string[max(0, m.start() - 6):m.start()] in ("tools/", "tools\\"):
        return name
    return renamed[name]


# Other sessions' folders are not read: they are theirs to rename.
OTHERS = ("tools/converter/", "tools/oracle-4.0/", "tools/analise/", "tools/analysis/")
changed = 0
for f in git("ls-files").splitlines():
    path = ROOT / f
    if f.startswith(OTHERS) or path.suffix not in TEXT or not path.is_file() or path.stat().st_size > 20_000_000:
        continue
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        continue
    new_text = rx.sub(swap, text)
    if new_text != text:
        path.write_text(new_text, encoding="utf-8", newline="")
        changed += 1
print(f"{changed} files with references rewritten")
