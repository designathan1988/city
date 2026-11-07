"""Memory gates: the map file lists every source file and no file that no longer exists; CLAUDE.md stays short."""
import re

GENERATED = (".uid", ".import", ".translation")  # Godot sidecars and import outputs: they belong to their source file


def mapped(f, rules):
    if f.endswith(GENERATED):
        return False
    return any(f == r or f.startswith(r + "/") for r in rules["mapped_roots"])


def inputs(ctx, path):
    return path in ("CLAUDE.md", ctx.rules["map_file"]) or (path in ctx.file_set and mapped(path, ctx.rules))


def check_claude_md(ctx, err):
    rules, listed_files = ctx.rules, ctx.listed_set
    if "CLAUDE.md" not in listed_files:
        err("CLAUDE_MD", "CLAUDE.md", "file missing")
        return
    map_file = rules["map_file"]
    if map_file not in listed_files:
        err("CLAUDE_MD", map_file, "file missing")
        return
    map_text = ctx.source(map_file).text
    start, end = rules["map_markers"]
    if start not in map_text or end not in map_text:
        err("CLAUDE_MD", map_file, f"markers {start} / {end} missing")
        return
    section = map_text.split(start, 1)[1].split(end, 1)[0]
    listed = set(re.findall(r"`([^`\s]+\.[A-Za-z0-9]+)`", section))
    listed_dirs = set(re.findall(r"`([^`\s]+)/\*\*`", section))
    for f in ctx.files:
        if mapped(f, rules) and f not in listed and not any(f.startswith(d + "/") for d in listed_dirs):
            err("MAPA_DESATUALIZADO", f, f"file missing from {map_file} (add a line with its responsibility)")
    for p in sorted(listed):
        if p not in listed_files:
            err("MAPA_DESATUALIZADO", map_file, f"the map names `{p}`, which no longer exists")
    lines = ctx.source("CLAUDE.md").text.count("\n")
    limit = rules["claude_md_max_lines"]
    if lines > limit:
        err("CLAUDE_MD", "CLAUDE.md", f"{lines} lines: keep it concise (up to {limit}); the detail goes to log/")
