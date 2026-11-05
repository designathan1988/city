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

// src/render/buildings/fracture.ts:164-170 interface StillData
/** Every fragment of a building in one buffer, in world space (three's, y up), one draw group per material */
export interface StillData {
  readonly position: Float32Array; readonly normal: Float32Array; readonly colour: Float32Array; readonly uv: Float32Array; readonly decay: Float32Array;
  readonly finish: Float32Array;
  readonly groups: readonly { material: number; start: number; count: number }[];
  readonly slots: readonly Int32Array[];
  readonly fragments: readonly Fragment[];
}
