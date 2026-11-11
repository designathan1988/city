// Two calls of one function on one line: each call site carries its own argument to the parameter.
class Left { side() { return 1; } }
class Right { side() { return 2; } }
function place(p) { return p.side(); }
export function pair() { return place(new Left()) + place(new Right()); }
