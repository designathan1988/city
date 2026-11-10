"""The languages' rules the tracer follows, as their official references state them, in one table.

Every reserved word of each language gets the role it plays when a feature is traced:
- define: introduces a name (a definition the walk can land on);
- import: brings a name or a file from elsewhere (a link to follow);
- branch / loop: opens paths that are all followed, with their condition kept;
- jump: ends or leaves a path (`return` carries the value back to the call site);
- scope: names where to look a name up (`self`, `super`, `this`, `::` targets);
- type: a type test or a type name (narrows what a receiver is);
- value: a literal or an operator (no link).
Sources: GDScript reference, "Keywords" (docs.godotengine.org/en/4.7/tutorials/scripting/gdscript/gdscript_basics.html);
C++ keywords and preprocessor tokens (cppreference.com/cpp/keyword, cppreference.com/cpp/preprocessor); Godot's names
the engine calls by: Object.call/call_deferred, Signal.connect, Object.connect, Callable (docs class_object, class_signal,
class_callable), and GDExtension bindings: ClassDB::bind_method(D_METHOD), ADD_SIGNAL(MethodInfo), ADD_PROPERTY
(docs engine_details/architecture/object_class.html, tutorials/scripting/cpp/gdextension_cpp_example.html).
"""
from __future__ import annotations

from .rule_table import CPP_RULES, GD_RULES, RULE_IDS  # noqa: F401  (the construct table)

GDSCRIPT_KEYWORDS = {
    "if": "branch", "elif": "branch", "else": "branch", "match": "branch", "when": "branch",
    "for": "loop", "while": "loop", "in": "loop",
    "break": "jump", "continue": "jump", "pass": "value", "return": "jump",
    "class": "define", "class_name": "define", "extends": "import", "signal": "define", "func": "define",
    "static": "define", "const": "define", "enum": "define", "var": "define",
    "is": "type", "as": "type", "void": "type",
    "self": "scope", "super": "scope",
    "preload": "import", "await": "jump", "yield": "jump", "assert": "value", "breakpoint": "value",
    "PI": "value", "TAU": "value", "INF": "value", "NAN": "value",
}

CPP_KEYWORDS = {
    **dict.fromkeys(("class", "struct", "union", "enum", "namespace", "typedef", "template", "concept", "friend",
                     "operator", "static", "extern", "inline", "virtual", "explicit", "mutable", "thread_local",
                     "constexpr", "consteval", "constinit", "register"), "define"),
    **dict.fromkeys(("using", "export", "import", "module"), "import"),
    **dict.fromkeys(("if", "else", "switch", "case", "default", "try", "catch", "requires"), "branch"),
    **dict.fromkeys(("for", "while", "do"), "loop"),
    **dict.fromkeys(("return", "break", "continue", "goto", "throw", "co_return", "co_yield", "co_await"), "jump"),
    **dict.fromkeys(("this", "public", "protected", "private"), "scope"),
    **dict.fromkeys(("auto", "decltype", "typename", "typeid", "sizeof", "alignof", "alignas", "bool", "char",
                     "char8_t", "char16_t", "char32_t", "wchar_t", "short", "int", "long", "signed", "unsigned",
                     "float", "double", "void", "const", "volatile", "static_cast", "dynamic_cast", "const_cast",
                     "reinterpret_cast"), "type"),
    **dict.fromkeys(("new", "delete"), "define"),
    **dict.fromkeys(("true", "false", "nullptr", "noexcept", "static_assert", "asm", "and", "and_eq", "bitand",
                     "bitor", "compl", "not", "not_eq", "or", "or_eq", "xor", "xor_eq", "contract_assert",
                     "override", "final"), "value"),
}

CPP_PREPROCESSOR = {
    "include": "import", "embed": "import", "define": "define", "undef": "define",
    "if": "branch", "ifdef": "branch", "ifndef": "branch", "elif": "branch", "elifdef": "branch",
    "elifndef": "branch", "else": "branch", "endif": "branch", "line": "value", "error": "value",
    "warning": "value", "pragma": "value",
}

ROLES = frozenset({"define", "import", "branch", "loop", "jump", "scope", "type", "value"})

# Godot: a method or a signal named by a string the engine resolves at run time.
NAME_CALLS = ("call", "call_deferred", "callv", "rpc", "rpc_id")      # Object.call*(method: StringName, ...)
SIGNAL_OPS = ("emit", "connect", "disconnect", "is_connected")       # Signal methods
BIND_CALLS = ("bind", "unbind", "bindv")                              # Callable.bind*: the same target
GDEXT_BIND = ("bind_method", "bind_static_method", "bind_vararg_method")  # ClassDB, with D_METHOD("name")
GDEXT_SIGNAL = "ADD_SIGNAL"                                           # ADD_SIGNAL(MethodInfo("name", ...))
GDEXT_PROPERTY = "ADD_PROPERTY"                                       # ADD_PROPERTY(PropertyInfo(.., "p"), set, get)

# Where each rule of log/trace-rules.md is handled ("module:Qualified.name"); tests/test_rules.py imports each one.
HANDLERS = {
    "DEF": ("gd_read:GdReader.unit", "cpp_read:CppReader.unit", "index:Index"),
    "LOOK": ("resolve:Resolver.bare", "resolve:Resolver.qualified", "resolve:Resolver.adl", "index:Index.visible",
             "typer:Typer.named"),
    "VALUE": ("typer:Typer.evaluate", "ir_gd:build", "ir_cpp:build"),
    "EXPR": ("ir_gd:build", "ir_cpp:build", "typer:Typer.evaluate"),
    "FLOW": ("typer:Typer.local", "typer:Typer.held", "typer:Typer.returns", "resolve:Resolver.argument_values",
             "resolve:Resolver.assigned_values"),
    "CALL": ("link:link_ref", "link:with_overrides", "link:fits", "resolve:Resolver.member", "link:accessor",
             "link:destructors", "link:implicit_construction", "cpp_implicit:declared_object", "cpp_implicit:operator",
             "cpp_implicit:range_calls", "cpp_implicit:destroyed", "gd_read:GdReader.accessors"),
    "BRANCH": ("gd_body:statement", "cpp_body:statement"),
    "JUMP": ("gd_body:statement", "cpp_body:statement"),
    "NAME": ("resolve:Resolver.by_string", "typer:Typer.made_by_name"),
    "SIGNAL": ("link:connect",),
    "BIND": ("cpp_expr:registration", "index:Index.bind", "resolve:Resolver.engine_members"),
    "ENTRY": ("walk:entry_reason",),
    "TYPE": ("typer:Typer.type_class",),
}

# Callables built from an object and a method name (G7): constructor -> (object argument, name argument).
CALLABLE_CONSTRUCTORS = {"Callable": (0, 1)}

# The engine's constructors by name (G6): (global receiver, method) -> index of the argument naming the class.
NAME_CONSTRUCTORS = {("ClassDB", "instantiate"): 0}

# C++ casts and the library's makers: the type a templated call yields.
CASTS = ("static_cast", "dynamic_cast", "const_cast", "reinterpret_cast", "bit_cast", "any_cast", "get", "get_if")
MAKERS = {"make_shared": "std::shared_ptr", "make_unique": "std::unique_ptr", "make_optional": "std::optional"}


def keywords(lang: str) -> dict:
    return GDSCRIPT_KEYWORDS if lang == "gd" else CPP_KEYWORDS
