// The code of src/render/occlusion.ts that is the feature's (the rest of the file is shared).

// src/render/occlusion.ts:57-87 slice of src/render/occlusion.ts#OcclusionBuffer.build (shared): the feature's statements; "…" marks code left out
  build(camera: Camera, aspect: number, solids: CameraSolids, drawn: (x: number, y: number) => boolean, near: number): void {
    // … 22 lines
    solids.forEachOccluder(x0, y0, x1, y1, (bx, by, cos, sin, lx0, ly0, lx1, ly1, bottom, top) => {
      if (!drawn(bx, by)) return;
      this.layBox(bx, by, cos, sin, lx0, ly0, lx1, ly1, bottom, top);
    });
  // … 3 lines
  }
