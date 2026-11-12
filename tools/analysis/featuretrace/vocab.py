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

# What a test proves of a name (`js_guards`, `gd_guards`): the guard of a NARROW tree, `(kind, ...)`.
G_TRUTHY = "truthy"                # (G_TRUTHY,): the value is truthy (ToBoolean)
G_MEMBER = MEMBER                  # (G_MEMBER, p): its property p is truthy (`x.p`, `x?.p`)
G_TYPEOF = "typeof"                # (G_TYPEOF, t): `typeof x === t`
G_INSTANCE = "instance"            # (G_INSTANCE, class tree): `x instanceof C`
G_HAS = "has"                      # (G_HAS, p): `"p" in x`
G_EQUALS = "equals"                # (G_EQUALS, constant): `x === c` with a constant of the language or a string
G_NULLISH = "nullish"              # (G_NULLISH,): `x == null`
G_MEMBER_EQUALS = "member-equals"  # (G_MEMBER_EQUALS, p, constant): `x.p === c`, a `switch` case
G_PREDICATE = "predicate"          # (G_PREDICATE, callee tree, index): a call, decided by what its callee declares
G_ASSERTS = "asserts"              # (G_ASSERTS, callee tree, index): after a call of one declared `asserts x is T`
G_IS = "is"                        # (G_IS, type text): GDScript `x is T`

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
JSX = "jsx"                # a JSX element: its tag handed to the JSX runtime with its props

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
SITE_DECLARED = "decl"     # an object of a declared type the platform gives (a result, a property, a declared variable)
SITE_DECLARED_FN = "declfn"    # a function of a declared function type the platform gives (a callback's argument)
SITE_RESOLVER = "resolver"     # the function `await` hands a thenable: what it is called with is the awaited value
SITE_GENERATOR = "gen"         # the generator object a generator function's call makes: its yields are its elements
SITE_PROMISE = "promise"       # the promise an async function's call makes: what it returns is its result
SITE_KEY = "key"               # a key enumerating one object gives (`for...in`): a string naming its members

# Fields of an abstract object beyond its named properties.
ELEMENTS_FIELD = "[[elements]]"     # an array's elements, what an unknown key reaches
ANY_FIELD = "[[any]]"               # what a write through an unknown key put: every named read sees it
RESULT_FIELD = "[[PromiseResult]]"  # a promise's result (ECMA-262's internal slot of that name)
PLATFORM_FIELD = "[[platform]]"     # what a declared call on an object kept: the platform's state on it
JSX_CHILDREN = "[[children]]"       # a JSX element's children, named as the runtime's declarations name them

# Declared types kept whole (js_sigs; heap.types): the platform's signatures as type trees, type parameters kept.
T_PARAM = "tparam"         # (T_PARAM, level, index): a type parameter of the declaring type, the function or an alias
T_NAMED = "tnamed"         # (T_NAMED, text, (type arguments...), file, owner): a type a name names, where it is written
T_FN = "tfn"               # (T_FN, (parameter types...), result type, rest index or -1, `this` type)
T_UNION = "tunion"         # (T_UNION, (parts...)): a union, a conditional type's branches
T_BOTH = "tboth"           # (T_BOTH, (parts...)): an intersection of object types: one value of every part
T_VALUES = "tvalues"       # (T_VALUES, (the language's values...)): a primitive, a literal type, `keyof`
T_OBJECT = "tobject"       # (T_OBJECT, class key): an object type written in place (an interface without a name)
T_THIS = "tthis"           # (T_THIS,): the `this` type, the object the member is read on
T_INDEX = "tindex"         # (T_INDEX, object type, key type): an indexed access type `T[K]`
T_TUPLE = "ttuple"         # (T_TUPLE, (parts...)): a tuple type
T_QUERY = "tquery"         # (T_QUERY, name, file, owner): `typeof x`, the value a name has where it is written
T_OPEN = "topen"           # (T_OPEN,): a type the declaration writes no value for (a mapped type's or an `infer`'s)
LEVEL_TYPE, LEVEL_OWN, LEVEL_ALIAS = 0, 1, 2      # whose type parameter: the declaring type's, the function's, an alias's
