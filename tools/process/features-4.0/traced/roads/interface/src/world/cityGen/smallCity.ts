// What the feature uses of src/world/cityGen/smallCity.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cityGen/smallCity.ts:5-5 variable plans
/** THE SMALL CITY OF ROADCRAFT 2.0: one round city at the middle of the map, planned as 2.0 planned it (`planCity` with 2.0's defaults: the 'blocks' style, a straight rectangular grid, medium size). Everything follows from the seed. */

const plans = new Map<number, CityPlan>();

// src/world/cityGen/smallCity.ts:8-12 function smallCityPlan
/** The city of a seed, planned once. */
export function smallCityPlan(seed: number): CityPlan;
