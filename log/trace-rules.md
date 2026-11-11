# Trace rules: every construct of GDScript and C++, by the official references

Research for the feature tracer (`tools/analysis/trace_feature.py`), written before any further code change (owner's order,
2026-11-10). Each construct of each grammar has exactly one row: its role in a trace and the rule that handles it. The
implementation follows this table; a construct not in it is a gap in the research, not a case to patch.

Sources (read through context7):
- G1 GDScript reference, Keywords, Operators (precedence table), Variables, Functions ("scope priority for variable
  look-up is: local -> class member -> global"), Inheritance, `is`/`as`, `match`/`when`, lambdas, `await`, annotations
  (`@onready`, `@export`, `@tool`, `@rpc`, `@static_unload`, `@warning_ignore`) — docs.godotengine.org/en/4.7
  gdscript_basics, class_@gdscript, static_typing.
- G2 Variant: an untyped variable holds any value; its type at run time is the type of the value assigned
  (class_variant); `:=` infers from the value (static_typing).
- G3 Global scope: built-in types, engine classes, `@GlobalScope` functions, constants and singletons (OS, Engine,
  RenderingServer, ...), `@GDScript` functions, `class_name` classes, autoloads (class_@globalscope, singletons_autoload).
- G4 Signals and calls by name: Signal.connect/emit, Object.connect, Callable(object, "method"), Callable.bind,
  Object.call/call_deferred/callv (class_signal, class_callable, class_object).
- G5 GDExtension: GDCLASS, GDREGISTER_CLASS, `_bind_methods`, ClassDB::bind_method(D_METHOD), ADD_SIGNAL(MethodInfo),
  ADD_PROPERTY(PropertyInfo, setter, getter), callable_mp, connect(name, Callable), emit_signal (object_class,
  gdextension_cpp_example).
- G6 ClassDB.instantiate(class: StringName): "Instantiates a new object of the given class"; engine and GDExtension
  classes are in ClassDB, script `class_name` classes are not (class_classdb).
- G7 Callable(object, method: StringName) and Object.call(method: StringName, ...) take the method's name as a value; a
  String passed where a StringName is expected is converted (class_callable, class_object, class_stringname): a name
  held in a string value follows FLOW like any value.
- G8 Properties: `var x: set(value): ... get: ...` or `set = f, get = g`; set and get are always called, even inside
  the class (with or without `self.`), except for the variable's initializer and the variable's own name inside its own
  setter or getter, which reach the member directly (gdscript_basics, Properties).
- C1 C++ keywords, identifiers with special meaning, preprocessor tokens (cppreference keyword, preprocessor).
- C2 Expressions: primary expressions, operator precedence, value categories (expressions, operator_precedence,
  value_category, operator_member_access, operator_incdec).
- C3 Name lookup: unqualified (block, then class and bases, then enclosing namespaces, then global), qualified (through
  using-directives), argument-dependent lookup, class member lookup (unqualified_lookup, qualified_lookup, adl).
- C4 Overload resolution; virtual functions and the final overrider; a qualified call is never virtual (virtual).
- C5 `auto` deduced from the initializer; generic lambda parameters from the call's arguments; deduced return type from
  the `return` statements; templates: parameters replaced by the arguments (auto, template_argument_deduction, lambda).
- C7 Implicit calls: declaring an object of class type calls its constructor (`T x;`, `T x(args);`, `T x{args};`), a
  member initializer list writes the members; an automatic object's destructor runs at the end of its scope, `delete`
  runs it; an expression with an overloaded operator calls `operator<op>`; a range-for calls `begin()`/`end()` of the
  range (constructor, default_constructor, destructor, operators, range-for).
- C6 Translation unit: a file plus every file its `#include` brings in (preprocessor/include).

## The general rules (one each; every row below names which applies)

| Id | Rule |
|---|---|
| DEF | A definition puts a name in a scope: a unit the walk can land on (function, method, lambda, variable, field, constant, signal, class, enum, enumerator, parameter, local). |
| LOOK | A name used is resolved by its language's lookup order. GDScript (G1, G3): local -> class member (own, then each base, the engine's base members included) -> outer class -> global (class_name, autoload, @GlobalScope, @GDScript, built-in types, engine classes). C++ (C3, C6): block -> class and bases -> enclosing namespaces -> using-directives -> global, limited to the translation unit; plus argument-dependent lookup in the namespaces of the arguments' classes. A name found nowhere in the program is the engine's or the library's. |
| VALUE | The type of an expression is computed from its construct, recursively over its children (the EXPR rows), never from its text. |
| FLOW | A name with no declared type (GDScript untyped / Variant; C++ `auto`, generic lambda parameter, deduced return) holds the union of the types of every value that flows into it: initializer, each assignment, for a parameter the argument at each call site (callers, emitters of a signal it handles, the engine for engine callbacks and engine signals), for a call the values its `return`s carry, for a container the values put into it, for a loop variable the elements walked (G2, C5). |
| CALL | Calls the language makes without a call written are calls too (C7, G8): a property's set/get on a write/read, an object's constructor at its declaration and its destructor at scope end or `delete`, `operator<op>` for an operator on a class, `begin`/`end` for a range-for. A call resolves its callee (LOOK on the name, VALUE on the receiver), links caller -> callee, binds each argument to its parameter (FLOW), and the callee's result returns to the call site (FLOW). A method call also reaches the overrides in every class below the receiver's (GDScript: every method; C++: `virtual` or overriding, not when qualified) (C4). Overloads are chosen by the arguments (arity first) (C4). |
| BRANCH | Every path of a branch or loop is followed; the condition is carried on everything under it. |
| JUMP | `return` carries a value back to the call site; `break`/`continue`/`goto`/`throw` end a path; `await` resumes when the signal fires or the coroutine returns, with its value. |
| NAME | A name carried as a string reaches what the engine calls or creates by that name: ClassDB.instantiate(&"C") is an object of class C (G6); Callable(obj, "m"), call/call_deferred/callv("m"), connect("s", ...), emit_signal("s"), `&"m"` handed to a call, D_METHOD("m") bindings (G4, G5). The string's values follow FLOW like any value. |
| SIGNAL | `emit` reaches the signal; the signal reaches every handler connected to it in code (connect) or in a scene ([connection]); a signal no code declares is the engine's and the engine emits it (entry) (G4). |
| BIND | GDExtension: bind_method binds a script-visible name to a C++ method; ADD_SIGNAL declares a signal; ADD_PROPERTY binds a property to setter/getter (a read calls the getter, a write the setter); GDCLASS / GDREGISTER_CLASS make the class visible to scripts by its name; callable_mp(obj, &C::m) is a callable of C::m (G5). |
| ENTRY | The engine calls: engine callbacks (`_ready`, `_process`, `_input`, `_notification`, `_init`, ... `_bind_methods`), handlers of engine signals, scene-connected handlers, bound methods called by name, GDExtension init functions, the main loop. These end the walk back. |
| TYPE | A type construct names a type (for DEF of a declared type, for VALUE of casts); no link by itself. |
| NONE | No role in a trace (comments, punctuation-level tokens, editor-only markers). |

## EXPR: the value rule of each expression construct

| Construct | Value (type) |
|---|---|
| identifier | LOOK; a local/parameter/member gives its declared type or FLOW; a function gives a callable of it; a class gives the class (static access); an engine global gives the engine's |
| member access `a.b`, `a->b` | the type of member `b` in the class of `a` (with its bases); `->` first goes through what `a` points at (`operator->` of a program class, the held type of a library pointer/optional/iterator) (C2) |
| method call `a.f(x)` | the return type of `f` in the class of `a`, else FLOW of its returns; on a callable value (`c.call()`): the function it holds |
| call `f(x)` | LOOK `f`: a function's return (declared or FLOW); a class name constructs that class; a callable variable calls what it holds |
| subscript `a[i]` | a program class: its `operator[]` result; a library container: its element (template argument); an untyped container: the values put into it (FLOW) |
| unary / binary operator | a program class operand: its `operator<op>` result (C4); comparisons and logic: bool (engine); arithmetic on built-in or engine types: that type |
| `*p` / `&x` | what `p` points at / a pointer to `x` (a function's address is a callable) |
| ternary / conditional | the union of both branches' values |
| cast (`as T`, `static_cast<T>`, `(T)x`, `T{...}`) | T |
| `is` | bool; inside the branch it guards, the tested name is narrowed to T |
| `new T` / `T.new()` / `make_unique<T>` / `make_shared<T>` | T (or the library pointer to T); reaches T's constructor (`_init`) |
| lambda | a callable of the lambda |
| `await x` | the value the signal carries or the coroutine returns |
| literal (number, string, bool, null, array, dictionary, initializer list) | the engine's / the language's value; an array/dictionary literal holds its elements' values (FLOW) |
| `$Path`, `%Name`, node path | an engine Node (the scene gives its script when the path names a node of the scene that owns the script) |
| `&"name"` (StringName), `"name"` (String) | a value carrying that text; where a method or signal name is expected it names it (NAME, G7) |
| `this` / `self` / `super` | the enclosing class / its base (super: no override dispatch) |
| sizeof, alignof, offsetof, typeid, requires-expression, fold over a pack | the language's value; operands walked for their links |

## GDScript: every construct of the grammar (tree-sitter-gdscript 6.1.0, 85 named kinds)

| Kind | Role | Rule |
|---|---|---|
| source | scope: the script's class | DEF |
| class_name_statement | names the script globally (`class_name`) | DEF, LOOK (global) |
| extends_statement | the base: a global class, a file `"x.gd"`, an inner class `"x.gd".Inner` | DEF (base), LOOK |
| class_definition, class_body | inner class and its members | DEF |
| function_definition, constructor_definition | a method (`func`, `static func`, `_init`) | DEF, ENTRY when an engine callback |
| parameters, typed_parameter, default_parameter, typed_default_parameter, variadic_parameter | parameters with their type or `:=` default | DEF, FLOW |
| body | a block | BRANCH scope |
| variable_statement, onready_variable_statement, export_variable_statement | `var` (with `@onready`: set before `_ready`; `@export`: set by the editor/scene) | DEF, FLOW |
| const_statement | `const` (a `preload` const names a class) | DEF, FLOW |
| static_keyword | `static` member or function | DEF |
| setget, setter, getter, get_body, set_body | a property's accessors: a read runs the getter, a write the setter | DEF, CALL |
| signal_statement | `signal` | DEF, SIGNAL |
| enum_definition, enumerator_list, enumerator | `enum` and its values | DEF |
| lambda | `func(...)` as a value | DEF, VALUE (callable) |
| name | the name a definition introduces | DEF |
| type, inferred_type | a declared type / `:=` | TYPE, FLOW |
| annotation, annotations | `@onready`, `@export`, `@tool`, `@rpc`, `@static_unload`, `@warning_ignore`, `@icon`, `@abstract` | DEF modifiers; `@rpc` makes a method callable by name remotely (NAME) |
| remote_keyword | Godot 3 network keyword kept by the grammar | NAME (as `@rpc`) |
| expression_statement | an expression evaluated for its effects | VALUE |
| assignment, augmented_assignment | `x = v`, `x += v` | FLOW (writes x; a property write runs its setter) |
| if_statement, elif_clause, else_clause | `if` / `elif` / `else` | BRANCH |
| match_statement, match_body, pattern_section, pattern_guard, pattern_binding, pattern_open_ending | `match`, its patterns, `when` guards, `var` bindings (hold the matched value) | BRANCH, FLOW |
| for_statement | `for x in v` (x holds v's elements) | BRANCH, FLOW |
| while_statement | `while` | BRANCH |
| return_statement | `return v` | JUMP, FLOW (result) |
| break_statement, continue_statement, pass_statement, breakpoint_statement | `break`, `continue`, `pass`, `breakpoint` | JUMP / NONE |
| await_expression | `await` | JUMP, EXPR |
| identifier | a name | LOOK, EXPR |
| attribute | member chain `a.b.c` | EXPR (member access) |
| attribute_call | `.f(args)` in a chain | CALL, EXPR |
| attribute_subscript | `.b[i]` in a chain | EXPR (subscript) |
| call | `f(args)` (also `preload`, `load`, `Callable`, `emit_signal`, `connect`, `call*`) | CALL, NAME, EXPR |
| base_call | a call of the base class's method (`super.f()` / `super()`) | CALL (no override dispatch) |
| arguments | call arguments | CALL (each binds a parameter) |
| subscript, subscript_arguments | `a[i]` | EXPR |
| binary_operator | arithmetic, comparison, logic, `in`, `is`, `as` | EXPR |
| unary_operator | `-x`, `not x`, `~x` | EXPR |
| conditional_expression | `a if c else b` | EXPR, BRANCH |
| parenthesized_expression | `( )` | EXPR (the inner value) |
| array, dictionary, pair | literals holding values | EXPR, FLOW (elements) |
| get_node, node_path | `$Path`, `%Name`, `^"path"` | EXPR |
| string_name | `&"name"` | NAME |
| string, integer, float, true, false, null, escape_sequence | literals | EXPR (engine value) |
| line_continuation, comment, region_start, region_end, region_label | layout and editor markers | NONE |

GDScript reserved words (G1): `if elif else match when` BRANCH; `for while in` BRANCH/FLOW; `break continue pass return
await yield breakpoint` JUMP; `class class_name extends signal func static const enum var` DEF; `is as void` TYPE/EXPR;
`self super` EXPR; `preload` CALL (loads a script/class); `assert` EXPR (debug only); `PI TAU INF NAN` literals.

## C++: every construct of the grammar (tree-sitter-cpp 0.23.4, 206 named kinds)

| Kind | Role | Rule |
|---|---|---|
| translation_unit | a file; with its includes, the translation unit | DEF, LOOK (C6) |
| preproc_include, system_lib_string | `#include "x"` / `<x>` | LOOK (translation unit) |
| preproc_def, preproc_function_def, preproc_params, preproc_arg, preproc_call, preproc_directive | `#define` macros and other directives | DEF (macro); a macro use is a reference to it |
| preproc_if, preproc_ifdef, preproc_elif, preproc_elifdef, preproc_else, preproc_defined | conditional inclusion | BRANCH (both sides read) |
| namespace_definition, namespace_identifier, nested_namespace_specifier, namespace_alias_definition | `namespace` (anonymous: internal linkage) | DEF, LOOK |
| linkage_specification | `extern "C" { }` | DEF scope; ENTRY for exported init functions |
| declaration_list, field_declaration_list, compound_statement | scopes | DEF scope / BRANCH scope |
| class_specifier, struct_specifier, union_specifier, base_class_clause, access_specifier | classes and their bases | DEF, LOOK (bases), CALL (overrides) |
| enum_specifier, enumerator_list, enumerator | `enum` / `enum class` (unscoped values visible in the enclosing scope) | DEF |
| function_definition, function_declarator, trailing_return_type, field_initializer_list, field_initializer | functions, constructors' member initializers | DEF, FLOW (initializers write members) |
| declaration, field_declaration, init_declarator | variables and fields (in a block, `T x(args)` declares an object) | DEF, FLOW |
| parameter_list, parameter_declaration, optional_parameter_declaration, variadic_parameter_declaration | parameters (with defaults) | DEF, FLOW |
| pointer_declarator, reference_declarator, array_declarator, parenthesized_declarator, attributed_declarator, structured_binding_declarator, variadic_declarator, new_declarator, bitfield_clause | declarator forms (`auto [a, b]` binds the parts of the value) | DEF, FLOW |
| abstract_array_declarator, abstract_function_declarator, abstract_parenthesized_declarator, abstract_pointer_declarator, abstract_reference_declarator, pointer_type_declarator | unnamed declarators inside types | TYPE |
| default_method_clause, delete_method_clause, pure_virtual_clause, virtual_specifier, explicit_function_specifier | `= default`, `= delete`, `= 0`, `override`/`final`, `explicit` | DEF (pure virtual: dispatch reaches the overriders) |
| storage_class_specifier, type_qualifier, ref_qualifier, noexcept, throw_specifier, alignas_qualifier | `static`, `extern`, `inline`, `const`, `&`, `noexcept` ... (`static` free function: internal linkage) | DEF modifiers |
| template_declaration, template_parameter_list, type_parameter_declaration, optional_type_parameter_declaration, variadic_type_parameter_declaration, template_template_parameter_declaration, template_instantiation | templates (parameters replaced by the arguments written) | DEF, VALUE (C5) |
| concept_definition, requires_clause, requires_expression, requirement_seq, simple_requirement, compound_requirement, type_requirement, constraint_conjunction, constraint_disjunction | concepts and constraints | TYPE (operands walked) |
| alias_declaration, type_definition | `using X = Y`, `typedef` | DEF (type alias), LOOK |
| using_declaration | `using namespace N` / `using N::x` | LOOK |
| friend_declaration, static_assert_declaration, attribute_declaration | `friend`, `static_assert`, `[[...]];` | NONE (operands walked) |
| operator_name, destructor_name, operator_cast | `operator+`, `~T`, `operator T` | DEF (called by the operator / destruction / conversion) |
| primitive_type, sized_type_specifier, type_identifier, template_type, template_argument_list, type_descriptor, dependent_type, dependent_name, placeholder_type_specifier, auto, decltype | types (`auto` / `decltype(auto)`: FLOW) | TYPE, FLOW |
| attribute, attribute_specifier, ms_based_modifier, ms_call_modifier, ms_declspec_modifier, ms_pointer_modifier, ms_restrict_modifier, ms_signed_ptr_modifier, ms_unaligned_ptr_modifier, ms_unsigned_ptr_modifier | attributes and compiler modifiers | NONE |
| expression_statement | an expression for its effects | VALUE |
| if_statement, condition_clause, else_clause, init_statement | `if` (with init), `else` | BRANCH |
| switch_statement, case_statement | `switch` / `case` / `default` | BRANCH |
| for_statement, for_range_loop | `for`, range-for (the variable holds the range's elements) | BRANCH, FLOW |
| while_statement, do_statement | `while`, `do` | BRANCH |
| return_statement, co_return_statement | `return`, `co_return` | JUMP, FLOW |
| break_statement, continue_statement, goto_statement, labeled_statement, statement_identifier | jumps and labels | JUMP |
| try_statement, catch_clause, throw_statement, seh_try_statement, seh_except_clause, seh_finally_clause, seh_leave_statement | exceptions (a `throw` reaches the matching `catch`) | BRANCH, JUMP |
| co_yield_statement, co_await_expression | coroutines | JUMP, EXPR |
| attributed_statement | a statement with attributes (`[[likely]] if ...`) | VALUE (the inner statement by its own row; the attributes NONE) |
| identifier, field_identifier, qualified_identifier | names (`A::b` qualified) | LOOK, EXPR |
| this | the enclosing object | EXPR |
| call_expression, argument_list | calls | CALL, EXPR |
| template_function, template_method | `f<T>(...)`, `obj.template f<T>()` (casts, `make_*`, `get<T>`: EXPR) | CALL, EXPR |
| field_expression | `a.b`, `a->b` | EXPR (member access) |
| subscript_expression, subscript_argument_list | `a[i]` | EXPR |
| pointer_expression | `*p`, `&x` (`&C::m` / `&f`: a callable) | EXPR |
| unary_expression, binary_expression, update_expression, assignment_expression, comma_expression | operators (assignment: FLOW into the left side) | EXPR, FLOW |
| conditional_expression | `c ? a : b` | EXPR, BRANCH |
| cast_expression, compound_literal_expression | `(T)x`, `T{...}` | EXPR |
| initializer_list, initializer_pair, field_designator, subscript_designator, subscript_range_designator | brace initialization (each value flows into its element/member) | EXPR, FLOW |
| lambda_expression, lambda_capture_specifier, lambda_default_capture, lambda_capture_initializer | lambdas and captures | DEF, EXPR (callable) |
| new_expression, delete_expression | `new T(...)` (constructs, reaches the constructor), `delete p` (reaches the destructor) | CALL, EXPR |
| sizeof_expression, alignof_expression, offsetof_expression, generic_expression, extension_expression, fold_expression, parameter_pack_expansion | language operators on types and packs | EXPR (operands walked) |
| parenthesized_expression | `( )` | EXPR |
| gnu_asm_expression, gnu_asm_qualifier, gnu_asm_output_operand_list, gnu_asm_output_operand, gnu_asm_input_operand_list, gnu_asm_input_operand, gnu_asm_clobber_list, gnu_asm_goto_list | inline assembly | NONE (operands walked) |
| number_literal, char_literal, character, string_literal, raw_string_literal, concatenated_string, string_content, raw_string_content, raw_string_delimiter, escape_sequence, user_defined_literal, literal_suffix, true, false, null | literals (a user-defined literal calls its `operator""`) | EXPR |
| comment | comment | NONE |

C++ reserved words (C1): `class struct union enum namespace typedef template concept friend operator static extern inline
virtual explicit mutable thread_local constexpr consteval constinit register new delete` DEF; `using export import module`
LOOK; `if else switch case default try catch requires` BRANCH; `for while do` BRANCH; `return break continue goto throw
co_return co_yield co_await` JUMP; `this` EXPR; `public protected private` DEF (access); `auto decltype typename typeid sizeof
alignof alignas` and the built-in type words TYPE; casts (`static_cast dynamic_cast const_cast reinterpret_cast`) EXPR;
`true false nullptr` literals; `override final` DEF (virtual). Preprocessor: `include embed` LOOK, `define undef` DEF, `if
ifdef ifndef elif elifdef elifndef else endif` BRANCH, `line error warning pragma` NONE.

## What the implementation must change (derived from this table; to be planned, not yet coded)

1. VALUE is computed over the syntax tree, one function per EXPR row; no expression is ever turned into text and parsed
   again (text markers broke on every operator, call and literal inside a chain).
2. FLOW is one owner: every DEF of a name without a declared type collects its flows (initializer, assignments,
   arguments at call sites, returns, container puts, loop elements) as references to expression nodes, evaluated by VALUE.
3. LOOK follows the documented orders above exactly, including inherited engine members (GDScript) and argument-dependent
   lookup (C++); "found nowhere in the program" is the engine's, by the definition of the global scope (G3, C6).
4. Coverage is enforced: a test lists every named kind of both grammars and fails when a kind has no row here, and when a
   row's rule has no handler in the code.

## Solving the flows at scale (research, 2026-11-10)
The value flows (FLOW) form an inclusion-based (Andersen-style) analysis. The demand-driven recursive evaluation with
re-evaluation at cycles does not scale: on 25.7k definitions its second link pass took 346 s and the third did not end.
The standard solver, from the literature:
- S1 constraint graph: one node per place that holds values (local, parameter, field, return, container element), one
  subset edge per flow (assignment, argument -> parameter, return -> call result, put -> element); a node's set is the
  union of its predecessors' (Pearce, Kelly, Hankin, SCAM 2003; the worklist algorithm).
- S2 worklist with difference propagation: a node whose set changed pushes only the new values to its successors,
  until no set changes (Pearce et al.).
- S3 complex constraints: an edge that depends on values (`x.f()`: which f depends on x's classes; a parameter's
  callers) is added when the values arrive, and its endpoints are queued (Andersen; Hardekopf and Lin).
- S4 cycles: the nodes of a cycle of subset edges end with the same set; online cycle detection collapses them into one
  node (Hardekopf and Lin, PLDI 2007, lazy and hybrid cycle detection; Pereira and Berlin, CGO 2009, wave propagation in
  topological order).
Sources: whileydave.com/publications/PKH03_SCAM_preprint.pdf; cs.utexas.edu/~lin/cs380c/handout14.pdf;
cse.iitm.ac.in/~rupesh/teaching/pa/jan17/scribes/6.pdf; arxiv.org/pdf/2602.01720 (Phoenix); PUS, ICSE 2022.

## The solver chosen (research, 2026-11-10)
- S5 Local top-down solving with dynamic dependencies (TD and RLD, Seidl et al.): only the unknowns a query needs are
  evaluated; dependencies are found by observing what each evaluation reads; when an unknown's value changes, the
  unknowns that read it are evaluated again (RLD: put on a worklist). Side-effecting systems: an evaluation may also
  contribute to other unknowns (here: a reference's evaluation adds links into a unit; the links into a unit are an
  unknown its readers depend on). Correctness and termination are proved for monotone right-hand sides over finite
  domains (Isabelle AFP "Top_Down_Solver"; "Three improvements to the top-down solver", MSCS 2021; arXiv 2209.10445).
- S6 Call-graph precision ladder: CHA (every override below the static class) over-approximates RTA, which
  over-approximates VTA and points-to; the tracer resolves a receiver by the values that flow into it (points-to
  style) and adds overrides below each class found (CHA from that class), so it is at least as precise as CHA and
  never misses an override of a class a value has (arXiv 1907.06522; Dean et al. 1995; Bacon and Sweeney 1996;
  Sundaresan et al. 2000).
Implementation: `typer.Typer` (unknowns = (expression tree, unit); `read`, `grew`, worklist), `resolve.Resolver.incoming`
and `keep_edge` (the side-effected unknown "links into a unit"), `link.build` (references as unknowns, one worklist).
- S7 What was implemented of S1-S5 (2026-11-10): the worklist solver with recorded dependencies (S5), difference
  propagation along the links (S2: each argument of each link is one node carried once to its parameter), and one
  identity per value (a type naming a class of the program is that class in whatever unit it is read: without it
  the same class counted once per reading unit and the sets grew with the program). On 1132 files / 25770
  definitions the fixpoint takes 16 s (191 s before the identity; 118 s with whole-program passes). S4 (collapsing
  cycles) is not needed at this cost and is not implemented.

## JavaScript and TypeScript: sources (read through context7, 2026-11-10)

- J1 ECMAScript functions and parameters: default values, rest parameters, `arguments`; destructuring assignment and
  binding patterns (MDN Functions, Default parameters, Rest parameters, Destructuring).
- J2 `this`: in a method, the object the method was called on; in a class method, the instance; arrow functions take
  `this` from the enclosing lexical context and ignore `call`/`apply`/`bind`'s thisArg; `call(thisArg, ...args)`,
  `apply(thisArg, args)`, `bind(thisArg, ...args)` call or bind the function (MDN this, Arrow functions,
  Function.prototype.call/apply/bind).
- J3 Modules: `import` forms (named, default, namespace `* as`, side-effect), `export` forms (named, default, `export
  { a as b } from`, `export * from`), dynamic `import()`, `import.meta`; a module's top-level code runs once when it is
  first imported (MDN import, export, Modules guide). Module resolution: relative specifiers with extension
  substitution (`./a.js` -> `a.ts`), extensionless paths and directory `index` files under `bundler`/`node10`,
  `paths`/`baseUrl`, `node_modules` packages with package.json `exports`/`module`/`main`/`types` (TypeScript handbook,
  Modules reference: moduleResolution node16/nodenext/bundler).
- J4 Classes: `constructor`, `extends` and `super(...)` (a derived class without a constructor gets the default one,
  which calls the base's), `super.m()`, static members and `static {}` blocks (run when the class is evaluated),
  public fields (initialized in the constructor; `this` the instance), private `#` members, getters and setters (MDN
  Classes, constructor, extends, static, Public class fields, Private elements).
- J5 Calls the language makes: a getter on a read, a setter on a write; `for...of`, spread and array destructuring
  call `[Symbol.iterator]()` and `next()`; `+`, comparisons and template literals call `valueOf`/`toString`/
  `[Symbol.toPrimitive]`; a tagged template calls its tag; `yield` resumes on `next()` (MDN Symbol.iterator,
  Object.prototype.valueOf, Addition, Template literals).
- J6 The host's callbacks: EventTarget `addEventListener(type, listener)` (a function or an object with
  `handleEvent`), `on<type>` handler properties, `dispatchEvent(event)`, `setTimeout`/`setInterval`,
  `requestAnimationFrame`, `queueMicrotask`, Promise `then`/`catch`/`finally` and `await` (a thenable's `then`),
  Worker `postMessage` delivering a `message` event to the other side (MDN addEventListener, Microtask guide, Using
  promises, Worker message event, postMessage).
- J7 Scope: `var` is function-scoped and hoisted, `let`/`const` block-scoped (temporal dead zone), function
  declarations hoisted whole; closures see the enclosing functions' names; a script (no import/export) puts its
  top-level names in the global scope (MDN var, let, Grammar and types, Closures; TypeScript Modules theory).
- J8 TypeScript: contextual keywords (`abstract`, `declare`, `enum`, `namespace`, `module`, `readonly`, `type`,
  `keyof`, `infer`, `satisfies`, `as`, `is`, `asserts`, `override`, `accessor`, `implements`, `interface`);
  interfaces and namespaces merge with declarations of the same name; `declare global` and `declare module "m"`
  augment the global scope and a module; types are erased: `as`, `satisfies`, `!` keep the value (TypeScript
  handbook, release notes 1.8, 2.0, 4.3, 4.9, 6.0; Declaration merging).
- J9 Object literals: method shorthand, getters and setters, `this` in a method is the object; an anonymous function
  takes the name of the variable or property it is assigned to (MDN Object initializer, Working with objects,
  Function.name).
- J10 JSX: a lowercase tag is intrinsic (the host's); a capitalized tag is a value in scope, a function component
  `(props) => Element` or a class component; `react-jsx` emits `_jsx(Component, props)` (TypeScript handbook 4.6,
  5.1 JSX.ElementType).
- J11 three.js (r160-r186 docs): `EventDispatcher.addEventListener/dispatchEvent/removeEventListener`; Object3D events
  `added`, `removed`, `childadded`, `childremoved`, `dispose`; `Object3D.onBeforeRender/onAfterRender` callbacks,
  `traverse(callback)`; `WebGLRenderer.setAnimationLoop(callback)`; `Material.onBeforeCompile`,
  `customProgramCacheKey`; `Loader.load(url, onLoad, onProgress, onError)`; TSL `Fn()` and node update callbacks
  (`onFrameUpdate`, `onRenderUpdate`, `onObjectUpdate`); `dispose()` dispatches `dispose`. All of them are the general
  rules J6 (a callable handed to, or written into, an object of a library) and SIGNAL by event type: nothing is
  written for three.js by name. When the library's own source is among the folders read, it is code like any other
  and its calls are followed through it.
- J12 Grammar: tree-sitter-typescript 0.23.2, `typescript` for `.ts`/`.mts`/`.cts`, `tsx` for `.tsx` and for
  JavaScript (`.js`, `.jsx`, `.mjs`, `.cjs`: TypeScript's grammar extends JavaScript's; the TSX variant reads JSX).

## JavaScript and TypeScript: the general rules applied

| Id | Rule |
|---|---|
| LOOK | A bare name: the block and function scopes outward (closures, J7), then the module's top-level names, then its imports (each followed to the exporting module through `export ... from` and `export *`), then the global scope (scripts, `declare global`), else the host's or a library's. Never a class member without `this.` (J4). A member: on the classes every value of the receiver can be, then up `extends` (the prototype chain); an interface or abstract declaration reaches the members of every class that implements or extends it. A type name in an annotation is looked up the same way. |
| FLOW | As for the other languages: a declared type names the class; every value written to a name flows into it; arguments to parameters (rest parameters take the elements); destructuring binds parts; a callable value stays a callable (FN). `as`/`satisfies`/`!` keep the value. |
| CALL | A call reaches the functions its callee can hold and the overrides below (every JS method is virtual). `new C` reaches C's `constructor` or the inherited one, and the field initializers. Getters, setters, `[Symbol.iterator]`, `valueOf`/`toString`, `then` on `await`, decorators, tagged templates and JSX components are calls (J5, J8, J10). `f.call/apply` call f; `f.bind` is f. |
| ENTRY | A module's top-level code (runs on import; an HTML entry or a bundler entry starts it); a static block; a callable handed to the host or a library (event listeners, timers, animation frames, promise reactions, library callbacks, J6, J11); handlers of events by type (SIGNAL). |
| SIGNAL | An event by its type: `addEventListener("t", h)`, `on("t", h)`, `on<t> = h` subscribe h; `dispatchEvent({type: "t"})`, `dispatchEvent(new Event("t"))`, `emit("t", ...)`, `postMessage(data)` ("message") fire it; with no class of the program defining these methods, the event is the host's or the library's and links by its type. |

## JavaScript and TypeScript: every construct of the grammar (tree-sitter-typescript 0.23.2, TSX, 184 named kinds)

| Kind | Role | Rule |
|---|---|---|
| program | a module: its top-level code runs once when the module is first imported (J3) | DEF, ENTRY |
| class_declaration, abstract_class_declaration, class, class_body | a class: its key, its members; `abstract` classes the same (J4) | DEF |
| class_heritage, extends_clause, implements_clause | `extends X` / `implements I`: the bases, looked up as values (J4, J8) | DEF, LOOK |
| class_static_block | `static { }`: runs when the class is evaluated, with the module (J4) | DEF, ENTRY |
| function_declaration, generator_function_declaration, function_expression, generator_function, arrow_function | a function: declaration, expression, generator, arrow (an arrow keeps the enclosing `this`, J2); named after the variable or property it is written to (J9) | DEF, FLOW |
| method_definition | a method, getter or setter (a getter runs on every read, a setter on every write, J5) | DEF, CALL |
| abstract_method_signature, method_signature, function_signature, call_signature, construct_signature, index_signature, property_signature | a member or signature declared without a body (TS interfaces, `abstract`, overloads, `declare`): a declaration the calls through the type reach, overridden by the classes below (J8) | DEF, TYPE |
| public_field_definition | a field: its initializer runs in the constructor (instance) or with the class (static) (J4) | DEF, FLOW |
| interface_declaration, interface_body, enum_declaration, enum_body, enum_assignment, internal_module, module, type_alias_declaration, ambient_declaration | TS interface, enum, namespace, type alias, ambient `declare` and `declare global`/`declare module` (merged with the declarations of the same name, J8) | DEF, TYPE |
| lexical_declaration, variable_declaration, variable_declarator, formal_parameters, required_parameter, optional_parameter | a variable (`var` function-scoped and hoisted, `let`/`const`/`using` block-scoped, J7), its initializer's value; a parameter, with its default value and the arguments of every call (J1) | DEF, FLOW |
| object_pattern, array_pattern, pair_pattern, object_assignment_pattern, assignment_pattern, rest_pattern, shorthand_property_identifier_pattern | destructuring: each name bound to its part of the value (a property, an element, the rest) (J1) | FLOW |
| assignment_expression, augmented_assignment_expression | an assignment: the value flows into the name or member written; a member with a setter calls it; a callable written to a member of a value the program does not define is handed to that library (J6) | FLOW, CALL |
| import_statement, import_clause, named_imports, namespace_import, import_specifier, import_require_clause, import_alias, import_attribute | `import`: a binding of a name exported by another module, resolved by the module resolution rules (relative path with extension substitution, index files, tsconfig paths, package.json exports, J3) | LOOK, FLOW |
| export_statement, export_clause, export_specifier, namespace_export | `export`: a name of the module made visible, renamed, defaulted or re-exported from another module (`export * from`, `export { a as b } from`) (J3) | LOOK, DEF |
| identifier, statement_identifier, this, super, nested_identifier | a name: block -> enclosing functions (closures) -> module -> imports -> global (J7); `this` is the class instance in a method and the enclosing `this` in an arrow (J2); `super` the base class | LOOK |
| member_expression, subscript_expression, optional_chain, property_identifier, private_property_identifier, shorthand_property_identifier, computed_property_name | a member `a.b`, `a?.b`, `a[k]`, `a.#p`: looked up on the classes of every value of `a` (prototype chain = bases); with a getter it is a call (J5); a computed key holding text values names the members those texts are | LOOK, CALL |
| call_expression, arguments | a call: callee resolved (a name, a member, any expression holding a function), arguments to parameters, the result back; `f.call(t, ...)`/`f.apply(t, a)`/`f.bind(t)` call f (J2); a callable handed to a function the program does not define runs there (host and library callbacks, J6); a tagged template calls its tag (J5) | CALL, FLOW |
| new_expression | `new C(args)`: C's constructor (inherited when C has none: the default constructor calls the base's, J4) and its instance field initializers | CALL, FLOW |
| decorator | `@dec`: the decorator is called with the class or member when the class is evaluated (J8) | CALL |
| jsx_element, jsx_opening_element, jsx_closing_element, jsx_self_closing_element, jsx_attribute, jsx_expression, jsx_namespace_name | JSX: a lowercase tag is the host's; a capitalized tag is a value in scope (a function or class component) handed to the JSX runtime, which calls it with the props (J10) | CALL, VALUE |
| binary_expression, unary_expression, update_expression, template_substitution, template_string | operators: a value of a class used by `+`, comparisons, a template `${x}` calls its `valueOf`/`toString`/`[Symbol.toPrimitive]` (J5); `in`, `instanceof`, `typeof`, `delete`, `void` give the language's values | EXPR, CALL |
| ternary_expression, sequence_expression, parenthesized_expression, as_expression, satisfies_expression, non_null_expression, instantiation_expression, spread_element | a value built from its parts: the ternary is either branch; a sequence its last; parentheses the inner; `as`/`satisfies`/`!`/`<T>` keep the value (type-only) with the type named (J8) | EXPR |
| await_expression, yield_expression | `await x`: the value x resolves to (a thenable's `then` is called, J6); `yield`: a value out of a generator, resumed by `next()` (J5) | EXPR, JUMP |
| object, pair, array | a container literal: an object literal is a class of its own (its methods, getters and fields; `this` in its methods is the object, J9); an array holds its elements | EXPR |
| string, string_fragment, escape_sequence, number, regex, regex_pattern, regex_flags, true, false, null, undefined, html_character_reference, jsx_text, meta_property | a literal: the language's value | VALUE |
| import | `import("m")`: the module, loaded later (a promise of its namespace); `import.meta` (J3) | LOOK, CALL |
| if_statement, else_clause, switch_statement, switch_body, switch_case, switch_default, try_statement, catch_clause, finally_clause, labeled_statement, with_statement, statement_block | every path is followed with its condition: `if`/`else`, `switch`, `try`/`catch`/`finally`, labels, `with` (its object's members are names in scope) | BRANCH |
| for_statement, for_in_statement, while_statement, do_statement | loops: `for`, `while`, `do`; `for...of` calls the iterable's `[Symbol.iterator]()` and the iterator's `next()`, `for...in` walks the keys (J5) | BRANCH, CALL |
| return_statement, throw_statement, break_statement, continue_statement | `return` carries a value back; `throw`, `break`, `continue` end a path | JUMP |
| expression_statement | a statement made of an expression | VALUE |
| type_annotation, type_arguments, type_parameters, type_parameter, type_identifier, nested_type_identifier, generic_type, predefined_type, array_type, tuple_type, union_type, intersection_type, function_type, constructor_type, object_type, parenthesized_type, literal_type, lookup_type, index_type_query, type_query, conditional_type, infer_type, mapped_type_clause, template_type, template_literal_type, readonly_type, rest_type, optional_type, this_type, existential_type, flow_maybe_type, default_type, constraint, type_predicate, type_predicate_annotation, asserts, asserts_annotation, adding_type_annotation, omitting_type_annotation, opting_type_annotation, extends_type_clause | a TS type: names the type a declared name holds (for VALUE of a declared name, and the receiver's class of a call through it); no link by itself (J8) | TYPE |
| comment, html_comment, hash_bang_line, debugger_statement, empty_statement, accessibility_modifier, override_modifier | no role in a trace: comments, `debugger`, `;`, modifiers checked only by the compiler | NONE |

## Difference propagation on every value (research, 2026-11-10 22:30)

Sources: D. J. Pearce, P. H. J. Kelly, C. Hankin, "Online Cycle Detection and Difference Propagation for Pointer
Analysis", SCAM 2003 (whileydave.com/publications/pkh03_scam): propagate only the change of a variable's solution, not
the whole solution; B. Hardekopf, C. Lin, PLDI 2007 (cycle detection, the same worklist); F. Pereira, D. Berlin, "Wave
Propagation and Deep Propagation for Pointer Analysis", CGO 2009; semi-naive evaluation of Datalog (each iteration joins
only the facts new since the last one; a rule with several recursive atoms takes the delta at each position in turn;
strata solved in order) (arXiv 1812.03975, 2108.11212).

The rule taken: a value node is a set that only grows. Its contributions are subscriptions: a source node, and a transfer
applied to each value of the source. When a source gains new values, only those go through each of its subscriptions; a
value already in the target is dropped (the set's identity). This is exact when the transfer is distributive over the
source's values, f(A u B) = f(A) u f(B): then the union of the transfers of the deltas is the transfer of the union, and
the least fixpoint is the same as recomputing everything. The links (references) and the two shapes below that are not
distributive stay consumers computed again in full, but only after every pending delta has propagated (a stratum).

| IR shape | Distributive | Delta rule |
|---|---|---|
| name (local, parameter, capture, global, member of the class) | yes: a union of sources | the sources' new values pass unchanged |
| member(base, s) | yes over the base's values | each new base value: its member's held values or returns (subscriptions), template parameters replaced |
| call(callee, args) | yes over the callee's values | each new callable: its returns (subscription); anything else: the engine's value |
| call by a constructor's name, std::move/forward | yes | the class (constant); the argument's values unchanged |
| call of Callable(obj, name) / ClassDB.instantiate(name) | no: pairs of two growing sets | a consumer computed again in full |
| index, elements, deref | yes over the base | each new value: the element, or the operator's returns |
| op, unary | yes over the operand | each new value: the operator's returns, or the value |
| union, await, addr | yes | both sides / the inner value unchanged |
| collection, put | yes | each new value as an element |
| cast, new, callable, self, super, strname, module, literal | constant | added once |
| parameter (place) | yes: a union of the arguments of every link into the function | each link subscribes its argument tree |
| variable or field (place) | yes: a union of every value written | each write subscribes its value tree |
| signal handler parameter | yes | subscribes the signal's carried values |

## The pending rules found against a real run (research, 2026-11-10 23:20)

Sources: cppreference "Scope" ([basic.scope.block]: a name declared in a block is local to it, from its point of
declaration to the end of the block; the same name in another block is another entity), "Local classes"
([class.local]: a class declared in a function body is local to it, its name visible in that scope), "Most vexing
parse" / [dcl.ambig.res] with name lookup (in a block, `T x(a, b);` declares an object initialized with the values `a`,
`b` when they name values, not types), "Destructors" ([class.dtor]: after the body, the destructors of the non-static
data members in reverse order, then of the direct bases; std::unique_ptr's destructor deletes what it owns, a container
destroys its elements, [unique.ptr.single.dtor], [container.reqmts]); MDN `let`/`const` (block scope), `var` (function
scope); GDScript reference (a local is visible in the block it is declared in); semi-naive evaluation (each relation's
delta at each position in turn, the others whole).

| Rule | Where | What the tracer does |
|---|---|---|
| Block scope | C++, GDScript, JS `let`/`const`/`class`/`catch`/loop variables | a declaration whose name is already a local of the function in another block is another local (its own key); uses inside its block read it |
| Function scope | JS `var`, parameters | one local for the whole function |
| Local class | C++ | read as a class keyed in its function; its name names it inside the function and its lambdas |
| Direct-initialization in a block | C++ | each "parameter" that is a bare name is a value passed to the constructor |
| Implicit destruction | C++ | a destructor (written or implicit) runs the destructors of the members held by value, of what a library owner holds (smart pointers, containers, optional, pair, tuple, variant), and of the bases |
| Relinking a reference | all | when what it reads grew, only the difference is linked: each grown node's new values in its place, in turn, the others whole; a decision that depends on the whole set (anything reached, an overload that fits) uses what the reference already found |
| Overrides of a call | all | every override below each target is linked as such, whatever else the same call reached (no dependence on the order of relinks) |

## Collapsing cycles of identical values (research, 2026-11-11 02:00)

Sources: D. J. Pearce, P. H. J. Kelly, C. Hankin, "Online Cycle Detection and Difference Propagation for Pointer
Analysis" (SCAM 2003; TOPLAS 2007): nodes on a cycle of inclusion edges always hold the same set, so a cycle can be
collapsed into one node, found online as edges are added; B. Hardekopf, C. Lin, "The Ant and the Grasshopper: Fast and
Accurate Pointer Analysis for Millions of Lines of Code" (PLDI 2007): Lazy Cycle Detection — when a value set is
propagated along an edge a -> b and the two sets come out equal, look for a cycle through that edge (once per edge) and
collapse it; F. Pereira, D. Berlin (CGO 2009): wave propagation over the collapsed graph.

| Rule | What the tracer does |
|---|---|
| Which edges form cycles | only subscriptions that pass values unchanged (`same`: an argument to a parameter, a local's value, a return); a transfer that changes values (member, call, step, operator, element) never joins a cycle |
| When to look | after values pass along a `same` edge a -> b and a and b hold the same set, the first time for that edge |
| What is collapsed | the strongly connected component of `same` edges through a and b: one representative holds the union of the values and every subscription of the members (union-find on node keys) |
| What a reader sees | a node's values are its representative's; consumers that read a merged node are relinked over its values |

### When the cycles are looked for (research continued, 2026-11-11 02:40)

Read further before planning: Hardekopf and Lin's slides (PLDI 2007) state the trade-off of every online detector: too
eager costs graph walks, too lazy finds cycles late. Pearce, Kelly and Hankin keep two forms: SCAM 2003 keeps a
topological order of the graph and searches only from an edge that breaks it; PASTE 2004 / TOPLAS 2007 collapse every
cycle with Tarjan's algorithm over the whole graph once per pass of the solver (Tarjan gives the order for free).
Sources: https://www.doc.ic.ac.uk/~phjk/Publications/OnlineCycleDetectionSCAM2003.pdf,
https://www.doc.ic.ac.uk/~phjk/Publications/EfficientFieldSensitivePointerAnalysisforCPASTE04.pdf,
https://www.cs.utexas.edu/~lin/papers/pldi07.pdf, https://www.slideserve.com/jalen/the-ant-and-the-grasshopper-fast-and-accurate-pointer-analysis-for-millions-of-lines-of-code.

What this tracer's graph does to each form, read from its code:
- Lazy detection (the row "When to look" above) fires on every `same` edge out of a node with one value (a local
  holding the one class its constructor gives): such chains are most of the graph, and each firing walks everything
  downstream. Too eager here.
- A kept topological order: nodes are wired on demand, a target before its sources (`Typer.node_of` wires the node, then
  the trees it follows), so most new edges break the order and the region searched is every node added since.
- A whole sweep with Tarjan: linear in the `same` edges, finds every cycle present, and needs no trigger per edge. The
  solver here is a worklist, not passes, so a sweep runs when the `same` edges have doubled since the last one (the first
  when the drain starts, after every reference was linked once): the sweeps together cost at most twice the final graph.

Decision (replaces the row "When to look" above, kept as it was written): Tarjan's sweep over the `same` edges, run when
they have doubled; each strongly connected component of two or more nodes becomes one node. A merge is exact: each
member's subscriptions get only the values that member lacked, each difference still waiting for a member goes to that
member's subscriptions, and a consumer that read a member reads the merged node from its start.

### What the lab run showed (2026-11-11 04:30)

Measured once at the end, base `6aaab88f` against the merge, on the lab (145 626 definitions): the same 785 524 links,
the drain after the first linking 196 s -> 180 s, the whole run 383.6 s -> 382.7 s. Two findings, both read in the code:
- A call site, not a line, carries arguments. `Resolver.keep_edge` keeps one link per (caller, callee, kind, line,
  file) and subscribes the arguments of that first one only: `place(new Left()) + place(new Right())` on one line gave
  `place`'s parameter one of the two, whichever was linked first (the two runs kept a different one of the lab's
  `doc.addNode({...})` pair on one line of `groundRoads.ts`). Rule: the report keeps one link per line; the values
  flow from every call site, each set of argument trees once.
- A reader moved by a merge read the merged node from its start (the plan's rule), 17 318 more relinks: what the drain
  saved, the relinking spent. Rule: a reader moved from a member gets exactly the values of the union it had not read
  (those of the member past its mark, and the others' values), then the tail as before.

### The lab with three.js after the call-site rule (2026-11-11 05:10)

With every call site carrying its arguments, the lab with three.js (`src`, `tsconfig.json`, three's `package.json`,
`build`, `examples/jsm`) is no longer the run measured above: 273 408 links after the first linking (273 231 before),
and at 827 s 2 009 318 links with 169 225 consumers still waiting (785 524 at the end before), stopped at the deadline.
The earlier numbers were those of a flow that lost every call after the first on a line: three's shading language is
written that way (`add( mul( a, b ), mul( c, d ) )`), and its helpers' parameters now hold what every call gives them.
The size of that context-insensitive flow, not cycles, is what the lab costs: the next item (`log/pendencias.md`).

## Why the lab with three.js grows (cause read in the code, 2026-11-11 06:00)

three's shading language (TSL) makes its functions with factories, and the tracer's values cannot tell the products of
one factory apart:
- `ShaderNodeProxy(NodeClass, scope, factor, settings)` (three.webgpu.js 4290-4389) returns one of three arrow functions
  (4344, 4354, 4362) capturing `NodeClass`; 164 functions (`add`, `mul`, ...) are made by `nodeProxy`/`nodeProxyIntent`.
  A closure is valued by its body (`FN + gid`) and a captured name is read from the enclosing function's one local node,
  so the 164 are three functions whose `NodeClass` is every node class given to any of them, whose `params` are what
  every call passes: every call builds every node class with every argument.
- `Fn(jsFunc)` (5066) keeps `jsFunc` in `new FnNode` -> `new ShaderNode` -> `ShaderNodeInternal.jsFunc` (4762), called at
  4544. An object is valued by its class (`CLASS + key`), so that field holds all 221 shader bodies and each such call
  reaches all of them. `ConvertType(type)` (4839) is the same shape for `float`, `vec3`, `color`, ...
- The call-site rule fed these hubs what was lost before; links grow as calls x classes. Both builds of the node system
  (`three.webgpu.js`, `three.webgpu.nodes.js`) are roots and solved whole, as is every module of `examples/jsm`.

Research: Li, Tan, Møller, Smaragdakis, "Precision-Guided Context Sensitivity for Pointer Analysis" (OOPSLA 2018,
https://cs.au.dk/~amoeller/papers/zipper/paper.pdf): precision is lost in few methods, found by their value-flow
patterns after a context-insensitive pass, and context sensitivity is applied to them only. Wei, Ryder, "Adaptive
Context-sensitive Analysis for JavaScript" (ECOOP 2015, https://core.ac.uk/download/62920104.pdf): per function, call
site, object or parameter sensitivity. Andreasen et al. / Wei et al. (FSE 2016,
https://static.aminer.org/pdf/20170130/pdfs/sigsoft/a1sexadxiwfthg5rq4mpftjc2rdloyin.pdf): the root causes are under 2%
of a JavaScript application's functions. Andreasen, Møller, "Determinacy in static analysis for jQuery" (TAJS): nested
closures need the outer frame's context. Lhoták, Hendren, SPARK (https://plg.uwaterloo.ca/~olhotak/pubs/cc03.pdf): the
call graph built on the fly, only reachable methods analysed.

What the cause asks for: a closure or an object made in a call carries that call's context (heap cloning), and the
factories and the methods that read their products are analysed per context (arguments and receiver, the Cartesian
product of Agesen's CPA, selected by flow patterns as Zipper does). That is a new value model for the tracer, not a rule
added to the present one; the decision is the owner's (systematic-debugging: a fix that needs an architectural change
is discussed first).

## JavaScript and TypeScript values by the languages' own rules (research, 2026-11-11 07:20)

Found by reading the solver's state on the lab (probe of the links at 0.5, 1.0 and 1.5 M): at 1.5 M, 1 175 689 links
are `handed` links to draco's minified inner functions (`x`, `z`, `h`, `b`, ...), 21 228 value nodes hold 1 000 or more
values, and a parameter that holds one class (`b: Building`) holds 2 279: raw annotation texts (`Building | null`, `T`,
`unknown`, `Frame`), each a value of its own in every file and class it is written in, and `std::pair<Building, void>`
made by the C++ library's rule. Read in the code: `JsModules.globals` takes every unit a script owns (its inner
functions too); `Index.members` takes every unit, lambdas included; `Typer.hold_node`/`ret_node` add a unit's raw
`type_text`; `transfer.operator_one` treats a binary `*` as dereference and passes the operands of JS arithmetic
through; `new X()` in JS is the text `X` (a variable holding classes, or a function, gives nothing).

Sources: MDN `new` (a function is called as a constructor with `this` a new object; a non-primitive it returns is the
result, https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/new); MDN Global object, `var`,
`function` (only top-level `var` and function declarations of a script are global; a declaration in a function is its
local; modules have their own scope); MDN arithmetic and bitwise operators (operands coerced to numeric or string
values: the result is a number, a BigInt or, for `+`, a string); MDN Map (iteration yields `[key, value]` arrays,
`get` the value); TypeScript handbook, "Erased Types" (annotations are removed and never change the program's behaviour;
interfaces, aliases and type parameters have no runtime value).

| Rule | What the tracer does |
|---|---|
| L1 A function written inside a function is that function's local (C++ [expr.prim.lambda], GDScript lambdas, JS function declarations and expressions) | a lambda unit is never a member of the class, namespace or module around it: it is reached through the enclosing function's local (`unit.lambdas`) or the value that holds it |
| L2 A classic script's global scope | its top-level declarations only (`var`, `let`, `const`, `function`, `class`), never a function written inside a function |
| T1 Types are erased | an annotation gives what it names that has a runtime value: a class of the program, each member of a union or intersection, `typeof x` the value of `x`; containers of the language as containers of their element type (below); a type parameter, primitive, literal, function, object or tuple type, `keyof`, an indexed, conditional or mapped type, or a name that names no class gives no value: the values come from the flows |
| T2 The language's containers (ECMAScript built-ins) | `T[]`, `readonly T[]`, `Array`, `ReadonlyArray`, `Set`, `ReadonlySet`, `WeakSet`, `Iterable`, `IterableIterator`, `Iterator`, `Generator`, `AsyncIterable`, `AsyncIterableIterator`, `AsyncGenerator`, `ArrayLike` of T: index, iteration and the element calls give T; a tuple `[A, B]`: A or B; `Map`, `ReadonlyMap`, `WeakMap` of K, V: `get` and `values` give V, `keys` K, iteration and `entries` give `[K, V]` arrays; `Record<K, V>` and an index signature: a member or index gives V; `Promise`/`PromiseLike` of T: `await` and `then`'s callback give T; `Partial`, `Readonly`, `Required`, `NonNullable`, `Awaited`, `Pick`, `Omit` of T: T |
| T3 A declared return type | the function's returns hold what T1 and T2 give for it; a unit's annotation text is never itself a value, and a JS value never depends on the file it is read in (its names are resolved when it is made) |
| O1 JS operators | arithmetic, bitwise, exponent and unary numeric operators give the language's primitive value; `valueOf`/`toString` stay implicit calls (J5); `??`, `||`, `&&` and the ternary keep their operands (already) |
| O2 Binary `*` | multiplication in every language (C++ `operator*` of two operands); dereference is the unary form (`deref`) only |
| N1 `new X(...)` | X is evaluated: a class gives its instance (and its constructor is called, as before); a function is called as a constructor (linked as `new`, its arguments flow) and gives the objects it returns; anything else is the host's constructor (the language's value) |
| T4 A computed property `o[k]` (MDN property accessors) | on an object of the program: any of its class's (and bases') properties; a string key written is a member read (already) |
| J5' The iteration protocol (MDN Iteration protocols, `function*`) | for-of, spread and `yield*` over an object of the program give its `[Symbol.iterator]()`'s `next().value`; a generator call gives its generator object, whose iteration and `next().value` give what it `yield`s (`yield* e`: e's elements); an iterator result's `value` is an element |

Measured, then corrected (2026-11-11 09:30): T4 above, "`o[k]` any property", fed every method of every class into
three's computed reads (`BufferAttribute.getX`: `this.array[index * this.itemSize]` returned 3 570 functions,
`getConstNode`'s `value` held 3 632) and the lab's first propagation passed 13 M values in 30 s without ending. Sources:
Sridharan, Dolby, Chandra, Schäfer, Tip, "Correlation Tracking for Points-To Analysis of JavaScript" (ECOOP 2012):
dynamic property accesses read as any property are the main obstacle to analysing JS frameworks; Feldthaus, Schäfer,
Sridharan, Dolby, Tip, "Efficient Construction of Approximate Call Graphs for JavaScript IDE Services" (ICSE 2013):
dynamic property reads with an unknown name are left out, and the call graphs keep a high recall.

| Rule | What the tracer does |
|---|---|
| T4' A computed property `o[k]` (replaces T4) | the members named by each string value `k` can hold (G7, the key's values beside the object's, both growing); a key that is no known string reads no property of an object of the program; the language's containers and the program's own (ELEM) give their elements by index as before |

## Property names as values; types of the host (research, 2026-11-11 10:30)

Sources: MDN `for...in` (every enumerable string property, inherited ones too), `Object.keys` / `Object.values` /
`Object.entries` (the own enumerable string-keyed properties: names, values, `[name, value]` pairs); MDN "Enumerability
and ownership of properties" (a class's methods are on its prototype and not enumerable; instance fields and an object
literal's properties are); TypeScript handbook, "Type Declarations" (a type name the program does not declare comes
from a declaration file: the host's `lib` or a library's types).

| Rule | What the tracer does |
|---|---|
| P1 Enumerable properties of an object of the program | an object literal: every property it writes (its methods too) and its spread bases'; an instance of a class: the instance fields of the class and its bases (methods, accessors and static members are not enumerable on it) |
| P2 `for (k in o)` | k holds the names of o's enumerable properties as strings (G7), so `o[k]` reads exactly those (T4'); over an array or a container of the language, its indices: the language's value |
| P3 `Object.keys(o)`, `Object.values(o)`, `Object.entries(o)` (the language's `Object`, not a name of the program) | an array of the names as strings; of the properties' values; of `[name, value]` arrays |
| H1 A TypeScript type name the program does not declare | a type of the host or of a library not read: the host's value (a name the program declares as a class, interface, enum, alias or import of a module read keeps T1) |

### Closures and the names they share (found by a probe of the first nodes that received draco's `l`, 2026-11-11 11:30)

The draco script's own top-level `l` (a real global, L2) reached three's buffers through two readings of closures:
`exhaust.ts`'s `life[i] = l` written in a method of the object `createExhaust` returns was evaluated in
`createExhaust`'s scope (`model.assign` put the closure's tree into the enclosing function's list), where `l` is no
local; and minified modules (chevrotain, lil-gui) use, in a closure, a name their enclosing function declares later
(the reader copied the enclosing names when it read the closure). Sources: MDN Closures, `var` hoisting, `let`/`const`
(a closure's names are resolved in its enclosing scopes when it runs; declarations are hoisted, the temporal dead zone
is a run-time condition).

| Rule | What the tracer does |
|---|---|
| J7' A value written to a shared local (JS closures, C++ captures by reference) | is evaluated where it is written; the enclosing function's local holds what the writer's holds |
| J7'' A JS closure's name | is looked up in its enclosing functions whatever the order the declarations are written in (C++ and GDScript lambdas see only what is declared before them) |

## Call results per argument values: templates (research, 2026-11-11 12:30)

Measured on the lab after the JS rules (drain probes): the sets that still multiply are real values merged by
functions called from many places: i18n `t(key)` returns every key of the program (1 569 strings) to every caller;
`getConstNode(value)`, `ShaderNodeObject(obj)` return what any caller passed; generic setters fill `Vector3.x` with
colour strings. Sources: Agesen, "The Cartesian Product Algorithm" (ECOOP 1995,
https://bibliography.selflanguage.org/cpa.html): at a call site the product of the receiver's and arguments' types is
formed and each tuple analysed as its own template, the site revisited as the types grow; Li, Tan, Møller, Smaragdakis,
Zipper (OOPSLA 2018): precision is lost where a parameter flows to the result directly, wrapped or unwrapped, and
context is spent there only; Sridharan et al. (ECOOP 2012): correlated property accesses extracted and analysed per
property name.

| Rule | What the tracer does |
|---|---|
| C1 Which functions | a function whose parameters reach its result: directly (a local holding it, a union, `await`), through a member or an element read of it, or through a call by name of such a function on it; the parameters that reach it are its key; a function that writes an inner function keeps one template (its closures read its locals, B3) |
| C2 A call of such a function | each tuple of the values its key parameters can hold at that call is a template of the function: its parameters hold that tuple (the others what the calls of the template pass), its result is the call's; the call's values are the union over its tuples (Agesen's CPA, values only grow) |
| C3 The call graph | links stay those of the function read once over every call (a superset): the templates refine the values the calls give, never remove a link |
| C1' (replaces C1's last clause) | a function that writes an inner function has templates too: a template's closures read the unit's own locals, which every call feeds (C3), a superset of the template's: sound |
| C2' (replaces C2's product; measured: the city did not reach its fixpoint in 600 s, functions with two or three key parameters of hundreds of values each made their products) | for each key parameter, each value it holds at a call is a template (that parameter fixed, the others what the call writes); each parameter's family of results is a superset of the call's result, and the call holds their intersection (the meet of sound approximations, abstract interpretation's reduced product): templates grow as the sum of the values, not their product; one key parameter is Agesen's templates exactly |
| C2'' A template's parameters (found by the fixture: `either(a: Box | Tile, b: Box | Tile)` gave both classes to every call) | a template exists because a call writes its arguments: its key parameter holds its key value alone; its other parameters what its calls write, and their default value; never the type declared for them (T1 stays the unit's, for parameters no call of the program writes) |

### Templates share what their key does not reach (found by stack samples of the stalled lab run, 2026-11-11 14:00)

The lab with three.js did not reach its fixpoint in 900 s after C2'': stack samples showed the time in copying every
argument of a call into every template (`templates.route`, `other`) and in delivering each template's result into the
call one value at a time (`transfer.substituted`). The cause read in the code: a template was the whole unit read again
(every tree of the body its own node under the template's key, the calls in it included), so a function whose key held
a hundred values was wired a hundred times, and each copy's result, the parts no key reaches included, was delivered on
its own. Sources: Agesen (CPA: two templates differ only in the values of their arguments); Zipper (context is spent on
the flows of the selected parameters); Smaragdakis, Bravenboer, Lhoták, "Pick Your Contexts Well" (POPL 2011: a
context-sensitive copy is needed only where its value can differ between contexts).

| Rule | What the tracer does |
|---|---|
| C2''' A template's nodes | a tree is a template's own node only when it reads a key parameter, through any shape (an operand, a receiver, an argument, a local holding one); every other tree reads nothing a template binds, so it is the unit's node, the same in every template (exact); a parameter that is no key is the unit's, every call's (C1 selects the parameters that reach the result) |
| C2''' A call's result | is made of parts (the declared type, a JS annotation's values, a generator's object, each returned tree with its unions opened): a part no key reaches is one node, delivered to the call once whatever the number of templates; the result of one function at one call is one node, so a second reading of the same call (another receiver with the same inherited method) delivers it again, as it must |
| M1 A member's values read on a receiver | are its values as they are (an identity subscription, merged with its cycle) unless the receiver is a class template written with its arguments, whose parameters are then replaced (`substituted`) |

A first reading of C2''' shared every tree C1's flows do not reach (`reaches`): a tree reading a key parameter through an
operator or a method call would have been the unit's, mixing every call's values; the rule is by reads. A defect read in
the code of the earlier `route`: its subscription was kept per call and function, so a second reading of the same call
with another delivery (the class-template substitution of another receiver) delivered nothing.

## C++ declarations read by the standard (found by a probe of the first float field that held a `V2`, 2026-11-11 14:40)

Comparing HEAD's tracer and the new one over the same city sources left 61 links from float arithmetic to
`V2::operator+ - * /` the earlier template run did not have. A probe walking the subscriptions back from `V2::x` (a
float) to where a `V2` entered it found `const V2 corner[] = { f.at(...), ... }` (canyon_field.cpp) read as one
`V2(px, py)` construction with the four elements as its arguments: `px` received a `V2`, `x` from it, and every
arithmetic on floats then reached `V2`'s operators. Fixing it showed a second misreading: `const Vector3
HEAD_BOX(SKULL_W, SKULL_H, SKULL_D);` at namespace scope was read as a function (its value a callable, then handed to
the engine through an aggregate). Sources: ISO C++ [dcl.init.aggr] (an array is an aggregate: each element is
copy-initialized from its initializer-clause, the elements with none are value-initialized), [dcl.init.list], [dcl.array]
(a size not written is the clauses' count), [dcl.ambig.res] and [stmt.ambig] (what can be a function declaration is
one: each parameter must be a parameter-declaration, its decl-specifier a type name; else the declarator declares an
object direct-initialized with the expressions written).

| Rule | What the tracer does |
|---|---|
| C8 `T a[] = { c1, c2 }` | each element is initialized from its own clause: a braced clause constructs it with its elements, an expression copies it (FLOW, no constructor written); the elements with no clause (a written size larger than the clauses, or none) are default-constructed; an array of pointers, a pointer or a reference constructs nothing |
| C9 `T x(a, b);` | the parentheses are read by the expression grammar too (the same text parsed as a call's arguments, its lines kept). Once every file is indexed, the names written where each parameter's type goes are looked up: one that is a value of the program (a variable, a constant, a field, an enumerator, a function; in a block a local) makes it an object (at namespace scope a variable) direct-initialized with the expressions, its construction and what they read and call recorded; all types (a built-in type is one) make it a function; a name the program does not declare (a library's, `std::move`) keeps both readings, so no use is lost |

Measured on the city (same sources, HEAD's tracer against this one): 775 links from float arithmetic to `V2`'s
operators and 4 `V2` constructions with copied elements are gone; `HEAD_BOX` and its kind are read (`read`, no longer
`value` of a function); 87 links are new, every one a real read the direct-initializations make (the mutexes a
`std::lock_guard lock(m)` reads, the constants a `Vector3 o(r[LINE_ORIGIN], ...)` reads). The lab app's links are the
same 184 875.

C9 replaces the row "Direct-initialization in a block" of "The pending rules found against a real run" (kept as it was
written): a block's `T x(a, b);` is an object only when the lookup says so, and the same rule holds at namespace scope.

## Why the lab with three.js does not reach its fixpoint after P3 (found by probes of the solver's state, 2026-11-11 15:30)

With P1-P3, H1, J7', templates and C8/C9, the lab with three.js linked 453 596 links in 900 s and stopped at the deadline
(63 228 consumers waiting; 356 186 links in 104.5 s before P1-P3). Stack samples with the sizes of each frame's locals
(330-570 s): the time is in the deep walkers of the program, `unshared`'s `walk` (jobs/processWorker.ts) and `jsonCopy`
(world/buildings/types.ts), instantiated for every class that reaches them (103 563 templates), and in numeric places
that hold objects and shader texts (`_vector4.z ∪ invW`: 1 664 values; `size.x`: three's `ContextNode`; a `Vector3`'s
`x`: Collada parser objects; `y`: GLSL source). A probe stopping the worker between two propagations and walking the
subscriptions back from three's `Vector3.x` found where objects enter: identity flows from locals holding a `Vector3`
(`camera.lookAt(middle)` in farBatchSheet.ts, the same shape in five other lab sheets) into three's

    lookAt( x, y, z ) { if ( x.isVector3 ) { _target.copy( x ); } else { _target.set( x, y, z ); } }

The tracer's flows ignore the branch: `set`'s `x` receives the `Vector3`, `this.x = x` writes it into every `Vector3`'s
`x`, arithmetic and member reads on `x` then reach whatever the `Vector3` reaches, and P3 (`Object.keys` naming each
object's own properties) lets the walkers copy every field of every object they are handed into what they return. Three
writes its overloads this way throughout (`Color.set(r)`: `r.isColor`, `typeof r === 'number'`, `typeof r ===
'string'`). The rule the languages give is narrowing at branch conditions (TypeScript's control-flow analysis: type
guards by truthiness of a member, `typeof`, `instanceof`, `Array.isArray`, `in`, equality and discriminants, type
predicates, `!`, `&&`, `||`, and the code after an `if` whose branch always leaves; GDScript `is`, already written in the
EXPR table as "inside the branch it guards, the tested name is narrowed to T" and not implemented): the next theme.

## Narrowing at branch conditions (research, 2026-11-11 16:30)

Sources (read through context7): TypeScript handbook, "Narrowing" (`typeof` type guards, truthiness, equality, the
`in` operator, `instanceof`, assignments, control flow analysis, type predicates `parameterName is Type`, discriminated
unions by `if` and `switch`); TypeScript 2.0 release notes (control flow analysis: after `if (typeof x === "number")
return;` x is no longer a number; an assignment in the branch replaces the narrowed type); TypeScript 4.4 (aliased
conditions and discriminants, only for a `const`, a `readonly` property or an unmodified parameter); MDN "Falsy" (`false`,
`0`, `-0`, `0n`, `""`, `null`, `undefined`, `NaN`, `document.all`; every other value truthy), `typeof` (its exhaustive
table: "undefined", "object" for null and any object, "boolean", "number", "bigint", "string", "symbol", "function" for
functions and classes), `instanceof` (the prototype chain, or `[Symbol.hasInstance]`), `in` (a property of the object or
of its prototype chain), "Inheritance and the prototype chain" (a property put on `C.prototype` is read by every instance
of C; an assignment to a property the object does not have creates it; a property found nowhere on the chain reads
`undefined`); Godot, GDScript reference (`is` tests whether a value extends a class or is of a built-in type).

What the tracer had: branches are followed with their condition as text only, so the values flowing through a branch
are the values before it ("Why the lab with three.js does not reach its fixpoint after P3"); GDScript replaced a name
an `is` proves with the tested type inside the `if` body only; JS `true`, `false`, `null` and `undefined` were the
language's value (ENGINE), so no test could tell them apart; a property written outside the class (`o.extra = v`,
`C.prototype.flag = true`) and a property found nowhere were unread and the engine's value.

| Rule | What the tracer does |
|---|---|
| V1 The language's constants | JS `true`, `false`, `null` and `undefined` (the literal and the global name) are values of their own (`<lang>true`, ...); like the engine's value in every rule that does not test them (members, steps, calls, identity) |
| V2 A property written where no class declares it (MDN) | `o.p = v` on an object of class K that declares no `p` creates `p` on K's objects: the values written are K's property `p` (`#prop` node); `C.prototype` read on a class C is C's objects, so `C.prototype.p = v` gives every C (and every class below) the property; a read of `p` on K is K's and its bases' declared member, else what was written to `p` on K or its bases; a property no class of a chain the tracer read whole declares or writes is `undefined`, one a base the tracer did not read may hold is the engine's |
| G1 Guards (JS) | a condition proves facts about the values of a name (a local or a parameter): its truthiness; the truthiness of one of its properties (`x.p`, `x?.p`); `typeof x === "t"`; `x instanceof C`; `Array.isArray(x)` (the language's `Array`); `"p" in x`; `x === v`, `x !== v` with `v` a literal of the language or a string; `x == null`, `x != null`; `x.p === v` (a discriminant); `f(x)` where f is declared with a return type `x is T` (a type predicate); `!`, `&&` (both facts when true), `||` (both negations when false), parentheses |
| G2 Guards (GDScript) | `x is T`, `not`, `and`, `or`, parentheses |
| R1 Where facts hold | the `if` branch (the facts of the condition being true), the `else` branch (false), the statements after an `if` whose other branch always leaves (`return`, `throw`, `break`, `continue`, or a block or `if` that always does), the operands of `?:`, the right operand of `&&` (left true) and of `||` (left false), a loop's body (its condition true), a `switch` case that is entered only by its own test (the discriminant equal to its literal) |
| R2 Where they do not | a name assigned inside the region (an assignment, an update, a destructuring or loop binding), or assigned by any function written inside the enclosing function (a closure may run between the test and the use): such a name is read whole there; a property path (`a.b`) is never narrowed (any call may write it) |
| N1 A narrowed read | a name read inside a region is a narrowed node of the name's node: each value goes through each fact that holds, kept when the fact can be true of it (positive) or false of it (negative); a value the tracer cannot decide (the engine's, a number) is kept; an object of the program is decided by its class (and its bases), the value of a property by that property's values (each value of it decides as it comes); a class or a predicate named by the condition is decided by its values as they come; facts compose (nested regions narrow in turn) |
| N2 Precision kept | narrowing only removes values that cannot be there at run time; it never adds a value and never removes a link the call graph needs (C3) |
