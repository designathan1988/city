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
- C1 C++ keywords, identifiers with special meaning, preprocessor tokens (cppreference keyword, preprocessor).
- C2 Expressions: primary expressions, operator precedence, value categories (expressions, operator_precedence,
  value_category, operator_member_access, operator_incdec).
- C3 Name lookup: unqualified (block, then class and bases, then enclosing namespaces, then global), qualified (through
  using-directives), argument-dependent lookup, class member lookup (unqualified_lookup, qualified_lookup, adl).
- C4 Overload resolution; virtual functions and the final overrider; a qualified call is never virtual (virtual).
- C5 `auto` deduced from the initializer; generic lambda parameters from the call's arguments; deduced return type from
  the `return` statements; templates: parameters replaced by the arguments (auto, template_argument_deduction, lambda).
- C6 Translation unit: a file plus every file its `#include` brings in (preprocessor/include).

## The general rules (one each; every row below names which applies)

| Id | Rule |
|---|---|
| DEF | A definition puts a name in a scope: a unit the walk can land on (function, method, lambda, variable, field, constant, signal, class, enum, enumerator, parameter, local). |
| LOOK | A name used is resolved by its language's lookup order. GDScript (G1, G3): local -> class member (own, then each base, the engine's base members included) -> outer class -> global (class_name, autoload, @GlobalScope, @GDScript, built-in types, engine classes). C++ (C3, C6): block -> class and bases -> enclosing namespaces -> using-directives -> global, limited to the translation unit; plus argument-dependent lookup in the namespaces of the arguments' classes. A name found nowhere in the program is the engine's or the library's. |
| VALUE | The type of an expression is computed from its construct, recursively over its children (the EXPR rows), never from its text. |
| FLOW | A name with no declared type (GDScript untyped / Variant; C++ `auto`, generic lambda parameter, deduced return) holds the union of the types of every value that flows into it: initializer, each assignment, for a parameter the argument at each call site (callers, emitters of a signal it handles, the engine for engine callbacks and engine signals), for a call the values its `return`s carry, for a container the values put into it, for a loop variable the elements walked (G2, C5). |
| CALL | A call resolves its callee (LOOK on the name, VALUE on the receiver), links caller -> callee, binds each argument to its parameter (FLOW), and the callee's result returns to the call site (FLOW). A method call also reaches the overrides in every class below the receiver's (GDScript: every method; C++: `virtual` or overriding, not when qualified) (C4). Overloads are chosen by the arguments (arity first) (C4). |
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
| `&"name"` (StringName) | a name: NAME |
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
