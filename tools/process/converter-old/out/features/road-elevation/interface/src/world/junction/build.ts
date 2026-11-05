// What the feature uses of src/world/junction/build.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/build.ts:28-28 type SurfaceMode
/** How a node behaves geometrically. */
export type SurfaceMode = 'none' | 'junction';

// src/world/junction/build.ts:48-80 function surfaceMode
/** Classifies a node. */
export function surfaceMode(doc: RoadDoc, cache: PolylineCache, nodeId: NodeId): SurfaceMode;
