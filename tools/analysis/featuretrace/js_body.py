"""JavaScript / TypeScript statements (log/trace-rules.md, JavaScript and TypeScript rows; J1, J5, J7).

A block hoists its function declarations first (they are visible in the whole block, J7). Every path of `if`, `switch`,
loops, `try` and labels is followed with its condition (BRANCH). Each name that receives a value records its tree
(FLOW): declarations, destructuring, assignments, loop variables (`for...of`: the elements, after the implicit
`[Symbol.iterator]()` call; `for...in`: the keys), `catch` parameters; `return` keeps its value (JUMP). A write to a
captured local reaches the enclosing function's (closures share their variables, J7).
"""
from __future__ import annotations

from . import c_family, js_bounds, js_class, js_decl, js_expr, js_guards, jsdoc, narrowing, scopes
from .cpp_implicit import IMPLICIT
from .gd_body import squash
from .ir import put
from .ir_js import FUNCTIONS, member_name
from .model import assign, declare, declare_function
from .rules import HANDLER_PREFIX, ITERATOR_MEMBER
from .source import field_of, named_children, string_value

__all__ = ("IMPLICIT", "squash")
HOISTED = ("function_declaration", "generator_function_declaration")
SILENT = ("break_statement", "continue_statement", "empty_statement", "debugger_statement", "comment",
          "type_alias_declaration", "interface_declaration")


def block(r, node, unit, cond) -> None:
    """A block: its function declarations first (J7), then each statement; what a statement proves for the ones after
    it (an `if` whose other branch leaves, an assertion call) holds over the rest of the block (R1)."""
    items = named_children(node) if node is not None else []
    scopes.open_scope(r)
    for st in items:
        if st.type in HOISTED:
            local_function(r, st, unit)
    opened = 0
    for i, st in enumerate(items):
        after = statement(r, st, unit, cond)
        if after:
            narrowing.open(r, after, items[i + 1:], "js")
            opened += 1
    for _ in range(opened):
        narrowing.close(r)
    scopes.close_scope(r)


def guarded(r, facts: list, nodes: list, unit, cond) -> None:
    """Statements read where facts hold (R1)."""
    narrowing.open(r, facts, nodes, "js")
    for node in nodes:
        statement(r, node, unit, cond)
    narrowing.close(r)


def local_function(r, node, unit) -> int:
    """A function declaration in a function: a local of its block (hoisted: visible in the whole block, J7), the same
    one when the block's hoisting already declared it."""
    name = r.src.text(field_of(node, "name"))
    lam = r.function(node, name, unit.owner, unit, kind="lambda").local_id
    if unit.lambdas.get(scopes.visible(r, name)) != lam:
        declare_function(r, unit, name, lam)
    return lam


def statement(r, st, unit, cond) -> list | None:
    """One statement read by its rule; returns the facts it proves for the statements after it, if any."""
    src = r.src
    kind = st.type
    if kind == "statement_block":
        block(r, st, unit, cond)
    elif kind in ("lexical_declaration", "variable_declaration"):
        for declarator in (c for c in named_children(st) if c.type == "variable_declarator"):
            local(r, declarator, unit, cond, function_scope=kind == "variable_declaration")   # `var`: the function's
    elif kind in HOISTED:
        local_function(r, st, unit)
    elif kind in ("class_declaration", "abstract_class_declaration"):
        js_class.class_declaration(r, st, src.text(field_of(st, "name")), unit.owner, unit)
    elif kind == "enum_declaration":
        js_decl.declaration(r, st, unit.owner, unit)
    elif kind == "if_statement":
        when_true, when_false = c_family.if_statement(r, st, unit, cond, statement, expr, js_guards.facts)
        return narrowing.after_if(st, when_true, when_false, "js")
    elif kind == "switch_statement":
        value = field_of(st, "value")
        expr(r, value, unit, cond)
        entered_alone = True                    # a case only its own test enters: the one before it leaves
        for case in named_children(field_of(st, "body")):
            test = field_of(case, "value")
            body = [sub for sub in named_children(case) if sub is not test]
            label = f"switch {squash(src.text(value))}: " + ("case " + squash(src.text(test)) if test else "default")
            expr(r, test, unit, cond)
            facts = js_guards.case_facts(r, value, test, unit) if test is not None and entered_alone else []
            guarded(r, facts, body, unit, cond + (label,))
            entered_alone = any(narrowing.leaves(sub, "js") for sub in body)
    elif kind == "for_statement":
        scopes.open_scope(r)                    # `for (let ...)`: the loop's own scope
        parts = []
        for name in ("initializer", "condition", "increment"):
            part = field_of(st, name)
            if part is not None:
                parts.append(src.text(part).rstrip(";").strip())
                statement(r, part, unit, cond)
        body = field_of(st, "body")
        facts = js_guards.facts(r, field_of(st, "condition"), unit)[0]
        guarded(r, facts, [body], unit, cond + ("for " + squash("; ".join(parts)),))
        scopes.close_scope(r)
    elif kind == "for_in_statement":
        scopes.open_scope(r)
        loop(r, st, unit, cond)
        scopes.close_scope(r)
    elif kind in ("while_statement", "do_statement"):
        test = field_of(st, "condition")
        expr(r, test, unit, cond)
        facts = js_guards.facts(r, test, unit)[0] if kind == "while_statement" else []   # a do's body runs first
        guarded(r, facts, [field_of(st, "body")], unit, cond + ("while " + squash(src.text(test)),))
    elif kind == "try_statement":
        block(r, field_of(st, "body"), unit, cond)
        handler = field_of(st, "handler")
        if handler is not None:
            scopes.open_scope(r)                # the catch parameter is local to its clause
            param = field_of(handler, "parameter")
            if param is not None:
                r.bind(param, ("literal", ""), unit, None, cond)
            label = "catch " + squash(src.text(param)) if param is not None else "catch"
            block(r, field_of(handler, "body"), unit, cond + (label,))
            scopes.close_scope(r)
        final = field_of(st, "finalizer")
        if final is not None:
            block(r, field_of(final, "body"), unit, cond + ("finally",))
    elif kind == "labeled_statement":
        statement(r, field_of(st, "body"), unit, cond)
    elif kind == "with_statement":
        expr(r, field_of(st, "object"), unit, cond)
        statement(r, field_of(st, "body"), unit, cond + ("with " + squash(src.text(field_of(st, "object"))),))
    elif kind == "return_statement":
        values = [c for c in named_children(st) if c.type != "comment"]
        unit.returns.append((src.line(st), squash(src.text(values[0])) if values else "",
                             r.ir(values[0], unit) if values else ("literal", "undefined")))
        for v in values:
            expr(r, v, unit, cond)
    elif kind in SILENT:
        return None
    else:
        expr(r, st, unit, cond)
        return js_guards.asserted(r, st, unit)         # a call of an assertion function (`asserts x is T`)
    return None


def local(r, declarator, unit, cond, function_scope: bool = False) -> None:
    """`const x = v` inside a function: a function or class written to it takes its name (J9); `var` belongs to the
    function's scope, `let`/`const` to the block's."""
    target, value = field_of(declarator, "name"), field_of(declarator, "value")
    if target.type == "identifier" and value is not None and value.type in FUNCTIONS:
        name = r.src.text(target)
        lam = r.function(value, name, unit.owner, unit, kind="lambda").local_id
        declare_function(r, unit, name, lam, function_scope=function_scope)
        return
    if target.type == "identifier" and value is not None and value.type == "class":
        js_class.class_declaration(r, value, r.src.text(target), unit.owner, unit)
        return
    tree = r.ir(value, unit) if value is not None else ("literal", "undefined")
    declared = r.type_values(field_of(declarator, "type"), unit)
    if target.type == "identifier":
        key = declare(r, unit, r.src.text(target), declared + ([tree] if value is not None else []),
                      function_scope=function_scope)
        if field_of(declarator, "type") is not None:
            unit.bounds[key] = js_bounds.of(r, field_of(declarator, "type"))      # what it can hold (S2)
        elif jsdoc.is_js(r) and jsdoc.declared(r, declarator):
            unit.bounds[key] = jsdoc.declared(r, declarator)                      # its JSDoc `@type` (S2')
    else:
        r.bind(target, tree, unit, None, cond)
    expr(r, value, unit, cond)


def loop(r, st, unit, cond) -> None:
    """`for (x of xs)`: xs's `[Symbol.iterator]()` is called and x takes each element; `for (k in o)`: o's enumerable
    property names as strings (P2)."""
    left, right = field_of(st, "left"), field_of(st, "right")
    operator = field_of(st, "operator")
    walked = r.ir(right, unit)
    of = operator is not None and r.src.text(operator).strip() == "of"
    value = ("elements", walked) if of else ("keys", walked)
    declared = field_of(st, "kind") is not None
    r.bind(left, value, unit, None if declared else True, cond)
    expr(r, right, unit, cond)
    if of:
        r.add(unit, "call", ITERATOR_MEMBER, st, cond, base=walked, text=IMPLICIT, argc=0)
    label = f"for ({squash(r.src.text(left))} {'of' if of else 'in'} {squash(r.src.text(right))})"
    statement(r, field_of(st, "body"), unit, cond + (label,))


def write_name(r, node, name: str, value: tuple, unit, cond) -> None:
    assign(r, unit, name, value, shared=True)
    r.add(unit, "write", name, node, cond, args=(value,))


def target(r, node, unit, cond, value: tuple | None) -> None:
    """The left side of an assignment: the name, member or element written and the value that flows into it."""
    if node is None:
        return
    kind = node.type
    args = (value,) if value is not None else ()
    if kind == "identifier":
        if value is not None:
            write_name(r, node, r.src.text(node), value, unit, cond)
        else:
            r.add(unit, "write", r.src.text(node), node, cond)
    elif kind in ("member_expression", "subscript_expression"):
        obj = field_of(node, "object")
        key = field_of(node, "property") if kind == "member_expression" else field_of(node, "index")
        name = member_name(r.src, key) if kind == "member_expression" else string_value(r.src, key)
        expr(r, obj, unit, cond)
        if name is None:                                        # o[k] = v: a value put into o
            expr(r, key, unit, cond)
            if value is not None:                               # the write itself, its object and key kept as trees
                r.add(unit, "store", "", node, cond, base=r.ir(obj, unit), args=(value,), key=r.ir(key, unit))
            target(r, obj, unit, cond, put(value) if value is not None else None)
            return
        base = r.ir(obj, unit)
        if base == ("self",) and "#" in unit.owner:
            field = js_class.field_unit(r, unit.owner, name, node, None)   # `this.x = v` makes the property x (J4)
            if not field.bound and jsdoc.is_js(r):
                field.bound = jsdoc.declared(r, node)                      # `/** @type {T} */ this.x = v` (S2')
        r.add(unit, "write", name, node, cond, base=base, args=args)
        if value is not None and name.startswith(HANDLER_PREFIX) and len(name) > len(HANDLER_PREFIX):
            # `x.on<type> = handler` subscribes to the event `<type>` (J6)
            r.add(unit, "listen", name[len(HANDLER_PREFIX):], node, cond, base=base, handler=value)
    elif kind in ("parenthesized_expression", "non_null_expression"):
        target(r, (named_children(node) or [None])[0], unit, cond, value)
    elif kind in ("object_pattern", "array_pattern"):
        r.bind(node, value if value is not None else ("literal", ""), unit, True, cond)
    else:
        expr(r, node, unit, cond)


def decorator(r, node, unit) -> None:
    """`@d` / `@d(args)`: d (or what d(args) returns) is called with the class or member (J8)."""
    inner = (named_children(node) or [None])[0]
    if inner is None:
        return
    expr(r, inner, unit, ())
    r.add(unit, "invoke", "", node, (), base=r.ir(inner, unit), args=(("literal", ""),))


def expr(r, node, unit, cond) -> None:
    js_expr.expression(r, node, unit, cond)
