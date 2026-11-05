// What the feature uses of src/lab/catalog/layers.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/catalog/layers.ts:39-39 type Add
/** THE LAYERS THAT STAND ON THE ROADS, each built by its own layer on a layout made here (no city): the signs (`signLayer.ts`), the barriers (`barriers.ts`). */

type Add = (spec: ItemSpec) => void;

// src/lab/catalog/layers.ts:118-134 function signalItems
/** The traffic signals of a signalised four-way junction as the signal layer stands them (`signals.ts`): a feed as the simulation gives it, its two groups (one
 *  each way across), one stage, every head green. */
export function signalItems(add: Add, cache: BatchCache): void;
