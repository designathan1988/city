// An object written in place is an object of its own shape: a factory's object is called exactly; an entry is never
// what an object nothing is known of holds.
export interface Desk { stamp(): number; file(): number }

export function makeDesk(): Desk {
  function stamp(): number { return 1; }
  return { stamp, file: () => 2 };
}

export function office(): number {
  const d = makeDesk();
  return d.stamp() + d.file();
}

export const spare = { fetch: () => 3 };

export function stray(unknown: any): number {
  return unknown.fetch();
}

export function opts(): void {
  later({ onDone: finish });
}

function finish(): void {}
