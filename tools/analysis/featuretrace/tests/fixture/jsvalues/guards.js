// Values through branch conditions (log/trace-rules.md, "Narrowing at branch conditions"): the language's constants
// (V1), properties written where no class declares them (V2), guards and the regions they hold in (G1, R1, R2, N1).
import { Box } from "./types";

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
