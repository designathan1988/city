// The code of src/render/scenery.ts that is the feature's (the rest of the file is shared).

// src/render/scenery.ts:131-172 slice of src/render/scenery.ts#SceneryKit (shared): the feature's statements; "…" marks code left out
export interface SceneryKit {
  // … 8 lines
  readonly furniture: Record<Exclude<FurnitureKind, 'streetTree' | 'shrub'>, BufferGeometry>;
// … 31 lines
}
