// Contexts and heap cloning (log/trace-core-plan.md, Task 3): each call of a factory makes its own closure and its own
// objects; a method keeps each receiver's fields apart; recursion through closures ends; a closure's write is the
// shared variable's; a callback no call reaches reads the run that made it; each function object has its own prototype.
class Apple {}
class Pear {}

function memo(compute) {
  return (key) => compute(key);
}
const appleOf = memo(() => new Apple());
const pearOf = memo(() => new Pear());

export function memoApple() { return appleOf(1); }

class Node {
  constructor(f) { this.f = f; }
  run() { return this.f(); }
}
function Fn(f) {
  const node = new Node(f);
  const fn = () => node.run();
  fn.node = node;
  return fn;
}
const applyApple = Fn(() => new Apple());
const applyPear = Fn(() => new Pear());

export function factoryApple() { return applyApple(); }

class Holder {
  constructor(v) { this.v = v; }
  get() { return this.v; }
}

export function receiverApple() {
  const a = new Holder(new Apple());
  const p = new Holder(new Pear());
  p.get();
  return a.get();
}

function wrap(f) { return () => f(); }
function spin(f, n) { return n > 0 ? spin(wrap(f), n - 1) : f; }

export function spun() { return spin(() => new Apple(), 3)(); }

function shared() {
  let held = null;
  const put = (v) => { held = v; };
  put(new Apple());
  return held;
}
export function sharedApple() { return shared(); }

const handlers = {};
function listen(made) {
  const box = new Holder(made);
  function onEvent() { return box.get(); }
  handlers.last = onEvent;
}
listen(new Apple());

function makeKind(make) {
  function Kind() {}
  Kind.prototype.build = make;
  return Kind;
}
const AppleKind = makeKind(() => new Apple());
const PearKind = makeKind(() => new Pear());

export function kindApple() { return new AppleKind().build(); }

function holderOf(v) { return { get() { return v; } }; }
export function literalApple() { holderOf(new Pear()); return holderOf(new Apple()).get(); }

function boxOf(v) {
  class Box { get() { return v; } }
  return new Box();
}
export function localApple() { boxOf(new Pear()); return boxOf(new Apple()).get(); }
function kindOf(v) { return class { get() { return v; } }; }
export function classValueApple() { kindOf(new Pear()); const K = kindOf(new Apple()); return new K().get(); }

function area(w, h) { return w * h; }
function same(x) { return x; }
function keep(into, value) { into.value = value; }
export function sized() { return area(new Apple(), 2) + area(new Pear(), 3); }
export function both() { same(new Pear()); keep({}, new Pear()); keep({}, new Apple()); return same(new Apple()); }

const wrapped = (x) => [x];
export function arrowAsMethod(flag) {
  const a = { run: wrapped }, b = { run: wrapped };
  const o = flag ? a : b;
  return o.run(new Apple());
}

// A function whose result carries what a parameter holds runs once per object of it (Wei & Ryder, ECOOP 2015, 1st-
// parameter sensitivity; Agesen's CPA): through a wrapper that calls it from one place, each caller still gets back a
// copy of its own object.
function copyOf(r) { return { ...r }; }
const at = (r) => copyOf(r);
export function wrappedApple() { at({ a: new Pear() }); return at({ a: new Apple() }).a; }
