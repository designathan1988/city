"""Document gates: the map file lists every source file and no file that no longer exists; CLAUDE.md stays short and is the
only source of instructions (PRECEDENCE_CLAIM, NESTED_CLAUDE); known problems are kept in the register alone
(PROBLEMS_ELSEWHERE)."""
import json
import os
import re
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from tasks import inputs as tree  # noqa: E402  the one owner of path globs

GENERATED = (".uid", ".import", ".translation")  # Godot sidecars and import outputs: they belong to their source file
CLAUDE_MD = "CLAUDE.md"
ORDER_SOURCES = (CLAUDE_MD,)  # the project's only source of orders
SETTINGS = ".claude/settings.json"
INSTRUCTION_NAMES = ("CLAUDE.md", "CLAUDE.local.md")  # the files Claude Code loads as instructions wherever they stand
RULES_DIR = ".claude/rules/"  # path-scoped rules, instructions too (Claude Code's memory documentation)
# A claim of precedence among instructions: the words below, with a source of orders as their object (a right of way in the
# game's own words, one road given way to by another, is no claim of this kind).
CLAIM = re.compile(r"(?i:\bbinding over\b|\b(wins?|outranks?|overrides?|takes? precedence)\b[^.;\n]{0,40}"
                   r"\b(claude\.md|memor(y|ies)|skills?|records?|instructions?|orders?)\b)|\bBINDING\b")
QUOTED = re.compile(r'"[^"]*"|`[^`]*`|\u201c[^\u201d]*\u201d')  # a citation: the words are someone else's, not a claim here
# Known problems live in doc/problems.jsonl alone: no ledger of problems and no table of symptoms and causes elsewhere. The
# register's generated view and the histories (records of what was, by their place) are no tracker.
REGISTER_VIEW = "doc/problems.md"
HISTORIES = "doc/history/"
LEDGER_NAMES = ("ledger.json",)
TABLE_ROW = re.compile(r"^\s*\|.+\|\s*$")
SYMPTOM = {"symptom", "sintoma"}
CAUSE = {"cause", "root cause", "causa", "causa raiz"}


def mapped(f, rules):
    if f.endswith(GENERATED):
        return False
    return any(f == r or f.startswith(r + "/") for r in rules["mapped_roots"])


def inputs(ctx, path):
    return path in (*ORDER_SOURCES, ctx.rules["map_file"]) or (path in ctx.file_set and mapped(path, ctx.rules))


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
            err("MAP_OUTDATED", f, f"file missing from {map_file} (add a line with its responsibility)")
    for p in sorted(listed):
        if p not in listed_files:
            err("MAP_OUTDATED", map_file, f"the map names `{p}`, which no longer exists")
    lines = ctx.source("CLAUDE.md").text.count("\n")
    limit = rules["claude_md_max_lines"]
    if lines > limit:
        err("CLAUDE_MD", "CLAUDE.md", f"{lines} lines: keep it concise (up to {limit}); the detail goes to doc/")


def _scanned(ctx, path):
    return not any(path == s or path.startswith(s + "/") for s in ctx.rules["instruction_skip"])


def instruction_inputs(ctx, path):
    return _scanned(ctx, path)


def _settings(ctx):
    try:
        with open(os.path.join(ctx.root, SETTINGS), encoding="utf-8") as fh:
            return json.load(fh)
    except (OSError, json.JSONDecodeError):
        return None


def _excluded(ctx, path, settings):
    """A file the settings keep from loading (claudeMdExcludes, globs matched against the absolute path)."""
    full = os.path.abspath(os.path.join(ctx.root, path)).replace("\\", "/")
    return any(tree.glob_regex(g).match(full) for g in (settings or {}).get("claudeMdExcludes", []))


def _text(ctx, path):
    """A listed file's text through the run's Sources (each file read once a run, whichever rule asks first): "" for a
    binary one (a NUL in its head, or no UTF-8) or one gone from the disk."""
    try:
        src = ctx.source(path)
    except OSError:
        return ""
    return "" if src.binary else src.text


def check_instructions(ctx, err):
    """CLAUDE.md is the only source of instructions: no precedence claim anywhere else (a citation in quotes or backticks is
    no claim), no other instruction file Claude Code would load."""
    settings = _settings(ctx)
    for path in sorted(p for p in ctx.listed if _scanned(ctx, p)):
        name = os.path.basename(path)
        if path != CLAUDE_MD and (name in INSTRUCTION_NAMES or path.startswith(RULES_DIR)) and not _excluded(ctx, path, settings):
            err("NESTED_CLAUDE", path, "an instruction file other than the root's CLAUDE.md: its orders belong in "
                                       "that file")
        if path in ORDER_SOURCES:
            continue
        for n, line in enumerate(_text(ctx, path).splitlines(), 1):
            bare = QUOTED.sub(" ", line)
            if CLAIM.search(bare):
                err("PRECEDENCE_CLAIM", f"{path}:{n}", "a claim of precedence outside CLAUDE.md: only it states what "
                                                       "wins (a citation of one is written in quotes)")


def problems_inputs(ctx, path):
    return _scanned(ctx, path) and (path.endswith(".md") or os.path.basename(path) in LEDGER_NAMES)


def check_problems_elsewhere(ctx, err):
    """No problem tracker but the register: a ledger of problems, or a table whose header names a symptom and a cause, in
    any tracked file but the register's view and the histories."""
    for path in sorted(p for p in ctx.listed if problems_inputs(ctx, p)):
        if path == REGISTER_VIEW or path.startswith(HISTORIES):
            continue
        if os.path.basename(path) in LEDGER_NAMES:
            err("PROBLEMS_ELSEWHERE", path, "a problem ledger outside doc/problems.jsonl: its problems belong in the register "
                                            "(python tools/atlas/cli.py problems add)")
            continue
        for n, line in enumerate(_text(ctx, path).splitlines(), 1):
            columns = {c.strip().strip("*").lower() for c in line.strip().strip("|").split("|")} if TABLE_ROW.match(line) else ()
            if columns and columns & SYMPTOM and columns & CAUSE:
                err("PROBLEMS_ELSEWHERE", f"{path}:{n}", "a table of symptoms and causes outside the register: each open "
                                                         "problem belongs in doc/problems.jsonl, what was belongs in a history")
