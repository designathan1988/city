// What the feature uses of src/render/buildings/fracture.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/fracture.ts:31-40 interface Fragment
/** What a ruin knows of each fragment besides its vertices (which are in the ruin's buffer, `StillData`). */
export interface Fragment {
  /** Centre in three's space (y up), standing. */
  readonly centre: readonly [number, number, number];
  /** Radius of the fragment round its centre. */
  readonly radius: number;
  /** Lowest point, three's y. */
  readonly low: number;
  /** Fragments it is joined to. */
  readonly neighbours: readonly number[];
}

// src/render/buildings/fracture.ts:79-90 interface FractureInput
/** The fragments of one building, from its drawn chunk. */
/** What a fracture needs, as plain arrays (so it can run in a worker, `fracture.worker.ts`). */
export interface FractureInput {
  /** The shell as it is drawn (`ShellChunk`): its normal, wear and paint in shorts, its uv worked out from them (`shellUv`). */
  readonly shells: readonly { mat: number; finish: number; position: Float32Array; normal: Int16Array; paint: Uint16Array; index: Uint32Array }[];
  /** Instanced parts (windows, doors, furniture): their model, non-indexed, and each instance's matrix. */
  readonly parts: readonly { mat: number; position: Float32Array; normal: Float32Array; uv: Float32Array | null; colour: Float32Array | null;
    instColours: Float32Array | null; matrices: Float32Array; count: number }[];
  readonly slabMat: number;
  /** The slabs' finish id: concrete. */
  readonly slabFinish: number;
  readonly slabs: readonly { corners: readonly (readonly [number, number, number])[]; y: number }[];
  readonly seed: number;
}

// src/render/buildings/fracture.ts:104-104 variable MODEL_ARRAYS
/** A kit model's vertices as plain arrays, made once per model. */
const MODEL_ARRAYS = new WeakMap<BufferGeometry, { position: Float32Array; normal: Float32Array; uv: Float32Array | null; colour: Float32Array | null }>();

// src/render/buildings/fracture.ts:164-170 interface StillData
/** Every fragment of a building in one buffer, in world space (three's, y up), one draw group per material */
export interface StillData {
  readonly position: Float32Array; readonly normal: Float32Array; readonly colour: Float32Array; readonly uv: Float32Array; readonly decay: Float32Array;
  readonly finish: Float32Array;
  readonly groups: readonly { material: number; start: number; count: number }[];
  readonly slots: readonly Int32Array[];
  readonly fragments: readonly Fragment[];
}
