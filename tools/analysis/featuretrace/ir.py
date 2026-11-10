"""The expression tree (IR): what an expression is, built from its syntax node by its construct (log/trace-rules.md EXPR).

Shapes (tuples, so facts stay picklable and comparable):
  ("name", s)                    a name, `A::b` qualified as written            LOOK
  ("member", base, s, arrow)     `base.s` / `base->s`                            member access
  ("call", callee, args)         a call; args is a tuple of IR                    CALL
  ("index", base, arg)           `base[arg]`
  ("op", op, left, right)        a binary operator
  ("unary", op, operand)
  ("cast", type_text)            a value of that type (casts, `as`, makers, `preload`: "path:<file>")
  ("new", type_text)             a constructed object of that type
  ("callable", lambda_local_id)  a lambda as a value
  ("literal", type_text)         a literal ("" when the language's own)
  ("union", a, b)                either value (a ternary)
  ("collection", values)         a container literal holding these values
  ("elements", e)                what walking `e` yields (a loop variable)
  ("put", e)                     a value put into a container (an element)
  ("await", e)                   what awaiting `e` gives
  ("strname", s)                 a name written as a string (NAME)
  ("self",) ("super",) ("node",) ("deref", e) ("addr", e)
"""
from __future__ import annotations


def name(text: str) -> tuple:
    return ("name", text)


def union(a: tuple | None, b: tuple | None) -> tuple:
    if a is None or a == b:
        return b if b is not None else ("literal", "")
    if b is None:
        return a
    return ("union", a, b)


def elements(e: tuple) -> tuple:
    return ("elements", e)


def put(e: tuple) -> tuple:
    return ("put", e)


LITERAL = ("literal", "")
