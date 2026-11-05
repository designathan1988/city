// The code of src/render/groundShape.ts that is the feature's (the rest of the file is shared).

// src/render/groundShape.ts:31-35 slice of src/render/groundShape.ts#GroundShapeAsk (shared): the feature's statements; "…" marks code left out
export interface GroundShapeAsk {
  readonly roads: RoadShapeCut | null;
// … 2 lines
}

// src/render/groundShape.ts:107-151 slice of src/render/groundShape.ts#shapeGroundAsk (shared): the feature's statements; "…" marks code left out
export function shapeGroundAsk(ask: GroundShapeAsk, scratch = new ShapeScratch()): PlateShapeAnswer[] {
  const roads = ask.roads ? roadShaper(ask.roads) : null;
  // … 1 lines
  const shapeAt = pads ? shapeBoth(roads, pads) : roads ? roads.shapeAt : null;
  return ask.plates.map((plate) => {
    // … 2 lines
    if (shapeAt) {
      const { n, cx, cy } = plate;
      const size = n * n;
      if (scratch.visited.length < size) {
        scratch.xs = new Float64Array(size); scratch.ys = new Float64Array(size); scratch.lands = new Float64Array(size); scratch.visited = new Int32Array(size);
        scratch.shaped = new Int32Array(size); scratch.heights = new Float64Array(size);
      }
      const { xs, ys, lands, visited } = scratch;
      const regions: TerrainRegion[] = [];
      let o = 0;
      for (let k = 0; k < plate.regions.length; k += 4) {
        const x0 = plate.regions[k]!, x1 = plate.regions[k + 1]!, y0 = plate.regions[k + 2]!, y1 = plate.regions[k + 3]!;
        regions.push([x0, x1, y0, y1]);
        if (plate.whole) continue;
        for (let iy = y0; iy <= y1; iy++) {
          for (let ix = x0; ix <= x1; ix++) {
            const i = ix + iy * n;
            xs[i] = plate.xy[o * 2]!;
            ys[i] = plate.xy[o * 2 + 1]!;
            lands[i] = plate.natural[o]!;
            o++;
          }
        }
      }
      if (plate.whole) {
        for (let i = 0; i < size; i++) { xs[i] = plate.xy[i * 2]!; ys[i] = plate.xy[i * 2 + 1]!; }
        lands.set(plate.natural);
      }
      const grid: CornerGrid = { n, cell: NaN, half: NaN, x: (i) => xs[i]!, y: (i) => ys[i]! };
      // In the world's coordinates, as the atlas asks its plates' shapers (`flatAtlas.ts` `shaperFor`).
      const shaper = { shapeAt: (x: number, y: number, ground: number) => shapeAt(x + cx, y + cy, ground), shapeBounds: NO_BOUNDS };
      const pass = ++scratch.pass;
      const { shaped, heights } = scratch;
      shapeCorners(grid, lands, shaper, regions, visited, pass, (i, height) => { shaped[count] = i; heights[count++] = height; });
    }
  // … 1 lines
  });
}

// src/render/groundShape.ts:154-154 slice of src/render/groundShape.ts#BlockRoads (shared): the feature's statements; "…" marks code left out
type BlockRoads = Pick<RoadElevation, 'shapeAt' | 'at' | 'roadAt'>;

// src/render/groundShape.ts:157-171 slice of src/render/groundShape.ts#blockDigest (shared): the feature's statements; "…" marks code left out
export function blockDigest(solve: BlockRoads, naturalAt: (x: number, y: number) => number,
  x0: number, y0: number, block: number, cell: number, half: number): number {
  // … 2 lines
  for (let x = Math.ceil((x0 + half) / cell) * cell - half; x <= x0 + block; x += cell) {
    for (let y = Math.ceil((y0 + half) / cell) * cell - half; y <= y0 + block; y += cell) {
      const shaped = solve.shapeAt(x, y, naturalAt(x, y));
      digest.add(quantum(shaped.height)).add(quantum(shaped.weight));
      const road = solve.roadAt(x, y);
      if (road.type >= 0 && Math.abs(road.across) <= road.half) digest.add(road.type).add(quantum(solve.at(x, y)));
      else digest.add(-1);
    }
  }
// … 1 lines
}

// src/render/groundShape.ts:187-193 slice of src/render/groundShape.ts#BlockDigestAsk (shared): the feature's statements; "…" marks code left out
export interface BlockDigestAsk {
  readonly solves: readonly { readonly roads: RoadShapeCut; readonly blocks: Float64Array }[];
// … 4 lines
}

// src/render/groundShape.ts:196-219 slice of src/render/groundShape.ts#digestBlocks (shared): the feature's statements; "…" marks code left out
function digestBlocks(ask: BlockDigestAsk): Float64Array[] {
  // … 9 lines
  return ask.solves.map(({ roads, blocks }) => {
    const solve = roadShaper(roads, naturalAt);
    // … 1 lines
    for (let k = 0; k < out.length; k++) {
      try {
        out[k] = blockDigest(solve, naturalAt, blocks[k * 2]!, blocks[k * 2 + 1]!, ask.block, ask.cell, ask.half);
      } catch (error) {
      // … 2 lines
      }
    }
  // … 1 lines
  });
}

// src/render/groundShape.ts:224-230 slice of src/render/groundShape.ts#PadsAsk (shared): the feature's statements; "…" marks code left out
export interface PadsAsk {
  // … 2 lines
  readonly paving: PavingData;
// … 2 lines
}

// src/render/groundShape.ts:233-250 slice of src/render/groundShape.ts#padsOfAsk (shared): the feature's statements; "…" marks code left out
export function padsOfAsk(ask: PadsAsk): (Pad | null)[] {
  // … 1 lines
  const read = pavingReader(ask.paving, naturalAt);
// … 14 lines
}
