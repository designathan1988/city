// Copies of an object's properties, by name (log/trace-research.md F34; ECMA-262 CopyDataProperties for an object
// spread, TypeScript's intersection `T & U` for a declared copy such as an assign): each property into the property of
// the same name, never into every property of the target.
export class Apple {}
export class Pear {}

declare function assign<T, U>(target: T, source: U): T & U;

export function spread() {
  const a = { x: new Apple(), y: new Pear() };
  const b = { ...a };
  return b.x;
}

export function spreadThenOwn() {
  const a = { x: new Apple() };
  const b = { ...a, y: new Pear() };
  return b.y;
}

export function chained() {
  const a = { x: new Apple() };
  const b = { ...a };
  const c = { ...b };
  return c.x;
}

export function spreadWritten() {
  const a: any = {};
  a.x = new Apple();
  const b = { ...a };
  return b.x;
}

export function assigned() {
  const t = { y: new Pear() };
  const r = assign(t, { x: new Apple() });
  return r.x;
}

export function assignedOther() {
  const t = { y: new Pear() };
  const r = assign(t, { x: new Apple() });
  return r.y;
}

// `for (k in src) dst[k] = v`: k is the name of a property src has (ECMA-262 EnumerateObjectProperties), so the write
// makes that property of dst, never any other.
export function enumerated() {
  const src = { x: new Apple() };
  const dst: any = {};
  for (const k in src) dst[k] = src[k];
  return dst.x;
}

export function enumeratedOther() {
  const src = { x: new Apple() };
  const dst: any = {};
  for (const k in src) dst[k] = src[k];
  return dst.y;
}

export function enumeratedLater() {
  const src: any = {};
  const dst: any = {};
  for (const k in src) dst[k] = new Pear();
  src.z = new Apple();
  return dst.z;
}

// `o[k] = v` with a key no text names: v is in o as a dictionary (what `o[k]` reads), no property a name reads
// (log/trace-research.md F40) — unless o's declared type says every name reads it (a string index signature).
export function byUnknownKey(k: string) {
  const o: any = {};
  o[k] = new Apple();
  return o.x;
}

export function byUnknownKeyRead(k: string) {
  const o: any = {};
  o[k] = new Apple();
  return o[k];
}

declare const table: { [name: string]: Pear };

export function indexedByName() { return table.row; }

// A write whose value is read with the write's own key (Sridharan et al., ECOOP 2012, correlated accesses): the
// value for each name is the one read with that name, never every value under every name.
export function cloneByKey() {
  const src = { a: new Apple(), b: new Pear() };
  const dst: any = {};
  for (const k in src) dst[k] = src[k];
  return dst.a;
}

export function cloneThroughLocal() {
  const src = { a: new Apple(), b: new Pear() };
  const dst: any = {};
  for (const k in src) { const v = src[k]; dst[k] = v; }
  return dst.b;
}
