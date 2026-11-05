// What the feature uses of src/sim/city/city.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/city/city.ts:14-14 field City.transit
/** Buses and trains on the drawn lines (`transit/transit.ts`). */
  readonly transit = new TransitSim();

// src/sim/city/city.ts:21-23 method City.minutes
/** Minutes since midnight of the first day, from the simulation clock. */
  minutes(w: SimWorld): number;
