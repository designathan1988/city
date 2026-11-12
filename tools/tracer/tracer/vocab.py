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
SHAPE = "shape"                  # an object written in place (a literal, a dictionary, an element's attributes): the
#                                  definitions written inside it with a name (an entry, a method) are its members
OPERATE = "operate"              # an operator applied to an object: +object, +property (the operator), +args
OPEN = "open"                    # a type that may be any value (one the language leaves open, one written in place)
ANCHORS = (FUNCTION, CONSTRUCTOR, GETTER, SETTER, CLASS, NAMESPACE, VARIABLE, FIELD, PARAM, BASE, MODULE_NAME, NAMED,
           CAST, PASS, ASSIGN, CALL, NEW, MEMBER, CHAIN, STEP, INDEX, RETURN, SELF, SUPER, IDENT, LITERAL, PAIR, IMPORT,
           INCLUDE, EXPORT, EXPORT_DEFAULT, SHAPE, OPERATE, OPEN)
DEFINING = (FUNCTION, CONSTRUCTOR, CLASS, NAMESPACE, VARIABLE, PARAM, FIELD, SHAPE)
# What a node is when read as an expression, the first that applies; the ones a node's kind alone can say (a pattern
# that is one kind of node and nothing else), which are kept by kind and not node by node; the facts the flow starts
# from.
ROLES = (IMPORT, CAST, NEW, CALL, OPERATE, CHAIN, STEP, MEMBER, INDEX, SELF, SUPER, LITERAL, IDENT)
BY_KIND = (SELF, SUPER, LITERAL, IDENT)
DRIVEN = (ASSIGN, RETURN, CALL, NEW, CHAIN)

# The captures naming a fact's parts.
NAME, TYPE, VALUE, BODY, LEFT, RIGHT = "name", "type", "value", "body", "left", "right"
CALLEE, ARGS, PROPS, OBJECT, PROPERTY, KEY, CONSTRUCT = "callee", "args", "props", "object", "property", "key", "construct"
SOURCE, ALIAS, WHOLE, DEFAULT = "source", "alias", "module", "default"
OPTIONAL, REST = "optional", "rest"          # a parameter that may be left out; one taking every argument after it

# Kinds of definition beyond FUNCTION, CLASS, NAMESPACE, VARIABLE, PARAM and SHAPE.
MODULE = "module"                # a file
KIND = {FUNCTION: FUNCTION, CONSTRUCTOR: FUNCTION, CLASS: CLASS, NAMESPACE: NAMESPACE, VARIABLE: VARIABLE,
        FIELD: VARIABLE, PARAM: PARAM, SHAPE: SHAPE}
CONTAINERS = (FUNCTION, CLASS, NAMESPACE, SHAPE)
SCOPES = (CLASS, NAMESPACE, MODULE)
CLASSES = (CLASS, SHAPE)                     # the definitions whose objects have members
STORED = (VARIABLE, PARAM)
RUNNING = (FUNCTION, CLASS, MODULE)          # the definitions code is written in

# Kinds of value a name can hold: a function, a scope (a class, a namespace, a file), an object of a class, an object
# of a type the code read does not define (a value is (kind, definition, how it was found)).
# A function an object was handed is kept by the object and is not its value: it runs where the object, or what was
# taken from it, is called.
FN, CLS, INST, OUT, KEPT = "fn", "cls", "inst", "out", "kept"
TXT = "txt"                      # a text written in the code that names a function: (TXT, the name, EXACT)
TOLD = "told"                    # such a name an object was handed in a text, and keeps
DECLARED, GLOBAL = -1, -2        # an object of a type not defined here: declared so (a member of it may still be a
#                                  script's where members are looked up as the program runs), or named by a name no
#                                  scope gives (the platform's own object: none of its members is the code's)
OBJECTS = (CLS, INST, OUT)       # the values that are objects

# Kinds of link (CALL and NEW as above) and how one was found: by the language's scopes and declared types, or by a
# name alone (a property written somewhere, a member of some class of that name).
HANDED = "handed"                # a function given to code the program does not define, which may run it
EXACT, BY_NAME, BY_TEXT = "exact", "by name", "by text"
STRENGTH = (BY_TEXT, BY_NAME, EXACT)         # of two ways one link was found, the later here is the one said
RUNS = (CALL, NEW, HANDED)
OUTSIDE = (OUT, DECLARED, EXACT)
PLATFORM = (OUT, GLOBAL, EXACT)
AMBIGUOUS = "ambiguous"          # a call found by name alone with more candidates than a trace follows
NEAR, DEEP, NONE = "near", "deep", "none"    # how far a trace follows what was found by name: one step, on, not
INLINE, EVERY = "inline", "every"            # which functions handed to code not read a trace goes into: the ones
#                                              written in the code handing them (it runs them there), or all

# What an edge of the flow carries: every value as it is; found by name (the functions known exactly, marked so);
# handed to an object (those functions, as kept by it); what the result of a call no definition answers is built
# with (those functions and the functions' names it was given in texts, as kept by it); the objects alone (what
# code not read gives back, or gives the functions it is handed, of the objects it was given).
AS_NAME, AS_KEPT, AS_BUILT, AS_OBJECTS = "as name", "as kept", "as built", "as objects"
AS_GUESS = "as guess"            # what a function or a member found by name gives: its functions alone, by name

# Node keys of the flow: a variable's values, a function's result, what is written under a property name, an
# expression's values, a step's.
VAR, RET, PROP, EXPR, PART = "var", "ret", "prop", "expr", "part"
ELEM = "elem"                    # what is written in an object written in place by a key not written there
