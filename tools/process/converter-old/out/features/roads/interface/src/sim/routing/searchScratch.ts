// What the feature uses of src/sim/routing/searchScratch.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/routing/searchScratch.ts:10-176 class SearchScratch
/**
 * The working memory of the trip searches (`planTrip`, `routeToDestination`), one per world and kept between searches (SIM-01):
 * every lane and movement a search meets gets a dense slot, and the costs, parents and bounds of a search live in typed arrays
 * under a generation stamp, so a search allocates nothing. A search's state is `slot * 2 + changed`.
 * DO NOT change the order the heap gives states in: equal `f` falls back to the key the string search compared
 * (`lane`, or `lane|c` for a state reached by a lane change), and the routes the drivers take depend on it.
 */
export class SearchScratch { … }

// src/sim/routing/searchScratch.ts:11-11 field SearchScratch.slots
private readonly slots = new Map<LaneletId, number>();

// src/sim/routing/searchScratch.ts:13-13 field SearchScratch.ids
/** The id of each slot. */
  readonly ids: LaneletId[] = [];

// src/sim/routing/searchScratch.ts:52-61 method SearchScratch.slot
slot(id: LaneletId): number;
