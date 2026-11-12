// A string is a value of its own text (log/trace-research.md F33; ECMA-262 ToPropertyKey): a key computed from a
// string names the member of that text, wherever the string came from.
export class Apple {}
export class Pear {}
export class Plum {}

type Part = "a" | "b";
declare function part(): Part;

export function viaKeys() {
  const o: any = { a: new Apple(), b: new Pear(), c: new Plum() };
  let out: unknown = null;
  for (const k of ["a", "b"]) out = o[k];
  return out;
}

function pick(o: any, k: string) { return o[k]; }

export function viaArgument() { return pick({ a: new Apple(), b: new Pear() }, "a"); }

function put(o: any, k: string, v: unknown) { o[k] = v; }

export function written() {
  const o: any = { a: null, b: null };
  put(o, "a", new Apple());
  return o.b;
}

export function writtenNamed() {
  const o: any = { a: null, b: null };
  put(o, "a", new Apple());
  return o.a;
}

export function declaredKey() {
  const o: any = { a: new Apple(), b: new Pear(), c: new Plum() };
  return o[part()];
}

export function numbered() {
  const list: unknown[] = [];
  const key = "0";
  list[key] = new Apple();
  return list[0];
}
