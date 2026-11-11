// Values through branch conditions (log/trace-rules.md, "Narrowing at branch conditions"): the language's constants
// (V1), properties written where no class declares them (V2), guards and the regions they hold in (G1, R1, R2, N1).
import { Box } from "./types";

// V1: `true`, `false`, `null` and `undefined` are values of their own.
export function yes() { return true; }
export function none() { return null; }
export function gone() { return undefined; }
export function lengthOfYes() { return yes().length; }
