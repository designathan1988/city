// A function declaration is hoisted: its body is read before the enclosing function's later declarations, yet what
// it writes to them reaches them (log/trace-rules.md, J7'; MDN Hoisting, closures).
class LateBox {}

export function lateWrite() {
  let kept = null;
  function fill() { kept = new LateBox(); }
  fill();
  return kept;
}
