// What the feature uses of src/editor/smallCityMap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/smallCityMap.ts:18-18 variable GROWTH_SEED
/** The stream the city's buildings are grown from. */
const GROWTH_SEED = 0x5eed;

// src/editor/smallCityMap.ts:41-41 variable SPILL_AT
/** Records held as objects while the city grows, past which the older are spilled into its table (`SpilledItems.spill`). */
const SPILL_AT = 4096;

// src/editor/smallCityMap.ts:43-43 variable SPILL_KEEP
/** The newest records kept as objects at a spill: the block growing now reads them as its neighbours. */
const SPILL_KEEP = 1024;

// src/editor/smallCityMap.ts:52-68 function buildCityTable
/**
 * THE CITY AS THE PAGE OPENS IT, MADE WITHOUT HOLDING ITS BUILDINGS AS OBJECTS (`cityGen.worker.ts`): the very city `buildCityMap` makes
 * (the same records, in the same order) prepared as `prepareCity` prepares it, but its buildings spilled into tables as they grow - the
 * table the growth reads its older neighbours back from, and the page's (`PreparedCityWriter`). DO NOT grow the whole city as objects
 * and prepare it after: ~20 KB of JS heap a building, twice over while it was prepared, and the city of 160,000 buildings took its
 * worker to 5.6 GB, past the V8 cage the worker shares with its page (2026-10-22). `phase`: told as each step ends (the scale probes).
 */
export function buildCityTable(city: CityPlan, phase?: (name: string) => void): PreparedCity;

// src/editor/smallCityMap.ts:71-94 function growCity
/** A planned city laid out and grown on one document. `spill`: told after each building grown and once at the end (`last`), with the store's items spilled to a table (`BuildingStore.spillToTable`, its members shared by `shared`); null: every record kept as an object. */
function growCity(city: CityPlan, shared: SharedMembers | null, spill: ((items: SpilledItems, last: boolean) => void) | null, phase?: (name: string) => void): RoadDoc;

// src/editor/smallCityMap.ts:97-113 function growLots
/** Every open lot of the city grown, on the flat ground of a new map and the paving its streets lay on it; `grew`: told after each building. */
function growLots(doc: RoadDoc, net: Network, grew: (() => void) | null): void;

// src/editor/smallCityMap.ts:101-101 function growLots.pavedAt
const pavedAt = (x: number, y: number): number => …;
