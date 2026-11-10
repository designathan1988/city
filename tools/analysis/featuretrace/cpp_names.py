"""C++ names: a declarator's name through its wrappers, template arguments dropped, scopes joined."""
from __future__ import annotations

from .source import field_of, named_children

WRAPPERS = ("pointer_declarator", "reference_declarator", "array_declarator", "init_declarator",
            "parenthesized_declarator", "attributed_declarator", "type_qualifier")


def strip_templates(text: str) -> str:
    out, depth = [], 0
    for ch in text:
        if ch == "<":
            depth += 1
        elif ch == ">":
            depth = max(0, depth - 1)
        elif depth == 0 and not ch.isspace():
            out.append(ch)
    return "".join(out)


def unwrap(node):
    """The name inside a declarator: through pointers, references, arrays, initialisers, parentheses."""
    while node is not None and node.type in WRAPPERS:
        inner = field_of(node, "declarator")
        if inner is None:
            kids = [c for c in named_children(node) if c.type not in ("type_qualifier", "attribute_specifier")]
            inner = kids[-1] if kids else None
        node = inner
    return node


def join(*parts: str) -> str:
    return "::".join(p for p in parts if p)


from .rules import CASTS, MAKERS  # noqa: E402



def template_call_type(text: str) -> str | None:
    """The type a templated call yields by the language's and the library's rules: a cast or `std::get<T>` gives T,
    `make_shared<T>` / `make_unique<T>` a pointer to T. Other templated calls say nothing by their arguments."""
    if "<" not in text:
        return None
    name = strip_templates(text).split("::")[-1]
    args = template_args(text)
    if not args or args[0].strip().isdigit():
        return None
    if name in CASTS:
        return type_of(args[0])
    if name in MAKERS:
        return f"{MAKERS[name]}<{type_of(args[0])}>"
    return None


def type_of(text: str) -> str:
    """A type as written, on one line (template arguments kept: the resolver looks through wrappers)."""
    return " ".join(text.split())


def template_args(text: str) -> list[str]:
    """The top-level template arguments of a type: `map<K, vector<V>>` -> [K, vector<V>]."""
    start = text.find("<")
    if start < 0:
        return []
    args, depth, current = [], 0, []
    for ch in text[start + 1:]:
        if ch == "<":
            depth += 1
        elif ch == ">":
            if depth == 0:
                break
            depth -= 1
        elif ch == "," and depth == 0:
            args.append("".join(current).strip())
            current = []
            continue
        current.append(ch)
    tail = "".join(current).strip()
    return args + ([tail] if tail else [])
