; GDScript (tree-sitter-gdscript) in the tracer's words: where each fact is in the grammar. The words compared with
; `#eq?` are the language's own (GDScript reference: `self`, `super`, `preload`, the constructor `_init`, `new`).

(class_name_statement name: (name) @name) @module.name
(extends_statement (_) @base)

(function_definition name: (name) @name return_type: (_)? @type body: (body) @body) @function
(constructor_definition "_init" @name return_type: (_)? @type body: (body) @body) @constructor
(lambda body: (body) @body) @function
(class_definition name: (name) @name) @class

(variable_statement name: (name) @name type: (type)? @type value: (_)? @value) @variable
(const_statement name: (name) @name type: (type)? @type value: (_)? @value) @variable
(signal_statement name: (name) @name) @variable
(enum_definition name: (name) @name) @variable
(for_statement left: (identifier) @name right: (_) @value) @variable

(parameters (identifier) @name @param)
(parameters (typed_parameter (identifier) @name type: (_) @type) @param)
(parameters (default_parameter (identifier) @name value: (_) @value) @param)
(parameters (typed_default_parameter (identifier) @name type: (_) @type value: (_) @value) @param)

(assignment left: (_) @left right: (_) @right) @assign
(augmented_assignment left: (_) @left right: (_) @right) @assign

(call . (_) @callee arguments: (arguments) @args) @call
(attribute) @chain
(attribute (_) (identifier) @property @step)
(attribute (attribute_call (identifier) @property arguments: (arguments) @args) @step)
(attribute (attribute_call (identifier) @construct (#eq? @construct "new")) @step)
(attribute (attribute_subscript (identifier) @property (subscript_arguments (_) @key)) @step)
(subscript . (_) @object (subscript_arguments (_) @key)) @index

; What holds what it is given: a choice (`a if c else b`, `or`, `and`), brackets, what is waited for; a cast, which is
; also an object of its type.
(parenthesized_expression (_) @value) @pass
(conditional_expression left: (_) @value) @pass
(conditional_expression right: (_) @value) @pass
(binary_operator left: (_) @value op: ["or" "and" "||" "&&"]) @pass
(binary_operator op: ["or" "and" "||" "&&"] right: (_) @value) @pass
(await_expression (_) @value) @pass
(binary_operator left: (_) @value op: "as" right: (_) @type) @cast

(return_statement (_) @value) @return

((identifier) @self (#eq? @self "self"))
((identifier) @super (#eq? @super "super"))
(identifier) @ident

(call (identifier) @_load arguments: (arguments (string) @source) (#eq? @_load "preload")) @import

(pair left: (_) @key value: (_) @value) @pair
[(string) (integer) (float) (array) (dictionary) (true) (false) (null)] @literal
