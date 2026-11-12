"""File-level gates: repo root, junk, single document, file and function sizes, header comments, sidecar files."""
import fnmatch
import os

import cpp_syntax
import gd_syntax
from scope import FIXTURES
from source import language_of


def check_root(ctx, err):
    """The root's entries as git lists them (scope.py: tracked plus untracked-not-ignored): what git ignores never enters the
    repository, so a folder the owner keeps beside it (.git/info/exclude) is not an entry of its root."""
    allowed = set(ctx.rules["root_allowed"])
    for entry in sorted({p.split("/", 1)[0] for p in ctx.listed}):
        if entry not in allowed and not entry.startswith(".git"):
            err("CLEAN_ROOT", entry, "entry outside rules.json/root_allowed at the repository root")


def check_junk(ctx, src, err):
    name = os.path.basename(src.path)
    if any(fnmatch.fnmatch(name, g) for g in ctx.rules["junk_globs"]):
        err("NO_JUNK", src.path, "temporary file or copy: delete it or fold it into its owner")


def in_doc_place(rules, path):
    """Whether a path lies in one of the declared document places (rules.json doc_places: Spec Kit's folders)."""
    return any(path == place or path.startswith(place + "/") for place in rules["doc_places"])


def check_single_doc(ctx, src, err):
    rules = ctx.rules
    if os.path.basename(src.path) in set(rules.get("doc_exempt_names", [])):
        return
    if (os.path.splitext(src.path)[1].lower() in rules["doc_extensions"] and src.path not in set(rules["allowed_docs"])
            and not in_doc_place(rules, src.path)):
        err("SINGLE_DOC", src.path, "the project's documents are CLAUDE.md, log/ and the declared places (rules.json doc_places): "
                                   "move the content there")


def check_sizes(ctx, src, err):
    limits = ctx.rules["max_lines"]
    ext = os.path.splitext(src.path)[1]
    if ext not in limits:
        return
    n = len(src.lines)
    if n > limits[ext]:
        err("FILE_SIZE", src.path, f"{n} lines (max {limits[ext]}): split it by responsibility")
    if n == 0:
        err("EMPTY_FILE", src.path, "empty file")


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
            err("FUNCTION_SIZE", f"{src.path}:{start}", f"function '{name}' has {length} lines (max {limit})")


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
        err("HEADER", src.path, "the file must start with a comment that states its responsibility")


def line_ending_inputs(ctx, path):
    """A file the change reached, wherever it lies (records and excluded folders included): the work wrote there."""
    return path in ctx.changed and not path.startswith(FIXTURES)


def check_line_endings(ctx, err):
    """LINE_ENDINGS (CLAUDE.md, files written as bytes): a file whose committed version had no carriage return gains none, and a
    new file has none. A file that was already CRLF keeps its form (the repository mixes them; the form is the file's own)."""
    for path in sorted(p for p in ctx.listed if line_ending_inputs(ctx, p)):
        with open(os.path.join(ctx.root, path), "rb") as fh:
            data = fh.read()
        if b"\r" not in data or b"\0" in data[:BINARY_PROBE]:
            continue
        before = ctx.before_bytes(path)
        if before is None or b"\r" not in before:
            what = "a new file" if before is None else "a file that had none"
            err("LINE_ENDINGS", path, f"carriage returns written into {what}: write the file's bytes with LF (the Edit and Write "
                                      "tools, or a script writing bytes)")


BINARY_PROBE = 8000  # git's own rule for telling a binary file: a NUL in its first 8000 bytes
SIDECARS = (".uid", ".import")


def check_sidecars(ctx, err):
    """A Godot .uid/.import sidecar belongs to its source file: without it the sidecar is an orphan."""
    for f in ctx.files:
        if f.endswith(SIDECARS) and f[: f.rindex(".")] not in ctx.listed_set:
            err("SIDECAR_WITHOUT_SOURCE", f, f"sidecar of '{f[: f.rindex('.')]}', which does not exist: delete it")
