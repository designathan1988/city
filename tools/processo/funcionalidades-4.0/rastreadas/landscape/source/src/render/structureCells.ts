// The code of src/render/structureCells.ts that is the feature's (the rest of the file is shared).

// src/render/structureCells.ts:28-35 slice of src/render/structureCells.ts#StructureCells (shared): the feature's statements; "…" marks code left out
export interface StructureCells {
  // … 3 lines
  prepare(net: Network, elevation: RoadElevation, terrainAt: (x: number, y: number) => number,
    blocks: readonly (readonly [number, number, number, number])[] | null): { readonly fresh: readonly Group[]; apply(): void };
// … 1 lines
}
