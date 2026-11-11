// Function.prototype.call, apply and bind by the receiver's value (log/trace-rules.md, J2''; MDN Function.prototype
// call, apply, bind): on a function they run it, or make the function that runs it, with `this` and leading
// arguments given; on an object with methods of those names, its methods.
class FormSizer {
  constructor() { this.width = 0; }
  setSize(width, height) { this.width = width; return this; }
}

class FormBinder {
  bind(a, b) { return a; }
  call(p) { return p; }
}

export function boundNotCalled() { const s = new FormSizer(); const resize = s.setSize.bind(s); resize(3, 4); return s; }

export function boundLeading() { const f = (a, b) => b; const g = f.bind(null, "first"); return g("second"); }

export function ownBind() { return new FormBinder().bind(1, "x"); }

export function ownCall() { return new FormBinder().call("p"); }

export function callForm() { const f = (a) => a; return f.call(null, "c"); }

export function applyForm() { const f = (a, b) => b; return f.apply(null, ["x", "y"]); }
