"""C++ calls the language makes without a call written (log/trace-rules.md: CALL, C7).

- declaring an object of class type calls its constructor (`T x;`, `T x(args);`, `T x{args};`; each element of an
  array its own) and, at the end of its scope, its destructor; `delete p` runs the destructor of what p points at;
- an expression with an operator on a class calls `operator<op>` (member or free, found by argument-dependent lookup);
- a range-for calls `begin()` and `end()` of the range.
Each is recorded as a reference; whether it reaches code of the program is decided by the resolver (a built-in operator
or a library type reaches nothing of the program).
"""
from __future__ import annotations

from .cpp_names import strip_templates
from .source import field_of, named_children

IMPLICIT = "implicit"     # a call the language makes: reaching no class of the program is no external use
OBJECT_DECLARATORS = ("identifier", "function_declarator", "init_declarator", "array_declarator")
ASSIGN_OPERATORS = ("=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=")   # each overloadable


def declared_object(r, unit, declarator, type_text: str, node, cond, add, args: tuple = ()) -> None:
    """`T x;`, `T x(args);`, `T x{args};`: T's constructor with those arguments (`T x(a, b);` read as an object: `args`,
    its initializers, `cpp_ambig`), and its destructor at scope end. An array of T is an aggregate ([dcl.init.aggr]):
    each element is initialized from its own initializer-clause (a braced one constructs it with its elements; a value
    copies it: FLOW), the elements with no clause by T's default constructor. A pointer or a reference declared
    constructs nothing."""
    if declarator.type not in OBJECT_DECLARATORS or not type_text or "*" in type_text or "&" in type_text:
        return
    pointer, array = declared_shape(declarator)
    if pointer:
        return
    calls = [()]
    if declarator.type == "function_declarator":
        calls = [args]
    elif declarator.type == "init_declarator":
        value = field_of(declarator, "value")
        if value is None or value.type not in ("initializer_list", "argument_list"):
            return                                      # `T x = v;` copies a value: FLOW, no constructor written
        clauses = named_children(value)
        if not array:
            calls = [tuple(r.ir(a, unit) for a in clauses)]
        else:
            calls = [tuple(r.ir(a, unit) for a in named_children(c)) for c in clauses if c.type == "initializer_list"]
            written, size = array_size(r, declarator)
            if not clauses or written and (size is None or size > len(clauses)):
                calls.append(())                        # the elements with no clause: value-initialized
    scope, _, name = strip_templates(type_text.replace("const ", "")).strip().rpartition("::")
    # only a class type has a constructor (default-initialization of a non-class does nothing, C7)
    for args in calls:
        add(r, unit, "new", name, node, cond, qualified=bool(scope), scope=scope, args=args, text=IMPLICIT)
    add(r, unit, "destroy", name, node, cond, qualified=bool(scope), scope=scope)


def declared_shape(declarator) -> tuple[bool, bool]:
    """What a declarator declares beside its name ([dcl.decl]): (a pointer or a reference, an array)."""
    pointer = array = False
    while declarator is not None and declarator.type not in ("identifier", "field_identifier", "function_declarator"):
        if declarator.type in ("pointer_declarator", "reference_declarator"):
            pointer = True
        elif declarator.type == "array_declarator":
            array = True
        if declarator.type == "parenthesized_declarator":
            declarator = (named_children(declarator) or [None])[0]
        else:
            declarator = field_of(declarator, "declarator")
    return pointer, array


def array_size(r, declarator) -> tuple[bool, int | None]:
    """Whether an array declarator writes its size, and the size when it is a number (`T x[4]`); a size not written
    (`T x[] = {...}`) is the initializer-clauses' count ([dcl.array])."""
    while declarator is not None and declarator.type != "array_declarator":
        declarator = field_of(declarator, "declarator")
    size = field_of(declarator, "size") if declarator is not None else None
    text = r.src.text(size).strip() if size is not None else ""
    return size is not None, int(text) if text.isdigit() else None


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
