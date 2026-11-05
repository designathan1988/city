// What the feature uses of src/lab/catalog/layers.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/catalog/layers.ts:39-39 type Add
/** THE LAYERS THAT STAND ON THE ROADS, each built by its own layer on a layout made here (no city): the signs (`signLayer.ts`), the barriers (`barriers.ts`). */

type Add = (spec: ItemSpec) => void;

// src/lab/catalog/layers.ts:70-74 function footwayOut
/** Where the footway of the road across runs (the document's first road): its middle, metres from the road's centre line. */
function footwayOut(net: Network): number;

// src/lab/catalog/layers.ts:76-94 function signItems
export function signItems(add: Add, cache: BatchCache): void;

// src/lab/catalog/layers.ts:77-77 function signItems.flat
const flat = (): number => …;

// src/lab/catalog/layers.ts:200-200 function farNetworkItems.flat
const flat = (): number => …;
