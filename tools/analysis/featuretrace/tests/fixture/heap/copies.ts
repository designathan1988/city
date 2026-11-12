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
