"""File-level gates: repo root, junk, single document, file and function sizes, header comments, sidecar files."""
import fnmatch
import os

import cpp_syntax
import gd_syntax
from source import language_of


def check_root(ctx, err):
    """The root's entries as git lists them (scope.py: tracked plus untracked-not-ignored): what git ignores never enters the
    repository, so a folder the owner keeps beside it (.git/info/exclude) is not an entry of its root."""
    allowed = set(ctx.rules["root_allowed"])
    for entry in sorted({p.split("/", 1)[0] for p in ctx.listed}):
        if entry not in allowed and not entry.startswith(".git"):
            err("RAIZ_LIMPA", entry, "entry outside rules.json/root_allowed at the repository root")


def check_junk(ctx, src, err):
    name = os.path.basename(src.path)
    if any(fnmatch.fnmatch(name, g) for g in ctx.rules["junk_globs"]):
        err("SEM_LIXO", src.path, "temporary file or copy: delete it or fold it into its owner")


def check_single_doc(ctx, src, err):
    rules = ctx.rules
    if os.path.basename(src.path) in set(rules.get("doc_exempt_names", [])):
        return
    allowed = rules["allowed_docs"]  # a path, or a folder when it ends in "/" (log/: the records)
    if src.path in allowed or any(a.endswith("/") and src.path.startswith(a) for a in allowed):
        return
    if os.path.splitext(src.path)[1].lower() in rules["doc_extensions"]:
        err("DOC_UNICO", src.path, "the project's documents are CLAUDE.md and log/: move the content there")


def check_sizes(ctx, src, err):
    limits = ctx.rules["max_lines"]
    ext = os.path.splitext(src.path)[1]
    if ext not in limits:
        return
    n = len(src.lines)
    if n > limits[ext]:
        err("TAMANHO_ARQUIVO", src.path, f"{n} lines (max {limits[ext]}): split it by responsibility")
    if n == 0:
        err("ARQUIVO_VAZIO", src.path, "empty file")


def function_limit(rules, path):
    """The limit of the file's extension, else of its language (a C++ header is held to the C++ limit)."""
    limits = rules["max_function_lines"]
    ext = os.path.splitext(path)[1]
    if ext in limits:
        return limits[ext]
    lang = language_of(path)
    same = [v for e, v in limits.items() if lang and language_of("x" + e) == lang]
    return min(same) if same else None


def check_function_sizes(ctx, src, err):
    limit = function_limit(ctx.rules, src.path)
    if limit is None or src.lang not in ("gd", "cpp"):
        return
    if src.lang == "gd":
        spans = gd_syntax.function_spans(gd_syntax.logical_lines(src))
    else:
        spans = cpp_syntax.function_spans(src.code())
    for name, start, length in spans:
        if length > limit:
            err("TAMANHO_FUNCAO", f"{src.path}:{start}", f"function '{name}' has {length} lines (max {limit})")


def check_headers(ctx, src, err):
    ext = os.path.splitext(src.path)[1]
    if ext not in set(ctx.rules["header_comment_extensions"]) or not src.text.strip():
        return
    toks = src.tokens
    if toks and toks[0].kind == "comment" and toks[0].text.startswith("#!"):
        toks = toks[1:]
    first = toks[0] if toks else None
    is_doc = first is not None and src.lang == "py" and first.kind == "string"
    if first is None or not (first.kind == "comment" or is_doc) or not any(c.isalpha() for c in first.text):
        err("CABECALHO", src.path, "the file must start with a comment that states its responsibility")


SIDECARS = (".uid", ".import")


def check_sidecars(ctx, err):
    """A Godot .uid/.import sidecar belongs to its source file: without it the sidecar is an orphan."""
    for f in ctx.files:
        if f.endswith(SIDECARS) and f[: f.rindex(".")] not in ctx.listed_set:
            err("SIDECAR_WITHOUT_SOURCE", f, f"sidecar of '{f[: f.rindex('.')]}', which does not exist: delete it")
