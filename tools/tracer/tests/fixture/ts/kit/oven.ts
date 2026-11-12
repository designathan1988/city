// A class with a getter, a setter and a static maker; a namespace.
export class Gauge {
  read(): number { return 1; }
  reset(): void {}
}

export class Oven {
  get heat(): Gauge { return new Gauge(); }
  set heat(gauge: Gauge) { gauge.reset(); }
  static make(): Oven { return new Oven(); }
}

export namespace Tools {
  export function wipe(): number { return 0; }
}
