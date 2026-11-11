// The platform from its declarations (log/trace-core-plan.md, Task 4): what a declared method does is what its
// signature says — its arguments into its receiver's type-parameter places, its callbacks called with them, its result
// from them or an object of its declared type; iteration and `await` by the language's protocols.
export class Apple {}
export class Pear {}

export function pushed() {
  const a: Apple[] = [];
  const b = a;
  b.push(new Apple());
  return a[0];
}

export function mappedValue() { return [new Apple()].map((x) => x)[0]; }
export function mappedResult() { return [new Apple()].map(() => new Pear())[0]; }

export function mapGet() {
  const m = new Map();
  m.set("k", new Apple());
  return m.get("k");
}

export function thenValue() {
  let got;
  Promise.resolve(new Apple()).then((v) => { got = v; });
  return got;
}

export async function awaited() { return await Promise.resolve(new Apple()); }
export async function executed() { return await new Promise((resolve) => resolve(new Apple())); }

export function iterated() {
  const s = new Set();
  s.add(new Apple());
  let last;
  for (const x of s) last = x;
  return last;
}

export function hosted() { return page.find("#a"); }
export function focused() {
  const e = page.find("#a");
  if (e) e.focus();
}

export function listened() {
  let seen;
  page.listen({ handle(e) { seen = e; } });
  return seen;
}
export function called() {
  let seen;
  page.listen((e) => { seen = e; });
  return seen;
}

export function onFocus(target: Element) { target.focus(); }

export function titled() { return page.title(); }
