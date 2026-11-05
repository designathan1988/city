// What the feature uses of src/sim/agents/lotTraffic.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lotTraffic.ts:49-49 variable KINDS
/** The kinds of place a bay's building is (`ambient/demand.ts` kindOf), each a class of the draws; the last class, a building gone (weighs nought). */
const KINDS: readonly (keyof Mix)[] = ['home', 'shop', 'work', 'civic'];

// src/sim/agents/lotTraffic.ts:50-50 variable GONE
const GONE = KINDS.length;

// src/sim/agents/lotTraffic.ts:715-718 function kindClass
/** A building's class in the draws: its kind of place (`KINDS`), or `GONE`. */
function kindClass(w: SimWorld, building: BuildingId): number;
