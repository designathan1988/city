// TypeScript annotations as the values they allow (T1-T3), the language's containers (T2) and operators (O1).
import { moreBoxes } from "./other";

export class Building { level(): number { return 1; } }
export class Box { open(): void {} }

export function levelOf<T>(b: Building | null, t: T, n: number): number {
	return b ? b.level() + n : 0;
}

export function boxes(): Box[] { return []; }

export function walk(): void {
	for (const x of boxes()) x.open();
}

export function lookup(map: Map<string, Box>, key: string): void {
	map.get(key)!.open();
}

export function entries(map: Map<string, Box>): void {
	for (const [k, v] of map) v.open();
}

export function eachValue(map: ReadonlyMap<string, Box>): void {
	map.forEach((v) => v.open());
}

export async function later(p: Promise<Box>): Promise<void> {
	(await p).open();
}

export function scaled(k: Box): number {
	const n = k * 2;
	return n;
}

export function allBoxes(f: boolean) {
	return f ? boxes() : moreBoxes();
}

// A computed property: the members its key's strings name; an unknown key reads none (T4').
export class Table { a = new Box(); b = 1; method(): void {} }

export function pick(t: Table): void {
	const key = "a";
	t[key].open();
}

export function anyKey(t: Table, k: string) {
	const v = t[k];
	return v;
}
