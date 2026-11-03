"""Memory gates: CLAUDE.md must map every source file and must not list files that no longer exist."""
import os
import re


GENERATED = (".uid", ".import") # Godot sidecar files: they belong to the file they annotate


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
    start, end = rules["map_markers"]
    if start not in text or end not in text:
        err("CLAUDE_MD", "CLAUDE.md", f"faltam os marcadores {start} / {end}")
        return
    section = text.split(start, 1)[1].split(end, 1)[0]
    listed = set(re.findall(r"`([^`\s]+\.[A-Za-z0-9]+)`", section))
    listed_dirs = set(re.findall(r"`([^`\s]+)/\*\*`", section))
    for f in files:
        if not mapped(f, rules):
            continue
        if f in listed or any(f.startswith(d + "/") for d in listed_dirs):
            continue
        err("MAPA_DESATUALIZADO", f, "arquivo ausente do mapa no CLAUDE.md (adicione uma linha com a responsabilidade)")
    for p in sorted(listed):
        if not os.path.exists(os.path.join(root, p)):
            err("MAPA_DESATUALIZADO", "CLAUDE.md", f"o mapa cita `{p}`, que não existe mais")
    lines = text.count("\n")
    if lines > 330:
        err("CLAUDE_MD", "CLAUDE.md", f"{lines} linhas: mantenha conciso (até 330); remova o que não é regra nem mapa")
