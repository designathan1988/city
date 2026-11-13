; TypeScript and JavaScript (tree-sitter-typescript) in the tracer's words: where each fact is in the grammar. The one
; word compared with `#eq?` is the language's own (ECMA-262: a class's `constructor` method).

(function_declaration name: (identifier) @name return_type: (_)? @type body: (_) @body) @function
(generator_function_declaration name: (identifier) @name return_type: (_)? @type body: (_) @body) @function
(function_expression name: (identifier)? @name return_type: (_)? @type body: (_) @body) @function
(generator_function name: (identifier)? @name body: (_) @body) @function
(arrow_function return_type: (_)? @type body: (_) @body) @function
(method_definition name: (_) @name return_type: (_)? @type body: (_) @body) @function
((method_definition name: (property_identifier) @_made) @constructor (#eq? @_made "constructor"))
(method_definition "get" name: (_)) @getter
(method_definition "set" name: (_)) @setter
(function_signature name: (identifier) @name return_type: (_)? @type) @function
(method_signature name: (_) @name return_type: (_)? @type) @function
(abstract_method_signature name: (_) @name return_type: (_)? @type) @function
(function_type return_type: (_)? @type) @function

(class_declaration name: (_) @name) @class
(abstract_class_declaration name: (_) @name) @class
(class name: (_)? @name) @class
(interface_declaration name: (_) @name) @class
(internal_module name: (identifier) @name) @namespace
(extends_clause value: (_) @base)
(implements_clause (_) @base)
(extends_type_clause (_) @base)
(generic_type name: (_) @name) @named
(type_alias_declaration name: (type_identifier) @name value: (_) @type) @variable
(type_parameter name: (type_identifier) @name constraint: (_)? @type) @variable

; A name taken out of an object or a list, wherever the pattern is written: each is a variable; in a declaration or a
; parameter, the property of its name (of that key) of what is taken apart.
(variable_declarator name: (identifier) @name type: (_)? @type value: (_)? @value) @variable
; A variable declared with `declare` (TypeScript handbook: Declaration Reference, ambient declarations): defined by
; code not read, its value an input.
(ambient_declaration (lexical_declaration (variable_declarator) @elsewhere))
(ambient_declaration (variable_declaration (variable_declarator) @elsewhere))
(object_pattern (shorthand_property_identifier_pattern) @name @variable)
(object_pattern (pair_pattern value: (identifier) @name @variable))
(object_pattern (object_assignment_pattern left: (shorthand_property_identifier_pattern) @name @variable))
(array_pattern (identifier) @name @variable)
(rest_pattern (identifier) @name @variable)
(variable_declarator name: (object_pattern (shorthand_property_identifier_pattern) @name @variable) value: (_) @object)
(variable_declarator name: (object_pattern (pair_pattern key: (_) @key value: (identifier) @name @variable))
  value: (_) @object)
(variable_declarator name: (array_pattern (identifier) @name @variable) value: (_) @value)
(public_field_definition "readonly"? @fixed name: (_) @name type: (_)? @type value: (_)? @value) @variable
(property_signature name: (_) @name type: (_)? @type) @variable
(for_in_statement left: (identifier) @name right: (_) @value) @variable
(for_in_statement left: (array_pattern (identifier) @name @variable) right: (_) @value)
(catch_clause parameter: (identifier) @name) @variable

(required_parameter pattern: (identifier) @name type: (_)? @type value: (_)? @value) @param
(optional_parameter pattern: (identifier) @name type: (_)? @type value: (_)? @value) @param @optional
(required_parameter pattern: (rest_pattern (identifier) @name) @rest type: (_)? @type) @param
(required_parameter pattern: (object_pattern) type: (_)? @type) @param
(required_parameter pattern: (array_pattern) type: (_)? @type) @param
(required_parameter pattern: (object_pattern (shorthand_property_identifier_pattern) @name @variable)) @object
(required_parameter pattern: (object_pattern (pair_pattern key: (_) @key value: (identifier) @name @variable))) @object
(arrow_function parameter: (identifier) @name @param)
(required_parameter (accessibility_modifier) pattern: (identifier) @name type: (_)? @type) @field

(assignment_expression left: (_) @left right: (_) @right) @assign
(augmented_assignment_expression left: (_) @left right: (_) @right) @assign
(update_expression argument: (_) @left) @assign

(call_expression function: (_) @callee type_arguments: (_)? @type arguments: (arguments) @args) @call
; A tagged template calls its tag with each substitution (ECMA-262, TaggedTemplate).
(call_expression function: (_) @callee arguments: (template_string) @args) @call
(template_substitution (_) @value) @pass
(new_expression constructor: (_) @callee type_arguments: (_)? @type arguments: (arguments)? @args) @new
(member_expression object: (_) @object property: (_) @property) @member
(subscript_expression object: (_) @object index: (_) @key) @index

; What holds what it is given: a choice (`?:`, `||`, `??`, `&&`), brackets, what is waited for, what is spread, the
; last of a row, the value an assignment gives, a value said to be there or to fit a type; a cast, which is also an
; object of its type.
(parenthesized_expression (_) @value) @pass
(ternary_expression consequence: (_) @value) @pass
(ternary_expression alternative: (_) @value) @pass
(binary_expression left: (_) @value operator: ["||" "??" "&&"]) @pass
(binary_expression operator: ["||" "??" "&&"] right: (_) @value) @pass
(await_expression (_) @value) @pass
(spread_element (_) @value) @pass
(sequence_expression (_) @value .) @pass
(assignment_expression right: (_) @value) @pass
(non_null_expression (_) @value) @pass
(satisfies_expression . (_) @value) @pass
(as_expression . (_) @value (_) @type .) @cast

(return_statement (_) @value) @return

; A call made for its effect: its value is taken by nothing (or only waited for).
(expression_statement (call_expression) @effect)
(expression_statement (await_expression (call_expression) @effect))
(arrow_function body: (expression) @value) @return

; The types that may be any value: the ones the language leaves open (TypeScript handbook, Everyday Types: `any`,
; `unknown`, `object`), one written in place, `this`, and the ones computed from others.
((predefined_type) @open (#any-of? @open "any" "unknown" "object"))
(object_type) @open
(this_type) @open
(type_query) @open
(lookup_type) @open
(index_type_query) @open
(conditional_type) @open

(this) @self
(super) @super
(identifier) @ident
(shorthand_property_identifier) @ident
(type_identifier) @ident

(import_statement (import_clause (named_imports (import_specifier name: (_) @name alias: (_)? @alias)))
  source: (string) @source) @import
(import_statement (import_clause (namespace_import (identifier) @module)) source: (string) @source) @import
(import_statement (import_clause (identifier) @default) source: (string) @source) @import
(call_expression function: (import) arguments: (arguments . (string) @source .)) @import
(export_statement (export_clause (export_specifier name: (_) @name alias: (_)? @alias)) source: (string) @source) @import
(export_statement . (string) @source) @include
(export_statement !source (export_clause (export_specifier name: (_) @name alias: (_) @alias))) @export
(export_statement "default" declaration: (_) @value) @export.default
(export_statement "default" value: (_) @value) @export.default

(pair key: (_) @key value: (_) @value) @pair
(object (method_definition name: (_) @key) @value @pair)
(object (shorthand_property_identifier) @key @value @pair)

; An object written in place is an object of its own shape: each entry written with a name (a key that is a name or
; a text, a name written alone, a method) is a member of it.
(object) @shape
(object (pair key: [(property_identifier) (string)] @name value: (_) @value) @variable)
(object (shorthand_property_identifier) @name @value @variable)
[(string) (template_string) (number) (array) (object) (true) (false) (null) (undefined) (regex)] @literal

; What decides which code runs and what a value is (ECMA-262: IfStatement, the iteration statements,
; ConditionalExpression, SwitchStatement, ReturnStatement, ThrowStatement; the binary and unary operators).
(if_statement condition: (_) @test consequence: (_) @then) @branch
(while_statement condition: (_) @test body: (_) @then) @branch @loop
(for_statement condition: (_) @test body: (_) @then) @branch @loop
(for_in_statement body: (_) @then) @branch @loop
(do_statement body: (_) @then condition: (_) @after) @branch @loop
(break_statement) @leave
(continue_statement) @again
(ternary_expression condition: (_) @test consequence: (_) @then alternative: (_) @else) @branch
(switch_statement value: (_) @test) @choose
(switch_case value: (_) @value) @case
(switch_default) @case
(return_statement) @exit
(throw_statement) @exit
(binary_expression left: (_) @left operator: _ @op right: (_) @right) @operation
(unary_expression operator: _ @op argument: (_) @right) @operation
