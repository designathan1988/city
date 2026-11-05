// What the feature uses of src/sim/roads/flowStats.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/roads/flowStats.ts:5-5 variable TAU
/** DO NOT age every leg ever seen every second */
const TAU = ROAD_TUNING.flow.trendWindow / 3;

// src/sim/roads/flowStats.ts:7-7 variable KEEP
/** What a second with nothing counted leaves of a rate. */
const KEEP = 1 - 1 / TAU;

// src/sim/roads/flowStats.ts:10-10 interface Rate
/** A leg's rate as it stood at the end of second `at`. */
interface Rate { rate: number; at: number }

// src/sim/roads/flowStats.ts:35-55 method FlowStats.step
step(dt: number): void;

// src/sim/roads/flowStats.ts:94-97 function agedTo
/** A rate brought up to the end of `second`: a factor `KEEP` for each second since it was kept, none counted. */
function agedTo(r: Rate, second: number): number;
