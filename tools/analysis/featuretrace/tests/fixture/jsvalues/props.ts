// Property names as values (P1-P3) and the types of the host (H1).
import { Box } from "./types";

type Point = { x: number };

const table = { a: new Box(), b: new Box() };

export function eachKey(): void {
	for (const k in table) table[k].open();
}

export function keysOf(): void {
	for (const k of Object.keys(table)) table[k].open();
}

export function valuesOf(): void {
	for (const v of Object.values(table)) v.open();
}

export function entriesOf(): void {
	for (const [k, v] of Object.entries(table)) v.open();
}

export function draw(c: HTMLCanvasElement, p: Point): number {
	c.getContext("2d");
	return p.x;
}
