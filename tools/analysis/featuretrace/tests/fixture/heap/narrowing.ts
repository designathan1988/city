// Narrowing in the new core (log/trace-research.md F31): a name read where a test proved something of its value keeps
// only the values the test can be so of (ECMA-262 typeof, ToBoolean, IsStrictlyEqual, instanceof, `in`, IsArray).
export class Apple { kind = "apple"; }
export class Pear { kind = "pear"; }
export class Seed extends Apple {}
export class Anything { static [Symbol.hasInstance](v: unknown) { return v !== undefined; } }

declare const flag: boolean;

export function typeofObject() {
  let v: unknown = new Apple();
  if (flag) v = () => new Pear();
  if (flag) v = 3;
  if (flag) v = null;
  if (typeof v === "object" && v !== null) return v;
}

export function typeofFunction() {
  let v: unknown = new Apple();
  if (flag) v = () => new Pear();
  if (typeof v === "function") return v;
}

export function typeofString() {
  let s: unknown = "a";
  if (flag) s = 1;
  if (flag) s = new Apple();
  if (typeof s === "string") return s;
}

export function falsy() {
  let w: Apple | null = new Apple();
  if (flag) w = null;
  if (!w) return w;
}

export function truthy() {
  let w: Apple | null = new Apple();
  if (flag) w = null;
  if (w) return w;
}

export function nullish() {
  let w: Apple | null = new Apple();
  if (flag) w = null;
  if (w == null) return w;
}

export function notNullish() {
  let w: Apple | null = new Apple();
  if (flag) w = null;
  if (w != null) return w;
}

export function strictNull() {
  let w: Apple | null = new Apple();
  if (flag) w = null;
  if (w === null) return w;
}

export function sameText() {
  let t: unknown = "a";
  if (flag) t = 1;
  if (flag) t = new Apple();
  if (t === "a") return t;
}

export function instance() {
  let x: Apple | Pear = new Apple();
  if (flag) x = new Pear();
  if (flag) x = new Seed();
  if (x instanceof Apple) return x;
}

export function notInstance() {
  let x: Apple | Pear = new Apple();
  if (flag) x = new Pear();
  if (flag) x = new Seed();
  if (!(x instanceof Apple)) return x;
}

export function ownInstanceTest() {
  let y: unknown = 3;
  if (flag) y = new Pear();
  if (y instanceof Anything) return y;
}

export function isArray() {
  let a: unknown = [new Apple()];
  if (flag) a = new Pear();
  if (Array.isArray(a)) return a;
}

export function notArray() {
  let a: unknown = [new Apple()];
  if (flag) a = new Pear();
  if (!Array.isArray(a)) return a;
}

export function has() {
  let h: any = new Apple();
  if (flag) h = 3;
  if ("kind" in h) return h;
}

export function lacks() {
  let h: any = new Apple();
  if (flag) h = 3;
  if (!("kind" in h)) return h;
}

export function tagged() {
  let s = { open: true, v: new Apple() };
  if (flag) s = { open: false, v: new Pear() };
  if (s.open === true) return s.v;
}

export function otherTag() {
  let s = { open: true, v: new Apple() };
  if (flag) s = { open: false, v: new Pear() };
  if (s.open !== true) return s.v;
}

export function ready() {
  let m = { ready: true, v: new Apple() };
  if (flag) m = { ready: false, v: new Pear() };
  if (m.ready) return m.v;
}

export function either() {
  let n: Apple | null = null;
  if (flag) n = new Apple();
  return n || new Pear();
}

function isApple(x: unknown): x is Apple { return x instanceof Apple; }

export function predicated() {
  let p: unknown = new Apple();
  if (flag) p = 3;
  if (flag) p = new Pear();
  if (isApple(p)) return p;
}

function mustBeApple(x: unknown): asserts x is Apple { if (!(x instanceof Apple)) throw new Error(); }

export function asserted() {
  let q: unknown = new Apple();
  if (flag) q = 3;
  mustBeApple(q);
  return q;
}
