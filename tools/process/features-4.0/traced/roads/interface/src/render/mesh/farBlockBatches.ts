// What the feature uses of src/render/mesh/farBlockBatches.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/farBlockBatches.ts:11-18 interface Entry
/** THE ROAD BLOCKS' FAR LEVELS DRAWN IN BATCHES, ONE A LOOK (material, order, shadows, attributes) */
// A block's far level (`surfaceTiles.ts` attachFarLevel) stays the mesh the residency shows, hides, lets go and makes again
// (`roadResidency.ts`); it is never drawn itself (its layers none) and its geometry is copied into its look's batch, its instance there
// shown as the mesh is. DO NOT draw each far block a mesh a material: the street view of a large city drew 11 far blocks of six
// materials, ~80 draw calls of the 600 (`npm run diagnose` byCalls, 2026-10-22), where one batch a material draws them all.

/** One far level adopted: where its geometry is in its batch, and the geometry stored there (another one on the mesh: stored again). */
interface Entry {
  readonly level: Mesh;
  readonly key: string;
  stored: BufferGeometry | null;
  geometryId: number;
  instanceId: number;
  shown: boolean;
}

// src/render/mesh/farBlockBatches.ts:27-27 variable ADOPTED
const ADOPTED = 'farBlockBatched';

// src/render/mesh/farBlockBatches.ts:51-198 class FarBlockBatches
export class FarBlockBatches { … }

// src/render/mesh/farBlockBatches.ts:54-54 field FarBlockBatches.entries
private readonly entries = new Map<Mesh, Entry>();
