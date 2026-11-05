// What the feature uses of src/editor/roads/powerLine.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roads/powerLine.ts:12-12 variable SNAP_REACH
const SNAP_REACH = m(8);

// src/editor/roads/powerLine.ts:25-68 function layPowerLinesSteps
/** `layPowerLines` a few streets at a time: yields every `STREETS_A_STEP` segments and returns how many runs were built. */
export function* layPowerLinesSteps(doc: RoadDoc, net: Network, segments: Iterable<SegmentId>, plan?: PlanRun): Generator<void, number>;

// src/editor/roads/powerLine.ts:79-79 type PlanRun
/** How one street's run is planned: its two ends, with the generator's reach, spacing and lamps. */
export type PlanRun = (doc: RoadDoc, from: Vec2, to: Vec2) => PoleRunPlan;

// src/editor/roads/powerLine.ts:82-82 variable BATCH
/** Streets in one call past which they are planned over shared tiles (`poleRunPlanner`). */
const BATCH = 24;

// src/editor/roads/powerLine.ts:85-91 function runPlanner
/** The planning of a call's runs. */
function runPlanner(net: Network, streets: number): PlanRun;
