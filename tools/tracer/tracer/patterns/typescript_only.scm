; What TypeScript writes that its JSX grammar cannot (the tsx grammar reads `<T>x` as an element): a cast written
; before its value.

(type_assertion (type_arguments (_) @type) (_) @value) @cast
