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
export function keyed(x) { if (x instanceof Box) return x; return null; }
export function useKeyed() { return keyed(new Tile()); }
keyed(new Box());
