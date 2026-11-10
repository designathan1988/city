// What the feature uses of src/sim/meso/meso.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/meso/meso.ts:23-23 variable MESO_DT
/** THE MESO TRAFFIC IN THE SIMULATION: the world's flows (`model.ts`) over the lane graph's roads, and the micro window they hand their cars to and take them back from */
// The cost of a tick is bounded, not the map's size: a step's work (the rates, the model's phases, the publish) is spread over the
// ticks of its second, `MESO_WORK_PER_TICK` links a tick (Factorio's work in buckets, FFF #421); a step owed while one is under way
// waits for it (the far traffic slows, the tick does not grow). The page is sent only the links that changed (`MesoDelta`).

/** Seconds of the city between two steps of the meso model. */
export const MESO_DT = 1;

// src/sim/meso/meso.ts:37-37 variable HANDOFF_TRIES
/** Handoff lanes tried for a new car before it goes without a destination (its lookahead drives it, the window's edge takes it back). */
const HANDOFF_TRIES = 4;
