// What the feature uses of src/sim/meso/memory.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/meso/memory.ts:7-7 variable ML
/** THE MESO TRAFFIC AS THE PAGE READS IT: the worker sends only the links whose numbers changed (`MesoDelta`, its buffers handed over, not copied); the page keeps its own copy of every link (`MesoMemory`) */

/** A link's record (Float32), by slot (the meso's own dense number, `meso.ts`): its numbers of the step, then its road's start, middle and end (x, y: drawn straight from afar)
 *  and `shape`, a serial moved on each time the slot's link or its line changes (the page writes its line again then; 0 in a delta: the line did not change). */
export const ML = { count: 0, queued: 1, speed: 2, length: 3, lanes: 4, flags: 5, ax: 6, ay: 7, mx: 8, my: 9, bx: 10, by: 11, shape: 12 } as const;

// src/sim/meso/memory.ts:8-8 variable MESO_RECORD
export const MESO_RECORD = 13;
