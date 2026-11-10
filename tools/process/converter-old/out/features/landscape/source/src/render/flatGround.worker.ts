// The code of src/render/flatGround.worker.ts that is the feature's (the rest of the file is shared).

// src/render/flatGround.worker.ts:7-29 slice of src/render/flatGround.worker.ts#serve (shared): the feature's statements; "…" marks code left out
export function serve(self: JobScope): void {
  // … 1 lines
  interface Ask { readonly id: number; readonly relief: ReliefVersion; readonly cx: number; readonly cy: number; readonly half: number; readonly cell: number; readonly side: number; readonly base: number }
  // … 1 lines
  self.onmessage = (event: MessageEvent<Ask>) => {
    // … 1 lines
    try {
      // … 1 lines
      for (let j = 0; j < side; j++) {
        // … 1 lines
        for (let i = 0; i < side; i++) heights[j * side + i] = reliefHeight(relief, cx - half + i * cell, y);
      }
      // … 2 lines
      const natural = cornerLand(mesh.heights, mesh.flip, side, half, cell, (x, y) => base + reliefHeight(relief, cx + x, cy + y));
      mesh.woods = woodsBase(natural, half, cx, cy);
    // … 2 lines
    } catch (error) {
    // … 1 lines
    }
  };
}

// src/render/flatGround.worker.ts:9-9 slice of src/render/flatGround.worker.ts#Ask (shared): the feature's statements; "…" marks code left out
  interface Ask { readonly id: number; readonly relief: ReliefVersion; readonly cx: number; readonly cy: number; readonly half: number; readonly cell: number; readonly side: number; readonly base: number }
