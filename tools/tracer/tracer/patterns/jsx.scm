; JSX (the tsx grammar of tree-sitter-typescript): an element's tag is called with one argument, the object its
; attributes make (TypeScript handbook, JSX: a value-based element is its component, run by the JSX runtime).

(jsx_opening_element name: (_) @callee) @call @props
(jsx_self_closing_element name: (_) @callee) @call @props
(jsx_attribute (property_identifier) @key (jsx_expression (_) @value)) @pair
