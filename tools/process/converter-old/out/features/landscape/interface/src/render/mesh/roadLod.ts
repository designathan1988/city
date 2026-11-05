// What the feature uses of src/render/mesh/roadLod.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/roadLod.ts:22-49 interface BlockLevels
/** What a block keeps of its two levels (`mesh.userData.lod`). */
export interface BlockLevels {
  /** The far level, null where the block is too small to have one. */
  readonly coarse: Mesh | null;
  /** The block's centre in the scene (x, z), for the distance to the camera. */
  readonly x: number;
  readonly z: number;
  /** A block of several surfaces drawn alike (`roadSurfaces.ts` drawGroups): each other surface's centre (x, z), as its own block had it.
   *  The block is near where any of them would be: none drawn coarser than it was as a block of its own. */
  readonly also?: readonly (readonly [number, number])[];
  /** Whether the full level is on the GPU (its arrays given back when it comes near: `bring`, else `refillArrays`). */
  full: boolean;
  /** The full level's arrays made off the page when the block comes near (the kept tiles read from the store, `roadSurfaces.ts`); null: not kept, cut again on the page. Unset: no such source (tests). */
  bring?: (() => Promise<GeometryArrays | null>) | null;
  /** Whether `bring` is under way: the block shows the full level when it lands. */
  bringing?: boolean;
  /** The last look it met the view at (`roadResidency.ts` updateRoadLevels): the least recently seen go first past the budget. */
  seenAt?: number;
  /** Its full level's bytes on the GPU as last measured: what bringing it near costs. */
  nearBytes?: number;
  /** The road block's cell it lies in (`roadSurfaces.ts` CHUNK, the network's far level's cell, `networkFarRoads.ts`): its levels go and come back with the cell's (`roadResidency.ts`). */
  cell?: string;
  /** Whether its far level is being made (`roadSurfaces.ts`): its cell keeps its levels till it lands. */
  pending?: boolean;
  /** Whether its far level was let go with its cell's, the network's far level drawing the cell (`roadResidency.ts` dropCell). */
  dropped?: boolean;
  /** Its far level made again off the page (the full level's arrays from the kept tiles, simplified by the workers): what a dropped cell is brought back with. */
  remake?: (() => Promise<BlockArrays | null>) | null;
}

// src/render/mesh/roadLod.ts:53-53 function levelsOf
export const levelsOf = (mesh: Mesh): BlockLevels | undefined => …;
