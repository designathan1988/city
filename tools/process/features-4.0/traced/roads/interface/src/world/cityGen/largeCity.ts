// What the feature uses of src/world/cityGen/largeCity.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cityGen/largeCity.ts:7-7 variable LARGE_CITY_RADIUS
/** THE LARGE CITY: the small city of Roadcraft 2.0 (`smallCity.ts`) made bigger - the same plan, the same 'blocks' style, the same straight grid, zones and squares, over a larger radius. Every block is built; no block is left as bare grass. */

/** The large city's radius: its grid spans 1.7 radii, about 2.7 km across (the small city's 1.1 km). */
export const LARGE_CITY_RADIUS = m(1_600);

// src/world/cityGen/largeCity.ts:10-12 function largeCityPlan
/** The large city of a seed (made once a city is asked for, `cityGen.worker.ts`: nothing kept between them); `radius`: the same city over another radius, its density kept (the scale steps: ~2.8 km for ~25 000 buildings, ~4.7 km for ~70 000, ~8.85 km for ~200 000 on the 18-tile map). */
export function largeCityPlan(seed: number, radius: number = LARGE_CITY_RADIUS): CityPlan;
