// The code of src/render/mesh/farBlockBatches.ts that is the feature's (the rest of the file is shared).

// src/render/mesh/farBlockBatches.ts:30-32 function batchedFar: the feature's own
export function batchedFar(level: Object3D): boolean {
  return level.userData[ADOPTED] === true;
}

// src/render/mesh/farBlockBatches.ts:62-71 method FarBlockBatches.adopt: the feature's own
  adopt(level: Mesh): void {
    if (this.entries.has(level)) return;
    const material = level.material as Material;
    const g = level.geometry;
    const names = Object.keys(g.attributes).sort().join(',');
    const key = `${material.uuid}|${level.renderOrder}|${level.castShadow}|${level.receiveShadow}|${names}`;
    level.userData[ADOPTED] = true;
    level.layers.disableAll();
    this.entries.set(level, { level, key, stored: null, geometryId: -1, instanceId: -1, shown: false });
  }
