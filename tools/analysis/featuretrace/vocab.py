"""The tracer's own vocabulary, one constant each, so the analysis (`heap`) writes no name (log/trace-rules.md, "The
feature tracer is generic"; tests/test_generic.py): the shapes of the expression trees the readers build (`ir`), the
kinds of reference a unit makes (`model.Ref`), the kinds of link the walk reads (`resolve.Edge`), the kinds of unit
(`model.Unit`), and the kinds of abstract object the analysis makes. None of them names a program, a library or an API.
"""
from __future__ import annotations

# Tree shapes (ir.py).
NAME = "name"
MEMBER = "member"
CALL = "call"
INDEX = "index"
OP = "op"
UNARY = "unary"
CAST = "cast"
NEW = "new"
CONSTRUCT = "construct"
CALLABLE = "callable"
LITERAL = "literal"
UNION = "union"
COLLECTION = "collection"
ELEMENTS = "elements"
EACH = "each"
KEYS = "keys"
PUT = "put"
AWAIT = "await"
STRNAME = "strname"
NARROW = "narrow"
SELF = "self"
SUPER = "super"
MODULE = "module"
DEREF = "deref"
ADDR = "addr"

# Reference kinds (model.Ref).
READ = "read"
WRITE = "write"
STORE = "store"            # a write through a key no literal names (`o[k] = v`)
VALUE = "value"
LAMBDA = "lambda"
INVOKE = "invoke"
BOUND = "bound"
HAND = "hand"
IMPORT = "import"

# Link kinds (resolve.Edge) beyond the reference kinds above.
HANDED = "handed"

# Unit kinds (model.Unit).
FUNC = "func"
METHOD = "method"
FIELD = "field"
VAR = "var"
CONST = "const"
CLASS_UNIT = "class"

# Kinds of abstract object (heap.objects).
SITE_NEW = "new"           # an object a `new` of a class makes, at its place in the source
SITE_LITERAL = "lit"       # an object literal, at its place
SITE_ARRAY = "arr"         # an array literal, at its place
SITE_FUNCTION = "fn"       # a function's object
SITE_CLASS = "class"       # a class itself (its constructor)
SITE_PROTOTYPE = "proto"   # the prototype object of a class or of a constructor function
SITE_MODULE = "module"     # a module's namespace
SITE_PRIMITIVE = "prim"    # a value of one of the language's primitive kinds
SITE_TEXT = "str"          # a string the analysis knows the text of
SITE_UNKNOWN = "unknown"   # what no declaration describes: counted in the report

# Fields of an abstract object beyond its named properties.
ELEMENTS_FIELD = "[[elements]]"     # an array's elements, what an unknown key reaches
ANY_FIELD = "[[any]]"               # what a write through an unknown key put: every named read sees it
