"""File-level gates: repo layout, junk, single document, size limits, header comments."""
import fnmatch
import os
import re


def walk(root, rules):
    """Relative paths (posix) of every project file, excluding third-party and generated folders."""
    out = []
    excl = rules["exclude_dirs"]
    for dirpath, dirnames, filenames in os.walk(root):
        rel_dir = os.path.relpath(dirpath, root).replace(os.sep, "/")
        rel_dir = "" if rel_dir == "." else rel_dir
        dirnames[:] = [d for d in dirnames if not _excluded(f"{rel_dir}/{d}".lstrip("/"), d, excl)]
        for f in filenames:
            out.append(f"{rel_dir}/{f}".lstrip("/"))
    return sorted(out)


def _excluded(rel, name, excl):
    return any(rel == e or name == e for e in excl)


def check_root(root, rules, files, err):
    allowed = set(rules["root_allowed"])
    for entry in os.listdir(root):
        if entry not in allowed and not entry.startswith(".git"):
            err("RAIZ_LIMPA", entry, "item fora da lista permitida na raiz (rules.json/root_allowed)")


def check_junk(rules, files, err):
    for f in files:
        name = os.path.basename(f)
        if any(fnmatch.fnmatch(name, g) for g in rules["junk_globs"]):
            err("SEM_LIXO", f, "arquivo temporário/cópia: apague ou incorpore")


def check_single_doc(rules, files, err):
    allowed = set(rules["allowed_docs"])
    exempt = set(rules.get("doc_exempt_names", []))
    for f in files:
        if os.path.basename(f) in exempt:
            continue
        if os.path.splitext(f)[1].lower() in rules["doc_extensions"] and f not in allowed:
            err("DOC_UNICO", f, "o único documento do projeto é CLAUDE.md; mova o conteúdo para lá")


def count_lines(path):
    with open(path, encoding="utf-8", errors="replace") as fh:
        return fh.read().splitlines()


def check_sizes(root, rules, files, err):
    limits = rules["max_lines"]
    for f in files:
        ext = os.path.splitext(f)[1]
        if ext not in limits:
            continue
        n = len(count_lines(os.path.join(root, f)))
        if n > limits[ext]:
            err("TAMANHO_ARQUIVO", f, f"{n} linhas (máx {limits[ext]}): divida por responsabilidade")
        if n == 0:
            err("ARQUIVO_VAZIO", f, "arquivo vazio")


def check_function_sizes(root, rules, files, err):
    limits = rules["max_function_lines"]
    for f in files:
        ext = os.path.splitext(f)[1]
        if ext not in limits:
            continue
        lines = count_lines(os.path.join(root, f))
        spans = _gd_functions(lines) if ext == ".gd" else _cpp_functions(lines)
        for name, start, length in spans:
            if length > limits[ext]:
                err("TAMANHO_FUNCAO", f"{f}:{start}", f"função '{name}' com {length} linhas (máx {limits[ext]})")


def _gd_functions(lines):
    spans, cur = [], None
    for i, line in enumerate(lines):
        m = re.match(r"(static\s+)?func\s+(\w+)", line)
        if m or (cur and line and not line[0].isspace() and not line.startswith("#")):
            if cur:
                spans.append((cur[0], cur[1] + 1, _trim_len(lines, cur[1], i)))
            cur = (m.group(2), i) if m else None
    if cur:
        spans.append((cur[0], cur[1] + 1, _trim_len(lines, cur[1], len(lines))))
    return spans


def _trim_len(lines, start, end):
    while end > start and not lines[end - 1].strip():
        end -= 1
    return end - start


def _cpp_functions(lines):
    """Bodies opened at namespace level after a ')' line; good enough to flag god functions."""
    spans, depth, ns_depth, start, name = [], 0, 0, None, ""
    for i, line in enumerate(lines):
        code = re.sub(r"//.*", "", line)
        if re.match(r"\s*namespace\b", code) and "{" in code:
            ns_depth += 1
        for ch in code:
            if ch == "{":
                if depth == ns_depth and start is None and ")" in _context(lines, i):
                    start, name = i, _func_name(lines, i)
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == ns_depth and start is not None:
                    spans.append((name, start + 1, i - start + 1))
                    start = None
                if depth < ns_depth:
                    ns_depth = depth
    return spans


def _context(lines, i):
    return " ".join(lines[max(0, i - 2): i + 1])


def _func_name(lines, i):
    m = re.findall(r"([\w:~]+)\s*\(", _context(lines, i))
    return m[0] if m else "?"


def check_headers(root, rules, files, err):
    exts = set(rules["header_comment_extensions"])
    for f in files:
        if os.path.splitext(f)[1] not in exts:
            continue
        lines = [l for l in count_lines(os.path.join(root, f)) if l.strip()]
        if not lines:
            continue
        first = lines[0].lstrip()
        if lines[0].startswith("#!") and len(lines) > 1:
            first = lines[1].lstrip()
        ext = os.path.splitext(f)[1]
        prefixes = ("//", "/*") if ext in (".cpp", ".h", ".gdshader", ".glsl") else ("#", '"""')
        if not first.startswith(prefixes) or first.startswith("#include"):
            err("CABECALHO", f, "primeira linha deve ser um comentário dizendo a responsabilidade do arquivo")
