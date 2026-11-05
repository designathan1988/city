// What the feature uses of src/sim/drive/physicalMotion.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/drive/physicalMotion.ts:10-20 function physicalSpeed
/** Separate the driver's preferred standing gap from physical stopping room. */
export function physicalSpeed(p: DriverParams, speed: number, wanted: number, acceleration: number,
  obstacles: readonly Obstacle[], dt: number): number;
