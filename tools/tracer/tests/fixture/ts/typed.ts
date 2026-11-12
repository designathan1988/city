// A class below a base the code read does not define; functions given an object of a type it does not define, of a
// type's parameter and of an alias.
import Crate from "./lib/crate";
import { run } from "./lib/shop";

type Box = Crate;

class Winch extends Machine {
  lift(a: number): number { return a; }
}

export function drift(v: Vec): number { return v.lift(1); }
export function hoist<T>(any: T): number { return any.lift(3); }
export function boxed(box: Box): number { return box.lift(4); }
export function recast(thing: unknown): number { return (thing as Crate).lift(5); }

function onA(): void {}
function onB(): void {}
export function choose(flag: boolean): void {
  const pick = flag ? onA : (onB as () => void);
  register(pick || onB);
}
const queue: Array<() => void> = [];
const table: number[][] = [];
function byFirst(a: number[], b: number[]): number { return a[0] - b[0]; }
export function enqueue(): void { queue.push(onA); }
export function flush(): void {
  for (const job of queue) job();
  queue[0]();
}
export function sorted(): void { table.sort(byFirst); }
export function listed(): void {
  for (const row of table) register(row);
}
export function again(): number { return run(byFirst as never); }
export function glance(thing): void {
  register(thing.lift);
}
export function tally(): void {
  register(`${onA}` + onB);
}
