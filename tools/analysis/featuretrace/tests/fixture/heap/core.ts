// The heap analysis's core (log/trace-core-plan.md, Task 1): allocation sites, fields per object, calls, aliases.
export class Box {
  v: any = null;
  open(): number { return 1; }
}

export class Tile {
  lay(): number { return 2; }
}

export class Holder {
  box: Box = new Box();
  use() { return this.box.open(); }
}

export function viaLocal() { const b = new Box(); return b.open(); }

export function viaField() { return new Holder().use(); }

export function twoSites() { const a = new Box(); const b = new Box(); return a === b; }

function run(f: () => number) { return f(); }
function seven() { return 7; }

export function passed() { return run(seven); }

export function alias() {
  const o = { f: null as any };
  const p = o;
  p.f = new Tile();
  return o.f;
}

export function arrayAlias() {
  const a: any[] = [];
  const b = a;
  b[0] = new Tile();
  return a[0];
}

export function computedAlias(k: string) {
  const o: any = {};
  const p = o;
  p[k] = new Tile();
  return o.anything;
}

export function literalRead() { const o = { k: new Box() }; return o.k; }
export function indexKeepsNamesApart() {
  const o: any = { name: new Box() };
  for (let i = 0; i < 2; i++) o[i] = new Tile();
  return o.name;
}
export function indexReadBack() {
  const o: any = {};
  for (let i = 0; i < 2; i++) o[i] = new Tile();
  return o["0"];
}
export function textKeyMayNameAny(k: string) {
  const o: any = { name: new Box() };
  o[k] = new Tile();
  return o.name;
}
