// The code of src/render/poleLayer.ts that is the feature's (the rest of the file is shared).

// src/render/poleLayer.ts:541-545 getter PoleLayer.triangles: the feature's own
  get triangles(): number {
    let triangles = 0;
    for (const block of this.blocks.values()) for (const part of block.parts) triangles += part.perInstance * part.count;
    return triangles;
  }
