// What the feature uses of src/lab/catalog/layout.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/catalog/layout.ts:89-91 function road
/** A road of `type` from (x0, y0) to (x1, y1). */
function road(doc: RoadDoc, type: number, x0: number, y0: number, x1: number, y1: number, structure: RoadStructure = 'ground', direction: SegmentDirection = 'both'): void;

// src/lab/catalog/layout.ts:111-136 function surfacesOf
/** The roads of a document as the engine builds them: the surfaces in steps with each block's far level made as the road workers make it (`roadTile.worker.ts`, here on the page). */
async function surfacesOf(net: Network, materials: SceneMaterials, groundMaterial: Material, paint: Map<string, Material>): Promise<Group>;

// src/lab/catalog/layout.ts:112-112 function surfacesOf.flat
const flat = (): number => …;

// src/lab/catalog/layout.ts:166-166 function layoutItems.flat
const flat = (): number => …;
