// What the feature uses of src/core/identityMemo.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/identityMemo.ts:2-12 function identityMemo
/** DO NOT key one by anything changed in place (the document, the network, a Map): it would answer for what the object was. */
export function identityMemo<K extends object, V>(compute: (key: K) => V): (key: K) => V;
