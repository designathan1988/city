// The code of src/render/plateMesh.ts that is the feature's (the rest of the file is shared).

// src/render/plateMesh.ts:73-98 slice of src/render/plateMesh.ts#buildPlateMesh (shared): the feature's statements; "…" marks code left out
export function buildPlateMesh(ground: Float64Array, side: number, half: number, cell: number, base: number): PlateMesh {
  // … 10 lines
  for (let cy = 0; cy < seg; cy++) {
    for (let cx = 0; cx < seg; cx++) {
      // … 3 lines
      flip[k] = cellDiagonal(heights[a]!, heights[b]!, heights[c]!, heights[d]!);
    // … 1 lines
    }
  }
  // … 1 lines
  const levels = new LandLevels(seg);
  levels.markAll();
  levels.measure(heights, flip);
  return { heights, flip, texels, levels: levels.data };
}
