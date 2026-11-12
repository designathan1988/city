// The entry: imports by name, by alias, as a namespace and by default; a callback handed to a function of the
// program and to one the code read does not define; a function kept in an object and in a variable.
import { run as start, Shop } from "./lib/shop";
import * as tools from "./lib/tools";
import Crate from "./lib/crate";

const later = (n: number) => tools.sharpen(n);

export function main(): void {
  const shop = new Shop(new Crate());
  shop.open(1);
  start(later);
  const table = { go: tools.sharpen, hop() { return later(2); } };
  table.go(3);
  table.hop();
  [1, 2].forEach((n) => tools.polish(n));
  shop.bus.on("sold", onSold);
  shop.bus.emit("sold");
}

function onSold(): void { tools.polish(4); }

export function loose(thing): void { thing.lift(5); }
