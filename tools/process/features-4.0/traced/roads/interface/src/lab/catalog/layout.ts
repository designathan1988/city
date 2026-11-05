// What the feature uses of src/lab/catalog/layout.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/catalog/layout.ts:37-50 function piecesOf
/** A built object's meshes as pieces: each block's far level as its level (`mesh/roadLod.ts`), an instanced mesh's copies counted. */
export function piecesOf(root: Object3D): PieceSpec[];

// src/lab/catalog/layout.ts:53-81 function shown
/** The object as a sheet and a 3D file show it: every copy where the engine put it (its poses made matrices, `capture.ts`), in a material drawn without the engine's uniforms. */
export function shown(root: Object3D, cache: BatchCache): Object3D;

// src/lab/catalog/layout.ts:89-91 function road
/** A road of `type` from (x0, y0) to (x1, y1). */
function road(doc: RoadDoc, type: number, x0: number, y0: number, x1: number, y1: number, structure: RoadStructure = 'ground', direction: SegmentDirection = 'both'): void;

// src/lab/catalog/layout.ts:166-166 function layoutItems.flat
const flat = (): number => …;
