// Type predicates and assertion functions (G1; TypeScript handbook, Narrowing: `x is T`, `asserts x is T`).
import { Box } from "./types";
import { Can } from "./guards";

export function isBox(x: Box | Can): x is Box { return x instanceof Box; }
export function assertBox(x: Box | Can): asserts x is Box { if (!(x instanceof Box)) throw new Error("no box"); }
export function viaPredicate(v: Box | Can) { if (isBox(v)) return v; return null; }
export function viaAssertion(v: Box | Can) { assertBox(v); return v; }
