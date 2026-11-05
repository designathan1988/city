// The code of src/render/instanceRuns.ts that is the feature's (the rest of the file is shared).

// src/render/instanceRuns.ts:91-94 function leastPixel: the feature's own
export function leastPixel(geometry: BufferGeometry): KindLevels {
  if (!geometry.boundingSphere) geometry.computeBoundingSphere();
  return { reach: geometry.boundingSphere?.radius ?? 1, least: 1 };
}

// src/render/instanceRuns.ts:126-129 method RunBuilder.add: the feature's own
  add(store: number, pose: ArrayLike<number>, scale: number, tint?: Tint): void {
    const poses = this.listOf(store, scale, tint);
    for (let i = 0; i < this.format.stride; i++) poses.push(pose[i]!);
  }

// src/render/instanceRuns.ts:213-213 method createInstanceRuns.setMap: the feature's own
    setMap(map) { cells.setMap(map); },
