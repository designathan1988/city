// A class with a typed field, a parameter property, a base class and an overridden method; a function taking a
// callback and calling it.
import Crate from "./crate";
import { sharpen } from "./tools";

class Counter {
  visit(): number { return this.greet(); }
  greet(): number { return 0; }
}

export class Shop extends Counter {
  bus = makeBus();
  constructor(private stock: Crate) { super(); }
  open(a: number): number { return this.stock.lift(a); }
  greet(): number { return sharpen(1); }
}

declare function makeBus(): any;

export function run(cb: (n: number) => number): number { return cb(7); }
