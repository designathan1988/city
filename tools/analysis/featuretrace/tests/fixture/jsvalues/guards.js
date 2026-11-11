// Values through branch conditions (log/trace-rules.md, "Narrowing at branch conditions"): the language's constants
// (V1), properties written where no class declares them (V2), guards and the regions they hold in (G1, R1, R2, N1).
import { Box } from "./types";
import { Tile } from "./helpers";

// V1: `true`, `false`, `null` and `undefined` are values of their own.
export function yes() { return true; }
export function none() { return null; }
export function gone() { return undefined; }
export function lengthOfYes() { return yes().length; }

// V2: a property written where no class declares it is the class's; `C.prototype` is C's objects.
export class Flagged {}
Flagged.prototype.flag = true;
export function flagOf() { return new Flagged().flag; }
const bag = {};
bag.extra = new Box();
export function useBag() { bag.extra.open(); }
export class Plain { a = 1; }
export function missing() { return new Plain().b; }

// G1, R1, R2, N1: a name read inside an `if` holds what the condition lets through.
export class V3 { x = 0; copy(v) { this.x = v.x; } set(x) { this.x = x; } }
V3.prototype.isV3 = true;
export function look(x) { const t = new V3(); if (x.isV3) { t.copy(x); } else { t.set(x); } }
look(new V3());
look(5);
export function pickBranch(c) { if (c instanceof Box) c.open(); else c.lay(); }
pickBranch(new Box());
pickBranch(new Tile());
export function branchValue(c) { if (c instanceof Box) {} else { return c; } }
branchValue(new Box());
branchValue(new Tile());
export function kept(c) { if (c instanceof Box) { c = new Tile(); c.lay(); } }
kept(new Box());
export function laterWrite(c) { if (c instanceof Box) { c.lay(); } const swap = () => { c = new Tile(); }; swap(); }
laterWrite(new Box());
export function nested(c) { if (c instanceof Box) { if (c.open) { return c; } } }
nested(new Box());
nested(new Tile());
export function keyed(x) { if (x.open) return x; return null; }
export function useKeyed() { return keyed(new Tile()); }
keyed(new Box());

// R1 complete: after an `if` that leaves, `?:`, `&&`, `||`, loops, `switch`; G1 complete.
export class Can { open() {} }
export function early(c) { if (!(c instanceof Box)) return; return c; }
early(new Box());
early(new Can());
export function ternary(c) { return c instanceof Box ? c : null; }
ternary(new Box());
ternary(new Tile());
export function andRight(c) { return c instanceof Tile && c; }
andRight(new Box());
andRight(new Tile());
export function orRight(c) { return c instanceof Box || c; }
orRight(new Box());
orRight(new Tile());
export function textOnly(c) { if (typeof c === "string") return c; return null; }
textOnly("s");
textOnly(new Box());
export function notNull(c) { if (c !== null) return c; return undefined; }
notNull(null);
notNull(new Box());
export function looseNull(c) { if (c != null) return c; return undefined; }
looseNull(null);
looseNull(undefined);
looseNull(new Box());
export function hasOpen(c) { if ("open" in c) return c; return null; }
hasOpen(new Box());
hasOpen(new Tile());
export function arrayOnly(c) { if (Array.isArray(c)) return c; return null; }
arrayOnly([1]);
arrayOnly(new Box());
const boxShape = { kind: "box", go() {} };
const canShape = { kind: "can", go() {} };
export function discriminant(s) { if (s.kind === "box") return s; return null; }
discriminant(boxShape);
discriminant(canShape);
export function bySwitch(s) { switch (s.kind) { case "box": return s; case "can": return null; } }
bySwitch(boxShape);
bySwitch(canShape);
export function looped(c) { while (c instanceof Box) { return c; } return null; }
looped(new Box());
looped(new Tile());
export function notBoth(c) { if (!(c instanceof Box && c.open)) { return c; } return null; }
notBoth(new Box());
notBoth(new Tile());
export function optional(x) { if (x?.isV3) return x; return null; }
optional(new V3());
optional(new Tile());
export function optionalElse(x) { if (x?.isV3) return null; return x; }
optionalElse(new V3());
optionalElse(new Tile());
export function unknownTest(c, t) { if (typeof c === t) return c; return null; }
unknownTest(new Box(), "object");
unknownTest(new Tile(), "object");

// An unknown value that passes `instanceof C` is a C (N1).
export function hostValue(e) { if (e instanceof Box) return e; return null; }
hostValue(window.thing);
