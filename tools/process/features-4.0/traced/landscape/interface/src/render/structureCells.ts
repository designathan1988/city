// What the feature uses of src/render/structureCells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/structureCells.ts:28-35 interface StructureCells
export interface StructureCells {
  readonly group: Group;
  readonly triangles: number;
  /** The cells a road edit reached (`blocks`: where the roads or the ground changed; null: everywhere) built again */
  prepare(net: Network, elevation: RoadElevation, terrainAt: (x: number, y: number) => number,
    blocks: readonly (readonly [number, number, number, number])[] | null): { readonly fresh: readonly Group[]; apply(): void };
  dispose(): void;
}

// src/render/structureCells.ts:37-115 function createStructureCells
export function createStructureCells(materials: SceneMaterials, groundMaterial?: Material): StructureCells;
