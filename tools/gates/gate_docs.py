"""Memory gates: the map file must list every source file and no file that no longer exists; CLAUDE.md stays short."""
import os
import re


GENERATED = (".uid", ".import", ".translation") # Godot sidecars and import outputs: they belong to their source file


def mapped(f, rules):
    if f.endswith(GENERATED):
        return False
    return any(f == r or f.startswith(r + "/") for r in rules["mapped_roots"])


def check_claude_md(root, rules, files, err):
    path = os.path.join(root, "CLAUDE.md")
    if not os.path.exists(path):
        err("CLAUDE_MD", "CLAUDE.md", "arquivo ausente")
        return
    with open(path, encoding="utf-8") as fh:
        text = fh.read()
    map_file = rules["map_file"]
    map_path = os.path.join(root, map_file)
    if not os.path.exists(map_path):
        err("CLAUDE_MD", map_file, "arquivo ausente")
        return
    with open(map_path, encoding="utf-8") as fh:
        map_text = fh.read()
    start, end = rules["map_markers"]
    if start not in map_text or end not in map_text:
        err("CLAUDE_MD", map_file, f"faltam os marcadores {start} / {end}")
        return
    section = map_text.split(start, 1)[1].split(end, 1)[0]
    listed = set(re.findall(r"`([^`\s]+\.[A-Za-z0-9]+)`", section))
    listed_dirs = set(re.findall(r"`([^`\s]+)/\*\*`", section))
    for f in files:
        if not mapped(f, rules):
            continue
        if f in listed or any(f.startswith(d + "/") for d in listed_dirs):
            continue
        err("MAPA_DESATUALIZADO", f, f"arquivo ausente de {map_file} (adicione uma linha com a responsabilidade)")
    for p in sorted(listed):
        if not os.path.exists(os.path.join(root, p)):
            err("MAPA_DESATUALIZADO", map_file, f"o mapa cita `{p}`, que não existe mais")
    lines = text.count("\n")
    limit = rules["claude_md_max_lines"]
    if lines > limit:
        err("CLAUDE_MD", "CLAUDE.md", f"{lines} linhas: mantenha conciso (até {limit}); o detalhe vai para log/")
