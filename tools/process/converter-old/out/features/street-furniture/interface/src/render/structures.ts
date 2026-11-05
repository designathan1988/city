// What the feature uses of src/render/structures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/structures.ts:39-45 interface StructureDetails
/** The parts of a raised structure that are not its deck */

export interface StructureDetails {
  readonly group: Group;
  readonly triangles: number;
  /** Where the structures built stand */
  readonly spans: readonly (readonly [number, number, number, number])[];
  dispose(): void;
}

// src/render/structures.ts:452-465 function buildStructureDetails
export function buildStructureDetails(
  net: Network,
  elevation: RoadElevation,
  terrainAt: (x: number, y: number) => number,
  materials: SceneMaterials,
  /** The terrain's own material, for a retaining wall's backfill */
  groundMaterial?: Material,
  /** Only these roads' structures (a cell of the structure layer, `structureCells.ts`); all of the map without. */
  only?: readonly SegmentRibbon[],
  /** The shapes the instances are drawn with, kept by the caller across builds; made here (and let go with these) without. */
  shapes?: { readonly pier: BufferGeometry; readonly box: BufferGeometry },
): StructureDetails;
