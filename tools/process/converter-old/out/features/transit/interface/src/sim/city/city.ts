// What the feature uses of src/sim/city/city.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/city/city.ts:14-14 field City.transit
/** Buses and trains on the drawn lines (`transit/transit.ts`). */
  readonly transit = new TransitSim();

// src/sim/city/city.ts:36-41 method City.step
/** Stage 2 (`pipeline.ts`): the lines run; traffic comes in at the road ends */
  step(w: SimWorld): void;
