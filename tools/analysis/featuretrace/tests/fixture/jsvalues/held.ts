// A member call on a property that holds a function gives what that function returns (log/trace-rules.md, CALL'').
class Lidded {
  lid(): number { return 1; }
}

const makeLidded = (n: number) => new Lidded();

const holder = { make: makeLidded };

type Maker = (n: number) => Lidded;

interface Makes {
  readonly make: Maker;
}

export function heldCall() { return holder.make(1); }

export function heldChain() { return holder.make(1).lid(); }

export function heldTyped(m: Makes) { return m.make(2); }
