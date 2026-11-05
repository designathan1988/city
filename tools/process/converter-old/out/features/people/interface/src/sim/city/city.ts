// What the feature uses of src/sim/city/city.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/city/city.ts:12-42 class City
/** THE CITY'S OWN SERVICES, with nobody living in it: the time of day, read off the simulation's fixed clock */
export class City { … }

// src/sim/city/city.ts:14-14 field City.transit
/** Buses and trains on the drawn lines (`transit/transit.ts`). */
  readonly transit = new TransitSim();

// src/sim/city/city.ts:16-16 field City.lots
/** Cars driving in and out of the lots through their car gates (`agents/lotTraffic.ts`). */
  readonly lots = new LotTraffic();

// src/sim/city/city.ts:21-23 method City.minutes
/** Minutes since midnight of the first day, from the simulation clock. */
  minutes(w: SimWorld): number;

// src/sim/city/city.ts:36-41 method City.step
/** Stage 2 (`pipeline.ts`): the lines run; traffic comes in at the road ends */
  step(w: SimWorld): void;
