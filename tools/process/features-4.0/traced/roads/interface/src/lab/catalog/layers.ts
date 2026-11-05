// What the feature uses of src/lab/catalog/layers.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/catalog/layers.ts:39-39 type Add
/** THE LAYERS THAT STAND ON THE ROADS, each built by its own layer on a layout made here (no city): the signs (`signLayer.ts`), the barriers (`barriers.ts`). */

type Add = (spec: ItemSpec) => void;

// src/lab/catalog/layers.ts:45-52 function cullClose
/** A layer's instances culled as a camera close by sees them (`renderer.ts` crowdFrustum, crowdProjection): a run's copies are written only by its cull,
 *  each at the level its size on the screen picks; this camera, 60 m off and 40 m up looking at the centre, sees the whole layout up close. */
export function cullClose(layer: { cull(frustum: Frustum, view: Matrix4): void }): void;

// src/lab/catalog/layers.ts:55-67 function oneRoad
/** A document with one road of the first class across, `extra` laid on it; its network built. */
function oneRoad(extra: (doc: RoadDoc, net: Network) => void, junction = false): { doc: RoadDoc; net: Network };

// src/lab/catalog/layers.ts:70-74 function footwayOut
/** Where the footway of the road across runs (the document's first road): its middle, metres from the road's centre line. */
function footwayOut(net: Network): number;

// src/lab/catalog/layers.ts:77-77 function signItems.flat
const flat = (): number => …;

// src/lab/catalog/layers.ts:118-134 function signalItems
/** The traffic signals of a signalised four-way junction as the signal layer stands them (`signals.ts`): a feed as the simulation gives it, its two groups (one
 *  each way across), one stage, every head green. */
export function signalItems(add: Add, cache: BatchCache): void;

// src/lab/catalog/layers.ts:238-262 function transitItems
/** Rails and stations as the transit layer lays them (`transit.ts`): a train's track with a station, the metro's under the ground with its station and way
 *  down, and a bus stop on a road. */
export function transitItems(add: Add, cache: BatchCache): void;
