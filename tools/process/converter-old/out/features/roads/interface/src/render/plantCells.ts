// What the feature uses of src/render/plantCells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/plantCells.ts:15-23 interface PlantCells
/** DO NOT write every tree of the map into its meshes again when one changed (`NatureForest.update`, `buildGroundCover`) */
export interface PlantCells {
  readonly group: Group;
  /** The cells whose places changed (`CandidateKeep.takeChangedCells`) written again; null */
  sync(changed: ReadonlySet<number> | null, cells: Iterable<number>): void;
  cull(frustum: Frustum, view: Matrix4): void;
  /** The map zoom: the leaf cards left out (`InstanceRuns.setMap`). */
  setMap(map: boolean): void;
  dispose(): void;
}

// src/render/plantCells.ts:80-110 function createTreeCells
/** The countryside's low-poly trees (`natureTrees.ts`), cell by cell: a body and its leaf cards near, its picture far (`plantImpostors.ts`). */
export function createTreeCells(kit: NatureTreeKit, name: string, treesIn: (cell: number) => TreePlacement[]): PlantCells;

// src/render/plantCells.ts:125-149 function createCoverCells
/** The painted covers' stones and bushes, the rivers' stones, and the woods' card trees where the low-poly ones are not grown. */
export function createCoverCells(kit: GroundCoverKit, name: string, places: {
  readonly rocksIn: (cell: number) => CoverPlacement[];
  readonly scrubIn: (cell: number) => CoverPlacement[];
  readonly treesIn: (cell: number) => TreePlacement[];
}): PlantCells;
