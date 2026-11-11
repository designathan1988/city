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

from .rule_table import CPP_RULES, GD_RULES, JS_RULES, RULE_IDS  # noqa: F401  (the construct table)

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

# JavaScript (MDN Lexical grammar: reserved words, strict-mode and module reservations) and TypeScript's contextual
# keywords (handbook): log/trace-rules.md J1-J12
JS_KEYWORDS = {
    **dict.fromkeys(("class", "function", "const", "let", "var", "using", "static", "get", "set", "async", "enum",
                     "interface", "type", "namespace", "module", "declare", "abstract", "accessor", "global",
                     "constructor"), "define"),
    **dict.fromkeys(("import", "export", "from", "require", "extends", "implements", "default", "as"), "import"),
    **dict.fromkeys(("if", "else", "switch", "case", "try", "catch", "finally", "with"), "branch"),
    **dict.fromkeys(("for", "while", "do", "of", "in"), "loop"),
    **dict.fromkeys(("return", "break", "continue", "throw", "yield", "await"), "jump"),
    **dict.fromkeys(("this", "super", "new", "private", "protected", "public", "package", "meta", "target"),
                    "scope"),
    **dict.fromkeys(("typeof", "instanceof", "keyof", "infer", "is", "asserts", "satisfies", "readonly", "override",
                     "unique", "any", "unknown", "never", "object", "string", "number", "boolean", "symbol",
                     "bigint", "void"), "type"),
    **dict.fromkeys(("true", "false", "null", "undefined", "delete", "debugger"), "value"),
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
    "DEF": ("gd_read:GdReader.unit", "cpp_read:CppReader.unit", "index:Index", "js_read:JsReader.unit",
            "js_decl:declaration", "js_class:class_declaration", "js_class:object_literal",
            "js_class:members"),
    "LOOK": ("resolve:Resolver.bare", "resolve:Resolver.qualified", "resolve:Resolver.adl", "index:Index.visible",
             "typer:Typer.named", "resolve:Resolver.js_scoped", "js_modules:JsModules.resolve",
             "js_modules:JsModules.export", "js_modules:JsModules.binding", "js_modules:JsModules.class_of"),
    "VALUE": ("typer:Typer.evaluate", "ir_gd:build", "ir_cpp:build", "ir_js:build"),
    "EXPR": ("ir_gd:build", "ir_cpp:build", "typer:Typer.evaluate", "ir_js:build", "js_expr:expression"),
    "FLOW": ("typer:Typer.wire_local", "typer:Typer.hold_node", "typer:Typer.ret_node", "flows:Flows.flows_of",
             "flows:Flows.carry", "flows:Flows.engine_calls", "solver:Solver.add", "solver:Solver.subscribe",
             "solver:Solver.propagate", "typer:Typer.wire", "transfer:member_one", "transfer:step_one",
             "solver:Solver.read", "solver:Solver.grew", "typer:Typer.canon"),
    "CALL": ("link:link_ref", "link:with_overrides", "link:fits", "resolve:Resolver.member", "link:accessor",
             "link:destructors", "link:implicit_construction", "cpp_implicit:declared_object", "cpp_implicit:operator",
             "cpp_implicit:range_calls", "cpp_implicit:destroyed", "gd_read:GdReader.accessors", "link:invoked",
             "link:handed", "js_link:constructors", "js_link:module_run", "js_link:jsx", "js_link:callback_args",
             "js_expr:call", "js_expr:implicit", "js_body:loop"),
    "BRANCH": ("gd_body:statement", "cpp_body:statement", "js_body:statement"),
    "JUMP": ("gd_body:statement", "cpp_body:statement", "js_body:statement"),
    "NAME": ("resolve:Resolver.by_string", "names:made_by_name", "names:callable_of", "names:texts"),
    "SIGNAL": ("link:connect", "js_link:listen", "js_link:fire", "js_expr:events"),
    "BIND": ("cpp_expr:registration", "index:Index.bind", "resolve:Resolver.engine_members"),
    "ENTRY": ("walk:entry_reason", "js_decl:module_body"),
    "TYPE": ("typer:Typer.type_class", "js_read:JsReader.type_values"),
}

# Callables built from an object and a method name (G7): constructor -> (object argument, name argument).
CALLABLE_CONSTRUCTORS = {"Callable": (0, 1)}

# The engine's constructors by name (G6): (global receiver, method) -> index of the argument naming the class.
NAME_CONSTRUCTORS = {("ClassDB", "instantiate"): 0}

# C++ casts and the library's makers: the type a templated call yields.
CASTS = ("static_cast", "dynamic_cast", "const_cast", "reinterpret_cast", "bit_cast", "any_cast", "get", "get_if")
MAKERS = {"make_shared": "std::shared_ptr", "make_unique": "std::unique_ptr", "make_optional": "std::optional"}
# [utility], [refwrap], [utility.as.const]: each returns its argument itself (a cast of its value category, a wrapper
# that calls through to it), so a call of one holds what its argument holds
PASS_THROUGH = frozenset({"move", "forward", "move_if_noexcept", "as_const", "ref", "cref"})


# Events by type (J6, J11): subscribing a handler, firing an event; the host's or a library's when no class of the
# program defines the method called. (method -> index of the type argument, index of the handler argument)
EVENT_SUBSCRIBE = {"addEventListener": (0, 1), "on": (0, 1), "once": (0, 1), "addListener": (0, 1),
                   "prependListener": (0, 1), "prependOnceListener": (0, 1)}
EVENT_FIRE = {"dispatchEvent": 0, "emit": 0}           # the event object / its type
MESSAGE_FIRE = {"postMessage": "message"}              # a message to the other side of a channel or worker
HANDLER_PREFIX = "on"                                  # `x.on<type> = handler` (HTML event handler properties)
EVENT_OBJECTS = ("Event", "CustomEvent", "MessageEvent", "KeyboardEvent", "MouseEvent", "PointerEvent")
# Callbacks of the language's own containers and promises: what each parameter of the callback receives (J1, J6).
ELEMENTS, ACCUMULATOR, AWAITED, ENGINE_VALUE = "elements", "accumulator", "awaited", "engine"
HIGHER_ORDER = {
    **dict.fromkeys(("forEach", "map", "filter", "find", "findIndex", "findLast", "findLastIndex", "some", "every",
                     "flatMap"), (ELEMENTS,)),
    "sort": (ELEMENTS, ELEMENTS), "toSorted": (ELEMENTS, ELEMENTS),
    "reduce": (ACCUMULATOR, ELEMENTS), "reduceRight": (ACCUMULATOR, ELEMENTS),
    "then": (AWAITED,), "catch": (ENGINE_VALUE,), "finally": (),
}
# Function.prototype.call/apply/bind (J2): the arguments after thisArg reach the function's parameters.
CALL_FORMS = {"call": "spread", "apply": "array"}
BIND_FORMS = ("bind",)
# Calls the language makes (J5): the member it calls, written as the program names it.
ITERATOR_MEMBER = "[Symbol.iterator]"
COERCION_MEMBERS = ("valueOf", "toString", "[Symbol.toPrimitive]")
THEN_MEMBER = "then"
CONSTRUCTOR = "constructor"


def keywords(lang: str) -> dict:
    return GDSCRIPT_KEYWORDS if lang == "gd" else JS_KEYWORDS if lang == "js" else CPP_KEYWORDS
