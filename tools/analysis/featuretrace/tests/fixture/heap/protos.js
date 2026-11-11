// Prototypes and classes as objects (log/trace-core-plan.md, Task 2; ECMA-262: OrdinaryCreateFromConstructor,
// MakeConstructor, ClassDefinitionEvaluation, accessor properties).
class Leaf {}

class Base {
  constructor() { this.kept = new Leaf(); }
  hello() { return new Leaf(); }
  static make() { return new Base(); }
}

class Derived extends Base {
  constructor() { super(); }
  hello() { return super.hello(); }
}

class Guarded {
  get value() { return new Leaf(); }
  set value(v) { this.stored = v; }
}

function Legacy() { this.k = new Leaf(); }
Legacy.prototype.greet = function () { return this.k; };

function Replaced() {}
Replaced.prototype = { kind() { return new Leaf(); } };

export function inherited() { return new Derived().hello(); }

export function staticMade() { return Base.make(); }

export function superField() { return new Derived().kept; }

export function getterRead() { return new Guarded().value; }

export function setterWrite() { const g = new Guarded(); g.value = new Leaf(); return g.stored; }

export function legacyGreet() { return new Legacy().greet(); }

export function replacedKind() { return new Replaced().kind(); }

export function constructorOf() { const d = new Derived(); return new d.constructor(); }
