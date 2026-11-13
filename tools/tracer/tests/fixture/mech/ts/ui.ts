// Handlers given through a tagged template and written into the platform's object; an object carrying an object of
// functions; a method bound and called; a function nothing calls; a call of which nothing is known.
export function panel(go: () => void) {
  const press = (): void => { go(); };
  return markup`<button @click=${press}>x</button>`;
}

export function alarm(): void {
  const bell = new Chime("x");
  bell.onclick = () => { ring(); };
}

function ring(): void {}

function host() {
  const hooks = { shout: () => ring() };
  return { hooks, count: 0 };
}

export function use(): void {
  const h = host();
  h.hooks.shout();
}

export class Clock { tick(): void {} }

export function later(c: Clock): void {
  const f = c.tick.bind(c);
  f();
}

function lonely(): number { return 1; }

export function blind(thing): void {
  thing.whirl();
}
