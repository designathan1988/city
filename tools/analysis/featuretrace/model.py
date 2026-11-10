"""The facts the tracer reads from code: definitions (units) and the references written inside them.

A unit is anything the language lets one define with a name: a function, method, lambda, variable, field, constant,
signal, class, enum or enumerator. A reference is a name used inside a unit, with the construct that uses it (call, read,
write, emit, connect, a name carried by a string). Every value is kept as an expression tree (`ir`): the receiver of a
reference, the arguments of a call, the value written, a local's values, a return. Nothing here knows a project.
"""
from __future__ import annotations

from dataclasses import dataclass, field

# Methods the Godot engine itself calls on a script or an extension class (Godot 4 API, Object/Node/Control/Resource
# virtuals). A function with one of these names is an entry point: the engine, not the code, calls it.
ENGINE_CALLBACKS = frozenset({
    "_init", "_static_init", "_ready", "_process", "_physics_process", "_input", "_unhandled_input",
    "_unhandled_key_input", "_shortcut_input", "_gui_input", "_notification", "_draw", "_enter_tree", "_exit_tree",
    "_get", "_set", "_get_property_list", "_validate_property", "_property_can_revert", "_property_get_revert",
    "_to_string", "_can_drop_data", "_drop_data", "_get_drag_data", "_make_custom_tooltip", "_has_point",
    "_get_minimum_size", "_integrate_forces", "_get_configuration_warnings", "_run", "_iter_init", "_iter_next",
    "_iter_get", "_bind_methods", "_mouse_enter", "_mouse_exit",
})

# C++ standard-library container members that change the container they are called on (ISO C++ [containers]).
# A call to one of them on a field writes that field; its last argument is a value put into the container.
STD_MUTATORS = frozenset({
    "push_back", "emplace_back", "push_front", "emplace_front", "pop_back", "pop_front", "insert", "emplace",
    "emplace_hint", "try_emplace", "insert_or_assign", "erase", "clear", "resize", "assign", "swap", "push", "pop",
    "merge", "extract", "splice", "remove", "append", "fill", "store", "fetch_add", "fetch_sub", "exchange",
})

FUNCTION_KINDS = frozenset({"func", "lambda", "method"})
STATE_KINDS = frozenset({"var", "field", "const"})


@dataclass
class Ref:
    """One use of a name inside a unit."""
    kind: str                      # call, read, write, value, name, emit, connect, new, load, lambda, thread
    name: str                      # the simple name used ("" for a load or lambda)
    base: tuple | None = None      # the receiver's expression tree (`a` in `a.f()`), None for a plain name
    line: int = 0
    cond: tuple = ()               # enclosing branch conditions, outermost first
    qualified: bool = False        # C++ `A::name`: `scope` holds A
    scope: str = ""                # C++ qualified scope written before the name
    handler: tuple | None = None   # connect: the handler's expression tree
    target: int = -1               # lambda: local id of the lambda unit in the same file
    text: str = ""                 # load: the path; name: the literal
    argc: int = -1                 # call: the number of arguments written (-1 when not a call)
    args: tuple = ()               # call: each argument's tree; write: (the value written,)


@dataclass
class Unit:
    """One definition."""
    kind: str                      # func, method, lambda, var, field, const, signal, class, enum, enumerator
    lang: str                      # gd or cpp
    name: str
    qual: str                      # qualified name: gd "res://path.gd.Inner.f", cpp "ns::A::f"
    owner: str                     # gd: class key; cpp: qualified class or namespace ("" when global)
    file: str
    line: int
    end_line: int
    local_id: int = -1             # index inside its file
    gid: int = -1                  # index in the whole index
    params: list = field(default_factory=list)
    required: int = -1             # parameters without a default (-1: not counted)
    variadic: bool = False
    types: dict = field(default_factory=dict)      # local or parameter name -> [trees of the values it holds]
    values: list = field(default_factory=list)     # a variable's or field's initializer trees
    lambdas: dict = field(default_factory=dict)    # local name -> lambda local id
    type_text: str = ""            # a variable's declared type, a function's declared return type
    refs: list = field(default_factory=list)
    returns: list = field(default_factory=list)    # (line, expression text, tree) of each return
    static: bool = False
    file_local: bool = False       # C++ internal linkage: anonymous namespace or static free function
    decl_only: bool = False        # C++ declaration with no body
    virtual: bool = False
    bound_as: list = field(default_factory=list)   # names the engine can call it by (D_METHOD)


@dataclass
class ClassFact:
    """A class as written: its key, the name it is visible by, what it extends."""
    key: str
    lang: str
    name: str                      # gd: class_name or inner name ("" for an unnamed script); cpp: simple name
    file: str
    line: int
    bases: list = field(default_factory=list)      # gd: [spec text]; cpp: [type texts]
    outer: str = ""                # gd inner class: the outer class key; cpp: enclosing scope
    engine_visible: bool = False   # cpp: has _bind_methods (the engine knows it by name)
    params: list = field(default_factory=list)     # cpp class template: its type parameter names


@dataclass
class FileFacts:
    """Everything one file defines and uses."""
    path: str
    lang: str
    units: list = field(default_factory=list)
    classes: list = field(default_factory=list)
    usings: list = field(default_factory=list)     # cpp: namespaces opened with `using namespace`
    aliases: dict = field(default_factory=dict)    # cpp: using X = Y / typedef
    bindings: list = field(default_factory=list)   # cpp: (bound name, target text, class qual, line, kind)
    connections: list = field(default_factory=list)  # tscn: (signal, from script, to script, method, line)
    autoloads: dict = field(default_factory=dict)  # project.godot: name -> res:// path
    mentions: list = field(default_factory=list)   # data: (string, line)
    includes: list = field(default_factory=list)   # cpp: the paths `#include "..."` names
    errors: int = 0
