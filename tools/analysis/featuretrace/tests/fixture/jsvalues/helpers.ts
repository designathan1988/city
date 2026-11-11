// Call results per argument values (C1, C2): helpers whose result depends on their arguments.
import { Box } from "./types";

export class Tile { lay(): void {} }

function same<T>(x: T): T { return x; }
function inner(x: { v: Box }) { return x.v; }
function wrapped(x: Box) { return same(x); }
function constant(x: Box) { return 1; }
function maker(x: Box) { return () => x; }

export function useBox(): void { same(new Box()).open(); }
export function useTile(): void { same(new Tile()).lay(); }
export function label() { return same("a"); }
export function deep(): void { wrapped(new Box()).open(); }

function down(n: number, x: Box): Box { return n > 0 ? down(n - 1, x) : x; }
export function recursive(): void { down(3, new Box()).open(); }

// Two key parameters: each one's templates, and the call keeps what both give (C2').
function either(a: Box | Tile, b: Box | Tile, f: boolean) { return f ? a : b; }
export function pairBox(f: boolean): void { either(new Box(), new Box(), f).open(); }
export function pairTile(f: boolean): void { either(new Tile(), new Tile(), f).open(); }
