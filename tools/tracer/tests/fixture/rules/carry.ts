// What code not read is given comes back from it: an element of a container, a callback's argument.
export class Crate {
  lift(): number { return 1; }
  weigh(): number { return 2; }
}

export function haul(crates: Crate[]): number {
  const bin = new Pile<number, Crate>();
  let n = bin.pick(1).lift();
  crates.forEach((c) => c.lift());
  n += crates.find((c) => c.weigh() > 0)!.weigh();
  return n;
}
