// Guesses by name: narrowed by the arguments' count and the members read beside; too many and the call is ambiguous.
export class Rotor { spin(a: number, b: number): number { return a + b; } brake(): void {} }
export class Fan1 { spin(): number { return 1; } }
export class Fan2 { spin(): number { return 2; } }
export class Fan3 { spin(): number { return 3; } }
export class Fan4 { spin(): number { return 4; } }
export class Fan5 { spin(): number { return 5; } }
export class Fan6 { spin(): number { return 6; } }
export class Fan7 { spin(): number { return 7; } }
export class Fan8 { spin(): number { return 8; } }
export class Fan9 { spin(): number { return 9; } }
export class Top1 { whirl(): number { return 1; } brake(): void {} }
export class Top2 { whirl(): number { return 2; } }

export function loose(thing: any): number {
  thing.brake();
  return thing.whirl();
}

export function sized(thing: any): number {
  return thing.spin(1, 2);
}

export function many(thing: any): number {
  return thing.spin();
}
