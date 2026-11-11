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
