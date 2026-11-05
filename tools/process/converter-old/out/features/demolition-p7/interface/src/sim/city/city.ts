// What the feature uses of src/sim/city/city.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/city/city.ts:12-42 class City
/** THE CITY'S OWN SERVICES, with nobody living in it: the time of day, read off the simulation's fixed clock */
export class City { … }

// src/sim/city/city.ts:14-14 field City.transit
/** Buses and trains on the drawn lines (`transit/transit.ts`). */
  readonly transit = new TransitSim();
