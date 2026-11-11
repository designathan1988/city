// What no declaration states, summarized from the declarations (log/trace-core-plan.md, Task 5).
export class Apple {}
export class Pear {}

export function defined() { const o = {}; define(o, "x", { value: new Apple() }); return o.x; }
export function gotten() { const o = {}; define(o, "y", { get() { return new Apple(); } }); return o.y; }
export function merged() { const t = {}; merge(t, { a: new Apple() }); return t.a; }
export function passed() { return pass(new Apple()); }
export function kept() { return keep(new Apple(), new Pear()); }
export function joined() { const t = {}; both(t, { a: new Apple() }); return t.a; }
export function invoked() { const o = { run() { return new Apple(); } }; return invoke(o, "run"); }
