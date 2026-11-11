// A closure is the function with the values its maker's call bound (log/trace-rules.md, C4, C4'): each memo runs its
// own function, each made function holds its own captured values and its own properties.
class Apple {}
class Pear {}

function memo(compute) {
  return (key) => compute(key);
}

const appleOf = memo(() => new Apple());
const pearOf = memo(() => new Pear());

export function memoApple() { return appleOf(1); }

export function memoPear() { return pearOf(1); }

function alias(p) {
  const q = p;
  return () => q;
}
const aliasA = alias(new Apple());
const aliasP = alias(new Pear());

export function aliasApple() { return aliasA(); }

function outer(p) {
  return () => () => p;
}
const nestedA = outer(new Apple());
const nestedP = outer(new Pear());

export function nestedApple() { return nestedA()(); }

function plain() {
  return () => 3;
}

export function plainValue() { return plain(); }

// A maker with two parameters its closure reads (C2' meets C4'): the call still gives the closure.
function pair(a, b) {
  return () => [a, b];
}
const pairAP = pair(new Apple(), new Pear());

export function pairMade() { return pairAP; }

export function pairRun() { return pairAP(); }

// A closure handed back to its own maker (`spin(wrap(f), n - 1)`): the maker's templates are keyed by the function a
// closure is, never by the template that made it, so they are finite (C4').
function wrap(f) {
  return () => f();
}
function spin(f, n) {
  return n > 0 ? spin(wrap(f), n - 1) : f;
}

export function spunRun() { return spin(() => new Apple(), 3)(); }

// A property written on a closure in the function that makes it is that closure's own (C4'); `call` and `bind` run a
// closure in its environment.
function tagged(x) {
  const fn = () => 1;
  fn.tag = x;
  return fn;
}
const taggedA = tagged(new Apple());
const taggedP = tagged(new Pear());

export function tagApple() { return taggedA.tag; }

export function callApple() { return appleOf.call(null, 1); }

export function bindApple() { return appleOf.bind(null)(1); }
