"""UI scripts keep no copy of the game's numbers (Redux principle: keep state minimal and derive the rest): no member of a
UI script takes a value read from the store (CityStore.snapshot, query, cached, constants); the widget reads them when it
draws. What a widget keeps of its own interaction (a press point, a hover, a timer, the tab open, its node references) is
not game state and is allowed. Exported configuration is allowed while the script never writes it."""
import gd_syntax

UI_ROOT = "project/ui/"
ASSIGN_OPS = {"=", "+=", "-=", "*=", "/=", "%=", "**=", "&=", "|=", "^=", "<<=", ">>="}
# What an exported setting is when it is configuration (a value tuned in the editor), not wiring (a node reference set by code).
VALUE_TYPES = {"int", "float", "bool", "String", "StringName", "Color", "Vector2", "Vector2i", "Vector3", "Vector3i", "Rect2",
               "Array", "Dictionary"}


def _assignments(lines, decl):
    """(line, right-hand tokens) of every assignment to the member after its declaration (`name =`, `self.name +=`...)."""
    for ln in lines:
        if ln.first == decl.line:
            continue
        toks = ln.toks
        for i, t in enumerate(toks[:-1]):
            if t.text == decl.name and toks[i + 1].text in ASSIGN_OPS:
                prev = toks[i - 1].text if i else ""
                if prev != "." or (i >= 2 and toks[i - 2].text == "self"):
                    yield ln.first, toks[i + 2:]


def applies(ctx, path):
    return path.startswith(UI_ROOT) and path.endswith(".gd")


STORE = "CityStore"
STORE_READS = {"snapshot", "query", "cached", "constants"}


def _reads_store(toks):
    """The tokens read one of the store's game numbers (CityStore.snapshot / query / cached / constants)."""
    return any(toks[i].text == STORE and toks[i + 1].text == "." and toks[i + 2].text in STORE_READS
               for i in range(len(toks) - 2))


def _store_copies(lines, decl):
    """Lines where the member takes a value read from the store: its initializer, or an assignment to it."""
    found = [decl.line] if _reads_store(decl.init_toks) else []
    return found + [at for at, rhs in _assignments(lines, decl) if _reads_store(rhs)]


def check_ui_state(ctx, src, err):
    lines = gd_syntax.logical_lines(src)
    for d in gd_syntax.declarations(lines):
        if d.keyword != "var" or not d.member:
            continue
        exported = any(a.startswith("export") for a in d.annotations)
        setting = not d.type_toks or d.type_toks[0].text in VALUE_TYPES
        if exported and setting and any(True for _ in _assignments(lines, d)):
            err("ESTADO_NA_UI", f"{src.path}:{d.line}",
                f"exported '{d.name}' is configuration and the script writes it: keep the setting where it is set")
        for at in _store_copies(lines, d):
            err("ESTADO_NA_UI", f"{src.path}:{at}",
                f"member '{d.name}' keeps a copy of the store's numbers: read CityStore.snapshot/cached when drawing, never keep them")
