// What the feature uses of src/render/networkFarRoads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/networkFarRoads.ts:108-108 field NetworkFarRoads.meshesOf
/** Each cell's instances in the layers' batches, by its key. */
  private readonly meshesOf = new Map<string, { readonly batch: GpuBatchedMesh; readonly id: number }[]>();

// src/render/networkFarRoads.ts:167-205 method NetworkFarRoads.place
private place(cells: readonly FarRoadCell[]): void;

// src/render/networkFarRoads.ts:253-261 method NetworkFarRoads.clearMeshes
private clearMeshes(): void;
