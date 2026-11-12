"""The tracer's own words, one constant each: the names a pattern file gives to what it finds in a grammar (the capture
that anchors a fact and the captures naming its parts), the kinds of definition, of value and of link. None of them is
a name of a program, a library or a platform; the tracer's code writes no other name (tests/test_generic.py).
"""
from __future__ import annotations

# The captures that anchor a fact (one per pattern).
FUNCTION = "function"            # a function, a method, a lambda: +name, +type (what it returns), +body
CONSTRUCTOR = "constructor"      # a function that makes its class's objects
GETTER = "getter"                # a function a read of its name runs
SETTER = "setter"                # a function a write of its name runs
CLASS = "class"                  # a class, an interface, a struct: +name
NAMESPACE = "namespace"          # a named scope holding definitions: +name
VARIABLE = "variable"            # a variable, a constant, a field, a signal: +name, +type, +value, or +object (+key):
#                                  the property of its name (of that key) taken from an object
FIELD = "field"                  # a definition that is also a member of the class around it (a parameter property)
PARAM = "param"                  # a parameter of the function around it: +name, +type, +value
BASE = "base"                    # what the class (or the file, where a file is a class) around it extends
MODULE_NAME = "module.name"      # the name a file gives the class it is: +name
NAMED = "named"                  # a node standing for the name inside it (a name given arguments): +name
PASS = "pass"                    # an expression holding what one of its parts holds (a choice, brackets): +value
CAST = "cast"                    # an expression holding what its +value holds, and an object of its +type
ASSIGN = "assign"                # +left, +right
CALL = "call"                    # +callee, +args, or +props (the node whose pairs are its one argument)
NEW = "new"                      # an object made of a class: +callee, +args
MEMBER = "member"                # +object, +property
CHAIN = "chain"                  # an object followed by steps, flat: its first child, then each `step`
STEP = "step"                    # a step of a chain: +property, +args (a call), +construct (it makes an object), +key
INDEX = "index"                  # +object, +key
RETURN = "return"                # +value
SELF = "self"                    # the object a method runs on
SUPER = "super"                  # that object, read as its base class
IDENT = "ident"                  # a name read
LITERAL = "literal"              # a value the language writes in place (a number, a string, an array)
PAIR = "pair"                    # a named entry of a literal: +key, +value
IMPORT = "import"                # +source, and +name +alias, or +module (the whole file's alias), or +default; with
#                                  none of them, an expression whose value is the file
INCLUDE = "include"              # +source: every name of that file is a name of this one
EXPORT = "export"                # +name, +alias
EXPORT_DEFAULT = "export.default"   # +value
ANCHORS = (FUNCTION, CONSTRUCTOR, GETTER, SETTER, CLASS, NAMESPACE, VARIABLE, FIELD, PARAM, BASE, MODULE_NAME, NAMED,
           CAST, PASS, ASSIGN, CALL, NEW, MEMBER, CHAIN, STEP, INDEX, RETURN, SELF, SUPER, IDENT, LITERAL, PAIR, IMPORT,
           INCLUDE, EXPORT, EXPORT_DEFAULT)
DEFINING = (FUNCTION, CONSTRUCTOR, CLASS, NAMESPACE, VARIABLE, PARAM, FIELD)
# What a node is when read as an expression, the first that applies; the ones a node's kind alone can say (a pattern
# that is one kind of node and nothing else), which are kept by kind and not node by node; the facts the flow starts
# from.
ROLES = (IMPORT, CAST, NEW, CALL, CHAIN, STEP, MEMBER, INDEX, SELF, SUPER, LITERAL, IDENT)
BY_KIND = (SELF, SUPER, LITERAL, IDENT)
DRIVEN = (ASSIGN, RETURN, CALL, NEW, CHAIN)

# The captures naming a fact's parts.
NAME, TYPE, VALUE, BODY, LEFT, RIGHT = "name", "type", "value", "body", "left", "right"
CALLEE, ARGS, PROPS, OBJECT, PROPERTY, KEY, CONSTRUCT = "callee", "args", "props", "object", "property", "key", "construct"
SOURCE, ALIAS, WHOLE, DEFAULT = "source", "alias", "module", "default"

# Kinds of definition beyond FUNCTION, CLASS, NAMESPACE, VARIABLE and PARAM.
MODULE = "module"                # a file
KIND = {FUNCTION: FUNCTION, CONSTRUCTOR: FUNCTION, CLASS: CLASS, NAMESPACE: NAMESPACE, VARIABLE: VARIABLE,
        FIELD: VARIABLE, PARAM: PARAM}
CONTAINERS = (FUNCTION, CLASS, NAMESPACE)
SCOPES = (CLASS, NAMESPACE, MODULE)
STORED = (VARIABLE, PARAM)
RUNNING = (FUNCTION, CLASS, MODULE)          # the definitions code is written in

# Kinds of value a name can hold: a function, a scope (a class, a namespace, a file), an object of a class, an object
# of a type the code read does not define (a value is (kind, definition, how it was found)).
# A function an object was handed is kept by the object and is not its value: it runs where the object, or what was
# taken from it, is called.
FN, CLS, INST, OUT, KEPT = "fn", "cls", "inst", "out", "kept"

# Kinds of link (CALL and NEW as above) and how one was found: by the language's scopes and declared types, or by a
# name alone (a property written somewhere, a member of some class of that name).
HANDED = "handed"                # a function given to code the program does not define, which may run it
EXACT, BY_NAME = "exact", "by name"
RUNS = (CALL, NEW, HANDED)
OUTSIDE = (OUT, -1, EXACT)

# What an edge of the flow carries: every value as it is; found by name (the functions known exactly, marked so);
# handed to an object (those functions, as kept by it).
AS_NAME, AS_KEPT = "as name", "as kept"

# Node keys of the flow: a variable's values, a function's result, what is written under a property name, an
# expression's values, a step's.
VAR, RET, PROP, EXPR, PART = "var", "ret", "prop", "expr", "part"
