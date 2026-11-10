// The code of src/render/landStroke.ts that is the feature's (the rest of the file is shared).

// src/render/landStroke.ts:62-62 slice of src/render/landStroke.ts#GRID (shared): the feature's statements; "…" marks code left out
const GRID = TERRAIN_SEGMENTS + 1;

// src/render/landStroke.ts:65-96 slice of src/render/landStroke.ts#shoreLevelsInto (shared): the feature's statements; "…" marks code left out
export function shoreLevelsInto(levels: Float32Array, position: ArrayLike<number>, count: number, ground: ArrayLike<number>): void {
  // … 1 lines
  for (let i = 0; i < count; i++) {
    const gx = (position[i * 3]! + TERRAIN_HALF) / TERRAIN_CELL;
    const gy = (position[i * 3 + 2]! + TERRAIN_HALF) / TERRAIN_CELL;
    const ix = Math.round(gx);
    const iy = Math.round(gy);
    if (ix < 0 || iy < 0 || ix >= GRID || iy >= GRID) continue;
    const k = iy * GRID + ix;
    // … 1 lines
    if ((ground[k] as number) >= position[i * 3 + 1]!) continue;
    levels[k] = Math.max(levels[k] as number, position[i * 3 + 1]!);
  }
  // … 1 lines
  for (let pass = 0; pass < 3; pass++) {
    // … 1 lines
    for (let iy = 0; iy < GRID; iy++) {
      for (let ix = 0; ix < GRID; ix++) {
        const k = iy * GRID + ix;
        if ((from[k] as number) > NO_WATER / 2) continue;
        let best = NO_WATER;
        if (ix > 0) best = Math.max(best, from[k - 1] as number);
        if (ix + 1 < GRID) best = Math.max(best, from[k + 1] as number);
        if (iy > 0) best = Math.max(best, from[k - GRID] as number);
        if (iy + 1 < GRID) best = Math.max(best, from[k + GRID] as number);
        // Only onto a BANK, ground above the water
        if (best > NO_WATER / 2 && (ground[k] as number) < best) continue;
        levels[k] = best;
      }
    }
  }
}

// src/render/landStroke.ts:99-105 slice of src/render/landStroke.ts#measureLevels (shared): the feature's statements; "…" marks code left out
export function measureLevels(job: LevelsJob): Float32Array {
  const levels = new LandLevels(job.seg);
  levels.take(job.levels);
  levels.markBox(job.box[0], job.box[1], job.box[2], job.box[3]);
  levels.measure(job.heights, job.flip);
  return levels.data;
}

// src/render/landStroke.ts:111-126 slice of src/render/landStroke.ts#woodsBlock (shared): the feature's statements; "…" marks code left out
export function woodsBlock(job: WoodsJob, scratch: LandStrokeScratch = { woods: null }): WoodsBlock {
  // … 5 lines
  const { cells, i0, i1, j0, j1 } = woodsSpan(half, job.rect);
  if (scratch.woods?.length !== cells * cells) scratch.woods = new Float32Array(cells * cells);
  // … 1 lines
  woodsBaseInto(whole, land, half, job.cx, job.cy, job.rect);
  // … 1 lines
  const w = Math.max(0, i1 - i0 + 1), h = Math.max(0, j1 - j0 + 1);
  const block = new Float32Array(w * h);
  for (let j = 0; j < h; j++) block.set(whole.subarray((j0 + j) * cells + i0, (j0 + j) * cells + i0 + w), j * w);
  return { i0, i1, j0, j1, cells: block };
}

// src/render/landStroke.ts:116-116 slice of src/render/landStroke.ts#woodsBlock.land (shared): the feature's statements; "…" marks code left out
  const land = (x: number, y: number): number => (onPlate(half, x, y) ? sampleCorners(grid, natural, flip, x, y) : outside[next++]!);

// src/render/landStroke.ts:129-132 function writeWoodsBlock: the feature's own
export function writeWoodsBlock(woods: Float32Array, cells: number, block: WoodsBlock): void {
  const w = block.i1 - block.i0 + 1;
  for (let j = block.j0; j <= block.j1; j++) woods.set(block.cells.subarray((j - block.j0) * w, (j - block.j0 + 1) * w), j * cells + block.i0);
}

// src/render/landStroke.ts:135-147 slice of src/render/landStroke.ts#runLandStroke (shared): the feature's statements; "…" marks code left out
export function runLandStroke(job: LandStrokeJob, scratch: LandStrokeScratch = { woods: null }): { readonly answer: unknown; readonly transfer: Transferable[] } {
  if (job.kind === 'shore') {
    const levels = new Float32Array(GRID * GRID);
    shoreLevelsInto(levels, job.position, job.count, job.ground);
    return { answer: levels, transfer: [levels.buffer] };
  }
// … 6 lines
}

// src/render/landStroke.ts:150-151 function unionRect: the feature's own
export const unionRect = (a: readonly [number, number, number, number] | null, b: readonly [number, number, number, number]): readonly [number, number, number, number] =>
  a === null ? b : [Math.min(a[0], b[0]), Math.min(a[1], b[1]), Math.max(a[2], b[2]), Math.max(a[3], b[3])];

// src/render/landStroke.ts:154-155 function unionBox: the feature's own
export const unionBox = (a: StrokeBox | null, b: StrokeBox): StrokeBox =>
  a === null ? b : [Math.min(a[0], b[0]), Math.max(a[1], b[1]), Math.min(a[2], b[2]), Math.max(a[3], b[3])];
