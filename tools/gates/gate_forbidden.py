"""The forbidden constructs of rules.json/forbidden, each judged on syntax (tokens, includes, receivers), never on raw
lines: rules.json gives each one its id, paths, exceptions and message (and MACHINE_PATH its pattern); every id has its
check in CHECKS, and an id with none is refused when the rules are listed (registry._forbidden)."""
import re

import cpp_syntax
import gate_text
from tokens import string_body

MARKER = re.compile(r"\b(TODO|FIXME|XXX|HACK)\b", re.I)
_HTML_COLOR = re.compile(r"^#[0-9a-fA-F]{6}(?:[0-9a-fA-F]{2})?$")
_SCENE_CALLS = {"change_scene_to_file", "change_scene_to_packed", "change_scene_to_node"}
_TREE_ONLY = {"quit", "set_pause"}
_EXCLUSIVE_GUARDS = {"unique_lock", "lock_guard", "scoped_lock"}  # the standard's guards that hold a mutex exclusively
_MAP_LOCK = {"shared_mutex", "map_mutex"}  # the map's mutex, by its type (the only shared_mutex of the core) or its name
_HAND_LOCKS = {"lock", "try_lock", "try_lock_for", "try_lock_until"}
_HANDLES = {"&", "&&", "*"}


def applies(rule, path):
    excepted = any(path == e or (e.endswith("/") and path.startswith(e)) for e in rule.get("except", []))
    return any(path.startswith(p) for p in rule["paths"]) and not excepted


def markers(ctx, src, rule, err):
    """Open-item markers, in any case, inside comment tokens only (an identifier named `todo` is code, not a marker)."""
    for t in src.comments():
        for m in MARKER.finditer(t.text):
            err(rule["id"], f"{src.path}:{t.line + t.text.count(chr(10), 0, m.start())}", rule["message"])


def includes_prefix(prefix):
    def check(ctx, src, rule, err):
        if src.lang not in ("cpp", "glsl"):
            return
        for inc, _angle, line in cpp_syntax.includes(src):
            if inc.startswith(prefix):
                err(rule["id"], f"{src.path}:{line}", rule["message"])
    return check


def _statement(code, i):
    """The tokens from code[i] to the end of its declaration: the next `;`, `{` or `,` outside parentheses, or the `)` that
    closes a list it stands in (a parameter)."""
    depth = 0
    for j in range(i, len(code)):
        x = code[j].text
        depth += {"(": 1, ")": -1}.get(x, 0)
        if depth < 0 or (depth == 0 and x in (";", "{", ",")):
            return code[i:j]
    return code[i:]


def map_writer(ctx, src, rule, err):
    """The map held exclusively by anything but map_hold::WriterHold, the hold that names its writer: a standard guard of a
    shared_mutex or of map_mutex (typed, deduced or by an alias of its type), and map_mutex locked by hand."""
    if src.lang != "cpp":
        return
    code = src.code()
    n = len(code)
    for i, t in enumerate(code):
        if t.kind != "ident":
            continue
        guard = t.text in _EXCLUSIVE_GUARDS and any(x.text in _MAP_LOCK for x in _statement(code, i))
        by_hand = (t.text == "map_mutex" and i + 3 < n and code[i + 1].text in (".", "->")
                   and code[i + 2].text in _HAND_LOCKS and code[i + 3].text == "(")
        if guard or by_hand:
            err(rule["id"], f"{src.path}:{t.line}", rule["message"])


def _qualified_start(code, i):
    """Where the qualified name ending at code[i] begins (`city::CityState` begins at `city`, `::CityState` at `::`)."""
    while i >= 1 and code[i - 1].text == "::":
        i -= 2 if i >= 2 and code[i - 2].kind == "ident" else 1
    return max(i, 0)


def render_reads(ctx, src, rule, err):
    """render/ only reads the city: it includes no command, and the CityState it is handed is const (by reference or pointer),
    never cast to writable; the ChangeLog it consumes is handed apart."""
    includes_prefix("sim/commands/")(ctx, src, rule, err)
    if src.lang != "cpp":
        return
    code = src.code()
    n = len(code)
    for i, t in enumerate(code):
        if t.kind != "ident" or t.text != "CityState" or i + 1 >= n:
            continue
        start = _qualified_start(code, i)
        before = code[start - 1].text if start >= 1 else ""
        cast = before == "<" and start >= 2 and code[start - 2].text == "const_cast"
        writable = code[i + 1].text in _HANDLES and before != "const"
        if cast or writable:
            err(rule["id"], f"{src.path}:{t.line}", rule["message"])


def core_owner(ctx, src, rule, err):
    """The CityCore class named in code: as an identifier or as a string (ClassDB, scene node types)."""
    for t in src.code():
        if (t.kind == "ident" and t.text == "CityCore") or (t.kind == "string" and string_body(t) == "CityCore"):
            err(rule["id"], f"{src.path}:{t.line}", rule["message"])


def _tree_names(code):
    """Names that hold the SceneTree in this script: assigned from get_tree()/Engine.get_main_loop() or typed SceneTree."""
    names = set()
    for i, t in enumerate(code[:-2]):
        if t.text == "var" and code[i + 1].kind == "ident":
            rest = [x.text for x in code[i + 2:i + 7]]
            if rest[:2] == [":", "SceneTree"] or "get_tree" in rest[:3] or "get_main_loop" in rest[:5]:
                names.add(code[i + 1].text)
        if t.kind == "ident" and code[i + 1].text == ":" and code[i + 2].text == "SceneTree":
            names.add(t.text)
    return names


def central_flow(ctx, src, rule, err):
    """Scene changes, pause and quit: the SceneTree calls, whatever variable holds the tree."""
    if src.lang != "gd":
        return
    code = src.code()
    trees = _tree_names(code)
    for i, t in enumerate(code):
        if t.kind != "ident":
            continue
        hit = t.text in _SCENE_CALLS
        if not hit and i >= 2 and code[i - 1].text == ".":
            recv = code[i - 2]
            on_tree = recv.text in trees or (recv.text == ")" and i >= 4 and code[i - 4].text == "get_tree")
            nxt = code[i + 1].text if i + 1 < len(code) else ""
            hit = on_tree and ((t.text in _TREE_ONLY and nxt == "(") or (t.text == "paused" and nxt == "="))
        if hit:
            err(rule["id"], f"{src.path}:{t.line}", rule["message"])


def loose_style(ctx, src, rule, err):
    """Colors and theme overrides written in code instead of the design tokens."""
    if src.lang == "godot_res":
        for t in src.code():
            if t.kind == "ident" and ("Color(" in t.text or t.text.startswith("theme_override_")):
                err(rule["id"], f"{src.path}:{t.line}", rule["message"])
        return
    if src.lang != "gd":
        return
    code = src.code()
    for i, t in enumerate(code):
        nxt = code[i + 1].text if i + 1 < len(code) else ""
        if t.kind == "ident" and ((t.text in ("Color", "Color8") and nxt in ("(", "."))
                                  or (t.text.startswith("add_theme_") and t.text.endswith("_override"))):
            err(rule["id"], f"{src.path}:{t.line}", rule["message"])
        elif t.kind == "string" and _HTML_COLOR.match(string_body(t)):
            err(rule["id"], f"{src.path}:{t.line}", rule["message"])


def data_reader(ctx, src, rule, err):
    """A path into res://data/ written anywhere but the data reader: whole .json paths and folder prefixes alike."""
    for t in src.code():
        if t.kind != "string":
            continue
        for m in re.finditer(r"res://data/[^\s\"']*", string_body(t)):
            tail = m.group(0).rsplit("/", 1)[-1]
            if "." not in tail or tail.endswith(".json"):
                err(rule["id"], f"{src.path}:{t.line}", rule["message"])


def _receiver(code, i):
    """Last identifier of the expression before code[i] (the `.`/`->` of a member call); `jobs()` gives `jobs`."""
    k = i - 1
    if k >= 0 and code[k].text == ")":
        depth = 0
        while k >= 0:
            depth += {")": 1, "(": -1}.get(code[k].text, 0)
            if depth == 0:
                break
            k -= 1
        inner = [t for t in code[k + 1:i - 1] if t.kind == "ident"]
        k -= 1
        if k >= 0 and code[k].kind == "ident":
            return code[k].text
        return inner[-1].text if inner else None
    return code[k].text if k >= 0 and code[k].kind == "ident" else None


def job_keys(ctx, src, rule, err):
    """A job given to a JobPool without a key that says its type: a braced list or a number straight to submit() on any
    JobPool-typed receiver. A key written out with its lane (JobKey{ JobLane::X, id }) or held in a JobKey is the rule itself:
    the lane is the user's own, so two users cannot collide."""
    if src.lang != "cpp":
        return
    facts = ctx.facts(src.path)
    pools = set(facts.get("jobpool_names", [])) | job_pool_members(ctx)
    code = src.code()
    n = len(code)
    for i, t in enumerate(code):
        nxt = code[i + 1].text if i + 1 < n else ""
        if t.kind == "ident" and t.text == "submit" and nxt == "(" and i >= 2 and code[i - 1].text in (".", "->"):
            first = code[i + 2] if i + 2 < n else None
            if _receiver(code, i - 1) in pools and first is not None and (first.text == "{" or first.kind == "number"):
                err(rule["id"], f"{src.path}:{t.line}", rule["message"])


def job_pool_members(ctx):
    """JobPool-typed names declared in native/src headers (members reached through other objects, s.jobs)."""
    def build():
        names = set()
        for f in ctx.files:
            if f.startswith("native/src/") and f.endswith(".h"):
                names.update(ctx.facts(f).get("jobpool_names", []))
        return names
    return ctx.lazy("job_pool_members", build)


def machine_paths(ctx, src, rule, err):
    """A home folder of one machine (a person's name with it) on any line, comments and strings alike: a comment leaks it as
    much as a string does."""
    if src.binary:
        return
    rx = re.compile(rule["pattern"])
    for n, line in enumerate(src.lines, 1):
        if rx.search(line):
            err(rule["id"], f"{src.path}:{n}", rule["message"])


CHECKS = {
    "NO_LOOSE_MARKERS": markers,
    "SIM_WITHOUT_GODOT": includes_prefix("godot"),
    "RENDER_READ_ONLY": render_reads,
    "MAP_WRITER_UNNAMED": map_writer,
    "ONE_CORE_OWNER": core_owner,
    "CENTRAL_FLOW": central_flow,
    "UI_LOOSE_STYLE": loose_style,
    "TEXT_THROUGH_TR": gate_text.untranslated,
    "DATA_ONE_READER": data_reader,
    "JOB_WITHOUT_KEY": job_keys,
    "MACHINE_PATH": machine_paths,
}
