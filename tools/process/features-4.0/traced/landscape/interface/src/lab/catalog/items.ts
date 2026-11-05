// What the feature uses of src/lab/catalog/items.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/catalog/items.ts:7-7 type Origin
/** ONE ENTRY OF THE ENGINE'S CATALOGUE: what an object is made of, measured from the pieces its builder made (`scripts/model-catalog.mjs`). */

/** Where the object came from: its own builder (kit), a model file, the test city (it depends on the city's layout), an editor preview, an effect. */
export type Origin = 'kit' | 'file' | 'test city' | 'editor preview' | 'effect';

// src/lab/catalog/items.ts:9-9 type Build
/** How the engine draws it: one draw for many copies, merged into a bigger mesh, a mesh of its own, skinned, points, lines. */
export type Build = 'instanced' | 'merged' | 'individual' | 'skinned' | 'points' | 'lines';

// src/lab/catalog/items.ts:11-20 interface PieceSpec
export interface PieceSpec {
  readonly label: string;
  readonly geometry: BufferGeometry;
  readonly material: Material | readonly Material[] | null;
  /** Its coarser levels, finest first (the piece's own geometry is level 0). */
  readonly levels?: readonly BufferGeometry[];
  readonly bones?: number;
  /** Copies of it inside the object (an instanced part of one building): its triangles count that many times, its buffers once. */
  readonly copies?: number;
}

// src/lab/catalog/items.ts:22-50 interface ItemSpec
export interface ItemSpec {
  readonly group: string;
  readonly name: string;
  readonly variation?: string;
  readonly origin: Origin;
  /** The source file that builds it. */
  readonly source: string;
  readonly build: Build;
  readonly pieces: readonly PieceSpec[];
  /** Whether it casts a shadow and with which geometry, when its builder says so (else read from the test city's meshes). */
  readonly shadow?: string;
  readonly notes?: readonly string[];
  /** What the test city's count of it is read from (`city.ts`). */
  readonly countKey?: string;
  /** Floors, for a building: its cost a floor. */
  readonly floors?: number;
  /** Bytes outside the geometry and textures that it needs (a body's shape targets). */
  readonly extraBytes?: number;
  /** Kept out of the running game's cost (an editor preview). */
  readonly preview?: boolean;
  /** Counted in the effects' section. */
  readonly effect?: boolean;
  /** Numbers measured elsewhere, for an object with no pieces of its own here (a test city layer read from the scene). */
  readonly given?: { readonly triangles: number; readonly vertices: number; readonly bytes: number; readonly released: boolean };
  /** The object as the engine put it together (a building with its instanced parts, a layer of the test city): drawn and exported as it is. */
  readonly object?: Object3D;
  /** The test city's own copy of an object a kit row already counts: listed with its picture and model, kept out of the cost. */
  readonly sceneCopy?: boolean;
}
