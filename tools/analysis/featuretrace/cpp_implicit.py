"""C++ calls the language makes without a call written (log/trace-rules.md: CALL, C7).

- declaring an object of class type calls its constructor (`T x;`, `T x(args);`, `T x{args};`) and, at the end of its
  scope, its destructor; `delete p` runs the destructor of what p points at;
- an expression with an operator on a class calls `operator<op>` (member or free, found by argument-dependent lookup);
- a range-for calls `begin()` and `end()` of the range.
Each is recorded as a reference; whether it reaches code of the program is decided by the resolver (a built-in operator
or a library type reaches nothing of the program).
"""
from __future__ import annotations

from . import scopes
from .cpp_names import strip_templates
from .source import field_of, named_children

IMPLICIT = "implicit"     # a call the language makes: reaching no class of the program is no external use
OBJECT_DECLARATORS = ("identifier", "function_declarator", "init_declarator", "array_declarator")
ASSIGN_OPERATORS = ("=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=")   # each overloadable


def declared_object(r, unit, declarator, type_text: str, node, cond, add) -> None:
    """`T x;`, `T x(args);`, `T x{args};`: T's constructor with those arguments, and its destructor at scope end."""
    if declarator.type not in OBJECT_DECLARATORS or not type_text or "*" in type_text or "&" in type_text:
        return
    args = ()
    if declarator.type == "function_declarator":
        params = field_of(declarator, "parameters")
        args = tuple(argument(r, a, unit) for a in named_children(params)) if params is not None else ()
    elif declarator.type == "init_declarator":
        value = field_of(declarator, "value")
        if value is None or value.type not in ("initializer_list", "argument_list"):
            return                                      # `T x = v;` copies a value: FLOW, no constructor written
        args = tuple(r.ir(a, unit) for a in named_children(value))
    scope, _, name = strip_templates(type_text.replace("const ", "")).strip().rpartition("::")
    # only a class type has a constructor (default-initialization of a non-class does nothing, C7)
    add(r, unit, "new", name, node, cond, qualified=bool(scope), scope=scope, args=args, text=IMPLICIT)
    add(r, unit, "destroy", name, node, cond, qualified=bool(scope), scope=scope)


def argument(r, param, unit) -> tuple:
    """A "parameter" of `T x(a, b);` in a block: a bare name is the value it names, passed to the constructor
    (direct-initialization; [dcl.ambig.res] with name lookup)."""
    typ = field_of(param, "type")
    if param.type == "parameter_declaration" and field_of(param, "declarator") is None and typ is not None             and typ.type in ("type_identifier", "identifier", "qualified_identifier"):
        return scopes.scoped_tree(r, ("name", strip_templates(r.src.text(typ))))
    return r.ir(param, unit)


def operator(r, unit, node, cond, add) -> None:
    """An operator on operands: `operator<op>` of the left operand's class, or a free one found by ADL."""
    kind = node.type
    if kind == "binary_expression":
        op, left, right = field_of(node, "operator"), field_of(node, "left"), field_of(node, "right")
    elif kind == "assignment_expression":
        op, left, right = field_of(node, "operator"), field_of(node, "left"), field_of(node, "right")
        if op is None or r.src.text(op) not in ASSIGN_OPERATORS:
            return
    elif kind in ("unary_expression", "update_expression"):
        op, left, right = field_of(node, "operator"), field_of(node, "argument"), None
    elif kind == "subscript_expression":
        index = field_of(node, "indices") or (named_children(node)[1:] or [None])[0]
        if index is not None and index.type == "subscript_argument_list":
            index = (named_children(index) or [None])[0]
        add(r, unit, "operator", "operator[]", node, cond, base=r.ir(field_of(node, "argument"), unit),
            args=(r.ir(index, unit),) if index is not None else ())
        return
    else:
        return
    if op is None or left is None:
        return
    args = (r.ir(right, unit),) if right is not None else ()
    add(r, unit, "operator", "operator" + r.src.text(op), node, cond, base=r.ir(left, unit), args=args)


def destroyed(r, unit, node, cond, add) -> None:
    """`delete p`: the destructor of what p points at."""
    target = field_of(node, "argument") or (named_children(node) or [None])[-1]
    if target is not None:
        add(r, unit, "destroy", "", node, cond, base=("deref", r.ir(target, unit)))


def range_calls(r, unit, right, node, cond, add) -> None:
    """A range-for over `right` calls `right.begin()` and `right.end()`."""
    base = r.ir(right, unit)
    for name in ("begin", "end"):
        add(r, unit, "operator", name, node, cond, base=base)
