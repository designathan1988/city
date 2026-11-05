// The code of src/render/networkFarRoads.ts that is the feature's (the rest of the file is shared).

// src/render/networkFarRoads.ts:108-108 slice of src/render/networkFarRoads.ts#NetworkFarRoads.meshesOf (shared): the feature's statements; "…" marks code left out
  private readonly meshesOf = new Map<string, { readonly batch: GpuBatchedMesh; readonly id: number }[]>();

// src/render/networkFarRoads.ts:167-205 slice of src/render/networkFarRoads.ts#NetworkFarRoads.place (shared): the feature's statements; "…" marks code left out
  private place(cells: readonly FarRoadCell[]): void {
    // … 1 lines
    for (const layer of [0, 1] as const) {
      // … 4 lines
      const batch = new GpuBatchedMesh(mine.length, vertices, indices, this.materials[layer]);
      batch.name = 'road-far-network';
      batch.renderOrder = layer === 0 ? 1 : 2;
      batch.receiveShadow = true;
      batch.matrixAutoUpdate = false;
      // … 1 lines
      batch.sortObjects = false;
      for (const c of mine) {
        // … 10 lines
        const id = batch.addInstance(batch.addGeometry(g));
        batch.setVisibleAt(id, this.shown(c.key));
        // … 5 lines
        if (list) list.push({ batch, id }); else this.meshesOf.set(c.key, [{ batch, id }]);
      }
      batch.computeBoundingSphere();
      batch.computeBoundingBox();
      this.group.add(batch);
    }
  }

// src/render/networkFarRoads.ts:253-261 slice of src/render/networkFarRoads.ts#NetworkFarRoads.clearMeshes (shared): the feature's statements; "…" marks code left out
  private clearMeshes(): void {
    for (const child of [...this.group.children]) {
      // … 1 lines
      (child as GpuBatchedMesh).dispose();
    }
  // … 3 lines
  }
