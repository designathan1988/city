// A script (no import, no export): sloppy code, where a plain call's `this` is the global object (MDN `this`), unless
// the function says "use strict" (log/trace-rules.md, J2''').
function sloppyThis() { return this; }
function sloppyRead() { return sloppyThis(); }

function strictThis() { 'use strict'; return this; }
function strictRead() { return strictThis(); }
