"""The facts the tracer reads from code: definitions (units) and the references written inside them.

A unit is anything the language lets one define with a name: a function, method, lambda, variable, field, constant,
signal, class, enum or enumerator. A reference is a name used inside a unit, with the construct that uses it (call, read,
write, emit, connect, a name carried by a string). Every value is kept as an expression tree (`ir`): the receiver of a
reference, the arguments of a call, the value written, a local's values, a return. Nothing here knows a project.
"""
from __future__ import annotations

from dataclasses import dataclass, field

from . import scopes

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
ALIAS_KIND = "alias"     # js: a type alias (`type X = T`) or a type no text names: holds the values its type admits (T1')


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
    object_of: tuple = ()          # cpp: a use of `T x(a, b);` read as an object: kept if a or b is a value (cpp_ambig)
    receiver: tuple | None = None  # js: the object written as `this` in `f.call(t)`, `f.apply(t)`, `f.bind(t)` (J2'')
    site: int = -1                 # new: where the object is made (its allocation site, the start of the expression)
    key: tuple | None = None       # store: the key no literal names (`o[k] = v`), as a tree


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
    declared: list = field(default_factory=list)   # js: the value trees a function's return annotation allows (T3)
    generator: bool = False        # js: `function*`: a call gives a generator object, whose iteration gives its yields
    asynchronous: bool = False     # js: `async`: a call gives a promise whose result is what it returns
    this_type: tuple = ()          # js: the declared type of its `this` parameter (`js_sigs`), () where none
    yields: list = field(default_factory=list)     # js: what each `yield` gives (`yield* e`: e's elements)
    refs: list = field(default_factory=list)
    returns: list = field(default_factory=list)    # (line, expression text, tree) of each return
    static: bool = False
    file_local: bool = False       # C++ internal linkage: anonymous namespace or static free function
    decl_only: bool = False        # C++ declaration with no body
    virtual: bool = False
    bound_as: list = field(default_factory=list)   # names the engine can call it by (D_METHOD)
    accessors: dict = field(default_factory=dict)  # property: "get"/"set" -> local unit id or (gd) function name
    accessor_of: str = ""          # the property whose set/get this function is (gd: its own name is direct)
    outer: int = -1                # lambda: local id of the function it is written in (same file)
    captured: set = field(default_factory=set)     # lambda: names that are still the enclosing function's locals
    written_by: dict = field(default_factory=dict)  # local name -> local ids of the closures that write it (shared)
    tails: set = field(default_factory=set)        # cpp: lines of the calls in tail position (a compiler may jump)
    local_types: dict = field(default_factory=dict)    # cpp: a class declared in the function, name -> class key
    objects: list = field(default_factory=list)    # cpp: `T x(a, b);` read as a function: an object if a or b is a value
    predicate: tuple = ()          # js: a return type `[asserts] x is T`: (x's index, T's value trees, asserts) (G1)
    defaults: set = field(default_factory=set)     # js: parameters written with a default (an omitted argument's)
    optional: bool = False         # js: a method declared optional (`m?(): T`): reading it may give undefined (T1')
    bounds: dict = field(default_factory=dict)     # js: parameter or local -> its declared type's bound (S2)
    bound: tuple = ()              # js: a field's, variable's or alias's declared type's bound; a function's result's
    late_writes: dict = field(default_factory=dict)    # js: name -> what a closure writes to a variable its enclosing
                                                        # function declares later in the text (J7')
    constructs: str = ""           # js: the class of the objects `new` on this function makes, its prototype object's
                                   # (N1', N1''): an ordinary function's (no arrow, method, async function, generator)
    this_of: str = ""              # js: what its `this` is (J2'''): "arrow" (its enclosing function's), "method" (the
                                   # receiver, of its class), "function" (what its calls give)
    strict: bool = False           # js: strict code (a module, a class, "use strict"): a plain call's `this` is undefined
    param_types: list = field(default_factory=list)    # js: each parameter's declared type kept whole (`js_sigs`), ()
                                                        # where none is written
    result_type: tuple = ()        # js: the declared result of a function, the declared type of a field or variable
    tparams: tuple = ()            # js: a function's or an alias's own type parameters: each one's default type
    indexes: list = field(default_factory=list)    # js: an index signature's (key type, value type) on its field
    ambient: bool = False          # js: declared only (a declaration file, `declare`): what it does is its declaration

    def runs(self, which: str, reader) -> int | str | None:
        """The accessor (`get` or `set`) an access of this property written in `reader` runs (G8, G8'): none inside
        its own accessors, where its name is the stored member; None when it has no such accessor."""
        target = self.accessors.get(which)
        if target is None or reader is not None and reader.accessor_of == self.name and reader.owner == self.owner:
            return None
        return target

    @property
    def key(self):
        """What keys its value nodes: its gid (a template of it is a `templates.Frame`, keyed by gid and tuple)."""
        return self.gid

    @property
    def fixed(self):
        """The parameter a template fixes to its key value: none for the unit itself."""
        return None

    @property
    def base(self):
        """The unit a template is of: the unit itself."""
        return self


def declare(r, unit: Unit, name: str, trees: list, param: bool = False, function_scope: bool = False) -> str:
    """A local or parameter declared in `unit`, under the key its block gives it (`scopes`); a name of the
    enclosing function it shadows is no longer captured. Returns the key."""
    key = name if param else scopes.local_key(r, unit, name, function_scope)
    if param and r.scopes:
        r.scopes[-1][name] = name               # the function's own scope (a lambda opens one of its own)
    unit.types[key] = trees
    unit.captured.discard(key)
    return key


def declare_function(r, unit: Unit, name: str, local_id: int, function_scope: bool = False) -> str:
    """A local whose value is a function written in `unit` (a lambda, an arrow, a nested function declaration):
    declared as `declare` does, and known as that function under the key its block gives the name, as every read of
    it is (`scopes`): a local of the same name in another block is another local. Returns the key."""
    key = declare(r, unit, name, [("callable", local_id)], function_scope=function_scope)
    unit.lambdas[key] = local_id
    return key


LOCAL_REFS = ("read", "write", "call", "value", "thread")   # a plain name that can be a local


def add_ref(r, unit: Unit, kind: str, name: str, line: int, cond: tuple, **extra) -> None:
    """A reference made where the reader is: a plain name is the local it names in the open blocks (`scopes`)."""
    if kind in LOCAL_REFS and extra.get("base") is None and not extra.get("qualified"):
        name = scopes.visible(r, name)
    unit.refs.append(Ref(kind, name, line=line, cond=cond, **extra))


def enclose(lam: Unit, owner: Unit) -> None:
    """A lambda written in `owner` sees owner's locals (C++ [expr.prim.lambda.capture]; GDScript: captured by value
    when the lambda is made): their values are owner's, read through `outer`; its own list holds what it writes."""
    lam.outer = owner.local_id
    lam.types = {k: [] for k in owner.types}
    lam.captured = set(owner.types)
    lam.lambdas = dict(owner.lambdas)
    lam.local_types = dict(owner.local_types)


def assign(r, unit: Unit, name: str, value, shared: bool) -> None:
    """A value written to a local (the one its name names where it is written): into the writing unit's own list,
    evaluated there; when the language shares a captured local with the enclosing function (C++ capture by reference,
    JS closures, J7), the enclosing function's local holds what the writer's holds (`written_by`)."""
    units, name = r.facts.units, scopes.visible(r, name)
    if name not in unit.types:
        if shared and unit.lang == "js" and unit.outer >= 0:      # a hoisted closure read before the enclosing
            unit.late_writes.setdefault(name, []).append(value)   # function's later declaration (J7')
        return
    unit.types[name].append(value)
    while shared and name in unit.captured and unit.outer >= 0:
        outer = units[unit.outer]
        if name not in outer.types:
            return
        outer.written_by.setdefault(name, []).append(unit.local_id)
        unit = outer


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
    shape: bool = False            # js: an interface or an object type: no object of its own (`structural`, T1')
    made: int = -1                 # js: an object literal (1) or a class expression (2) where the code makes it: a
                                   # `("cast", "#key")` of it is a value made there, any other cast a declared type
    tparams: tuple = ()            # js: its type parameters: each one's default type (`js_sigs`)
    heritage: list = field(default_factory=list)   # js: what it extends and implements, as declared types (`js_sigs`)


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
    imports: dict = field(default_factory=dict)    # js: local name -> (specifier, imported name | "default" | "*")
    exports: dict = field(default_factory=dict)    # js: exported name -> local name | ("from", specifier, name)
    star_exports: list = field(default_factory=list)   # js: specifiers of `export * from`
    config: dict = field(default_factory=dict)     # tsconfig (paths, baseUrl) / package.json (name, exports, main)
    script: bool = False           # js: no import or export: its top-level names are global (J7)
    errors: int = 0
