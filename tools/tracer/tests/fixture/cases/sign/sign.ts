// Sibling lamps lit from one distance by two thresholds read from a data file: a range lights neither. A pair lit by
// a test and its opposite: exactly one, always. A share computed by a division.
declare function fetchText(path: string): string;

const tune = JSON.parse(fetchText("conf://cases/sign/tune.json"));
let dist = 0;
let flag = false;

class Lamp { shown = false; }
class Glow { shown = false; }
class Port { lit = false; }
class Dock { lit = false; }
const lamp = new Lamp();
const glow = new Glow();
const port = new Port();
const dock = new Dock();

export function move(step: number): void { dist = step; }
export function toggle(on: boolean): void { flag = on; }
export function near(): void { lamp.shown = dist < tune.near; }
export function far(): void { glow.shown = dist > tune.near + tune["margin"]; }
export function pair(): void {
  port.lit = flag;
  dock.lit = !flag;
}

class Pot { share = 0; }
export function split(p: Pot, total: number): void {
  if (total / 3 > 2) p.share = total;
}
