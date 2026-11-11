// `new F()` on a function makes an object whose properties are what F writes on `this` (log/trace-rules.md, N1';
// MDN new operator): unless F returns an object, the new object is the result.
function Counter2(n) {
  this.count = n;
  this.tag = 'm';
  this.bump = function () { return this.count; };
}

export function madeObject() { return new Counter2(1); }

export function madeCount() { return new Counter2(1).count; }

export function madeBump() { return new Counter2(1).bump(); }
