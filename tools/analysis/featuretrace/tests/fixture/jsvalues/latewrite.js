// A function declaration is hoisted: its body is read before the enclosing function's later declarations, yet what
// it writes to them reaches them (log/trace-rules.md, J7'; MDN Hoisting, closures).
class LateBox {}

export function lateWrite() {
  let kept = null;
  function fill() { kept = new LateBox(); }
  fill();
  return kept;
}

// Two locals of one name in two blocks (scopes): the block's is read in the block, though the function declares a
// function of that name later.
export function scopedLocal() {
  let got;
  {
    const chosen = { r: [1] };
    got = chosen.r;
  }
  const chosen = () => 2;
  chosen();
  return got;
}
