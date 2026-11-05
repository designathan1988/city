// What the feature uses of src/sim/agents/walk.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/walk.ts:26-26 variable BODY
/** A body's radius: two people closer than twice this, side by side, touch. */
const BODY = m(0.27);

// src/sim/agents/walk.ts:32-32 variable LOOK
/** How far ahead others are looked for. */
const LOOK = m(8);

// src/sim/agents/walk.ts:834-834 interface Zebra
/** A crossing on a walker's way: a zebra as the vehicles and the signals know it (`edge`), or a crossing with no zebra */
interface Zebra { readonly id: CrossingId; readonly edge: SidewalkEdge | null; readonly way: Walkway }

// src/sim/agents/walk.ts:1451-1456 function addOther
function addOther(along: number, lat: number, oncoming: boolean, r: number, waitFor: Zebra | null, facing: boolean): void;

// src/sim/agents/walk.ts:1538-1880 function stepWalkers
function stepWalkers(w: SimWorld): void;
