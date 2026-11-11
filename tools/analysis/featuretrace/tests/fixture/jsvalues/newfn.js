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

// J2''': `this` of a function by its calls; N1'': an ordinary function is a constructor with a prototype of its own.
function Shape2(w) {}
Shape2.prototype.area = function () { return 4; };
Shape2.prototype.self = function () { return this; };

export function protoArea() { return new Shape2(1).area(); }

export function protoSelf() { return new Shape2(1).self(); }

function Legacy() { this.k = 1; }
Legacy.prototype = { kind() { return 'legacy'; } };

export function protoReplaced() { return new Legacy().kind(); }

function Base3() {}
Base3.prototype.hello = function () { return 5; };
function Derived3() {}
Derived3.prototype = Object.create(Base3.prototype);

export function protoInherited() { return new Derived3().hello(); }

function plainThis() { return this; }

export function thisPlain() { return plainThis(); }

export function thisCall() { return plainThis.call(new Shape2(1)); }

export function thisHanded() {
  let seen;
  [1].forEach(function () { seen = this; });
  return seen;
}

function tagFn() { return 1; }
tagFn.label = 'x';

export function fnExpando() { return tagFn.label; }

export function arrowProto() { const a = () => 1; return a.prototype; }

// J2''': `this` of a function called as a property is the object whose property holds it, not every receiver.
const heldA = { tag: 1, run: function () { return this.tag; } };
const heldB = { tag: 'b', run: function () { return 5; } };
function pickHeld(flag) { return flag ? heldA : heldB; }

export function heldRun() { return pickHeld(true).run(); }

const heldC = { mark: null, put: function (v) { this.mark = v; } };
const heldD = { mark: null, put: function (v) { return v; } };
function pickPut(flag) { return flag ? heldC : heldD; }

export function heldWrite() { pickPut(true).put(7); return heldD.mark; }
