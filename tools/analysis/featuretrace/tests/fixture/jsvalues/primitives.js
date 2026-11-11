// Members of the language's values (log/trace-rules.md, T5): a property read on a primitive is its wrapper's
// prototype's (MDN Object coercion: String, Number, Boolean, BigInt, Symbol); on null or undefined it throws.
export function primLength() { const s = "a <common> b"; return s.length; }
export function primUpper() { return "a <common> b".toUpperCase(); }
export function primIndex() { const s = "a <common> b"; return s[1]; }
export function primHas() { return "a <common> b".includes("a"); }
export function primSplit() { return "a <common> b".split(" "); }
export function primMatch() { return "a <common> b".match(/a/); }
export function primMissing() { return "a <common> b".notThere; }
export function primMethod() { return "a <common> b".slice; }
export function primEach() { for (const ch of "a <common> b") return ch; return undefined; }
export function primFixed(k) { return (k * 2).toFixed(2); }
export function primValueOf(k) { return (k * 2).valueOf(); }
export function primBoolText() { return true.toString(); }
export function primNullish() { const z = null; return z.anything; }
export function primSumLength(x) { return ("pre" + x).length; }
