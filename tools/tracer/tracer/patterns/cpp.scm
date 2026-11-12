; C++ (tree-sitter-cpp) in the tracer's words: where each fact is in the grammar. A definition written out of its scope
; (`A::f`) is told by its name being a member of that scope, a constructor by its name being its class's ([class.ctor],
; languages.py). The words compared are the language's own (the cast keywords, [expr.static.cast] and the three beside
; it). A declaration defines one name for each declarator it writes: each is its own fact.

(function_definition type: (_)? @type declarator: (function_declarator declarator: (_) @name) body: (_)? @body) @function
(function_definition type: (_)? @type
  declarator: (pointer_declarator declarator: (function_declarator declarator: (_) @name)) body: (_)? @body) @function
(function_definition type: (_)? @type
  declarator: (reference_declarator (function_declarator declarator: (_) @name)) body: (_)? @body) @function
(lambda_expression body: (_) @body) @function

; A function declared: its name is a name, never a declarator in brackets (that is a variable holding a function).
(field_declaration type: (_)? @type declarator: (function_declarator
  declarator: [(identifier) (field_identifier) (operator_name) (destructor_name) (template_function)] @name
  parameters: (_) @args) @function)
(field_declaration type: (_)? @type declarator: (pointer_declarator declarator: (function_declarator
  declarator: [(identifier) (field_identifier) (operator_name) (template_function)] @name parameters: (_) @args) @function))
(field_declaration type: (_)? @type declarator: (reference_declarator (function_declarator
  declarator: [(identifier) (field_identifier) (operator_name) (template_function)] @name parameters: (_) @args) @function))
(declaration type: (_)? @type declarator: (function_declarator
  declarator: [(identifier) (field_identifier) (qualified_identifier) (operator_name) (destructor_name) (template_function)] @name
  parameters: (_) @args) @function)
(declaration type: (_)? @type declarator: (pointer_declarator declarator: (function_declarator
  declarator: [(identifier) (field_identifier) (qualified_identifier) (operator_name) (template_function)] @name
  parameters: (_) @args) @function))
(declaration type: (_)? @type declarator: (reference_declarator (function_declarator
  declarator: [(identifier) (field_identifier) (qualified_identifier) (operator_name) (template_function)] @name
  parameters: (_) @args) @function))

(class_specifier name: (_) @name body: (_)) @class
(struct_specifier name: (_) @name body: (_)) @class
(base_class_clause [(type_identifier) (qualified_identifier) (template_type)] @base)
(namespace_definition name: (_) @name) @namespace
(nested_namespace_specifier (_) @object (_) @property) @member

(type_parameter_declaration (type_identifier) @name) @variable
(optional_type_parameter_declaration name: (type_identifier) @name default_type: (_)? @type) @variable
(alias_declaration name: (type_identifier) @name type: (_) @type) @variable
(type_definition type: (_) @type declarator: (type_identifier) @name) @variable
(enumerator name: (identifier) @name) @variable

(template_function name: (_) @name) @named
(template_method name: (_) @name) @named
(template_type name: (_) @name) @named

(declaration type: (_) @type declarator: (identifier) @name @variable)
(declaration type: (_) @type declarator: (pointer_declarator declarator: (identifier) @name) @variable)
(declaration type: (_) @type declarator: (reference_declarator (identifier) @name) @variable)
(declaration type: (_) @type declarator: (array_declarator declarator: (identifier) @name) @variable)
(declaration type: (_) @type declarator: (init_declarator declarator: (identifier) @name value: (_) @value) @variable)
(declaration type: (_) @type
  declarator: (init_declarator declarator: (pointer_declarator declarator: (identifier) @name) value: (_) @value) @variable)
(declaration type: (_) @type
  declarator: (init_declarator declarator: (reference_declarator (identifier) @name) value: (_) @value) @variable)
(declaration type: (_) @type
  declarator: (init_declarator declarator: (array_declarator declarator: (identifier) @name) value: (_) @value) @variable)
(structured_binding_declarator (identifier) @name @variable)
(field_declaration type: (_) @type declarator: (field_identifier) @name @variable)
(field_declaration declarator: (field_identifier) @name @variable . default_value: (_) @value)
(field_declaration type: (_) @type declarator: (pointer_declarator declarator: (field_identifier) @name) @variable)
(field_declaration type: (_) @type declarator: (reference_declarator (field_identifier) @name) @variable)
(field_declaration type: (_) @type declarator: (array_declarator declarator: (field_identifier) @name) @variable)
(for_range_loop type: (_) @type declarator: (identifier) @name right: (_) @value) @variable
(for_range_loop type: (_) @type declarator: (reference_declarator (identifier) @name) right: (_) @value) @variable

; A variable holding a function, written with its declarator in brackets.
(field_declaration declarator: (function_declarator
  declarator: (parenthesized_declarator (pointer_declarator declarator: (_) @name))) @variable)
(declaration declarator: (function_declarator
  declarator: (parenthesized_declarator (pointer_declarator declarator: (_) @name))) @variable)
(declaration declarator: (init_declarator declarator: (function_declarator
  declarator: (parenthesized_declarator (pointer_declarator declarator: (_) @name))) value: (_) @value) @variable)
(parameter_declaration declarator: (function_declarator
  declarator: (parenthesized_declarator (pointer_declarator declarator: (identifier) @name)))) @param

(parameter_declaration type: (_) @type declarator: (identifier) @name) @param
(parameter_declaration type: (_) @type declarator: (pointer_declarator declarator: (identifier) @name)) @param
(parameter_declaration type: (_) @type declarator: (reference_declarator (identifier) @name)) @param
(parameter_declaration type: (_) @type declarator: (array_declarator declarator: (identifier) @name)) @param
(optional_parameter_declaration type: (_) @type declarator: (identifier) @name default_value: (_) @value) @param
(optional_parameter_declaration type: (_) @type
  declarator: (pointer_declarator declarator: (identifier) @name) default_value: (_) @value) @param
(optional_parameter_declaration type: (_) @type
  declarator: (reference_declarator (identifier) @name) default_value: (_) @value) @param

(assignment_expression left: (_) @left right: (_) @right) @assign

(call_expression function: (_) @callee arguments: (argument_list) @args) @call
(new_expression type: (_) @callee arguments: (argument_list)? @args) @new
(declaration type: (_) @callee declarator: (init_declarator value: (argument_list) @args) @new)
(declaration type: (_) @callee declarator: (init_declarator value: (initializer_list) @args) @new)
(field_initializer (field_identifier) @callee (argument_list) @args) @new
(field_initializer (field_identifier) @callee (initializer_list) @args) @new
(field_expression argument: (_) @object field: (_) @property) @member
(qualified_identifier scope: (_) @object name: (_) @property) @member
(subscript_expression argument: (_) @object indices: (subscript_argument_list (_) @key)) @index

; What holds what it is given: a choice, brackets, an address or what is at one, the last of a row, the elements of
; a list in braces, the value an assignment gives; a cast, which is also an object of its type.
(parenthesized_expression (_) @value) @pass
(conditional_expression consequence: (_) @value) @pass
(conditional_expression alternative: (_) @value) @pass
(pointer_expression argument: (_) @value) @pass
(comma_expression right: (_) @value) @pass
(initializer_list (_) @value) @pass
(assignment_expression right: (_) @value) @pass
(cast_expression type: (_) @type value: (_) @value) @cast
((call_expression function: (template_function name: (identifier) @_cast arguments: (template_argument_list (_) @type))
  arguments: (argument_list (_) @value)) @cast
 (#any-of? @_cast "static_cast" "dynamic_cast" "const_cast" "reinterpret_cast"))

(return_statement (_) @value) @return

(this) @self
(identifier) @ident
(type_identifier) @ident
(namespace_identifier) @ident
(field_identifier) @ident

(preproc_include path: (_) @source) @include

[(number_literal) (string_literal) (char_literal) (true) (false) (null) (raw_string_literal)] @literal
