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
  return o[k];
}

export function literalRead() { const o = { k: new Box() }; return o.k; }
class Bag { items: unknown[] = []; }
class Owned { owner = this; tag = { holder: this }; }
function wrap(v: unknown) { return { value: v }; }
export function bagBoxes() { const a = new Bag(); a.items[0] = new Box(); return a.items[0]; }
export function bagTiles() { const b = new Bag(); b.items[0] = new Tile(); return b.items[0]; }
export function ownerOf() { return new Owned().owner; }
export function holderOf() { return new Owned().tag.holder; }
export function wrapped() { return wrap(new Box()).value; }
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
export function unknownKeyKeepsNamesApart(k: string) {
  const o: any = { name: new Box() };
  o[k] = new Tile();
  return o.name;
}

// A module's code initializes each of its variables, read or not (ECMA-262 module evaluation); a call written in a
// literal inside a literal is a link of the property that holds it.
function tone() { return new Tile(); }
export const SHADES = { warm: { skin: tone() } };

// A method no call reaches runs on an object of its class, whether or not the code read makes one.
export class Crate {
  open() { return this.lid(); }
  lid() { return new Tile(); }
}

// A value of a class of the program that comes from outside (an entry point's parameter) is an instance the
// program made: what its constructor wrote is on it.
export class Depot {
  stock;
  constructor() { this.stock = new Tile(); }
}
export function builds() { return new Depot(); }
export function stocked(d: Depot) { return d.stock; }

// What an entry point returns is made, in its run: a closure it returns sees what it captured.
function peek(d: Depot) { return d.stock; }
export function watcher(d: Depot) { return () => peek(d); }

// The instance of a class the code read does not make is one object: what its constructor writes, its methods read.
interface Plug { fit(): Tile; }
export class Socket {
  constructor(private plug: Plug) {}
  use() { return this.plug.fit(); }
}

// `new` on what no declaration describes is an open call, as a call of it is: what it is handed is called back and
// it gives the unknown object.
declare const outside: any;
export function forged() { return new outside.Blade(() => new Tile()); }

// A position the program declares a type for takes only values assignable to it (TypeScript Type Compatibility:
// structural, a class's members included): a container holding anything still gives a typed parameter, and a typed
// return, only what fits; an object that gains the members later fits then.
class Pot { boil() { return new Tile(); } }
class Pan { fry() { return new Box(); } }
interface Lidded { lid: Tile; }
const shelf: any[] = [];
shelf[0] = new Pan();
shelf[1] = new Pot();
function keep(p: Pot): Pot { return p; }
export function kept() { return keep(shelf[0]); }
function handOut(): Pot { return shelf[0]; }
export function handed() { return handOut(); }
function lidOf(x: Lidded) { return x; }
export function lidded() { const later: any = {}; const r = lidOf(later); later.lid = new Tile(); return r; }
