// The code of src/render/groundShape.ts that is the feature's (the rest of the file is shared).

// src/render/groundShape.ts:31-35 slice of src/render/groundShape.ts#GroundShapeAsk (shared): the feature's statements; "…" marks code left out
export interface GroundShapeAsk {
  // … 1 lines
  readonly pads: PadShapeCut | null;
// … 1 lines
}

// src/render/groundShape.ts:107-151 slice of src/render/groundShape.ts#shapeGroundAsk (shared): the feature's statements; "…" marks code left out
export function shapeGroundAsk(ask: GroundShapeAsk, scratch = new ShapeScratch()): PlateShapeAnswer[] {
  // … 1 lines
  const pads = ask.pads ? padShaper(ask.pads) : null;
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

// src/render/groundShape.ts:224-230 slice of src/render/groundShape.ts#PadsAsk (shared): the feature's statements; "…" marks code left out
export interface PadsAsk {
  readonly buildings: readonly { readonly record: RecordRef; readonly floor: number }[];
// … 4 lines
}

// src/render/groundShape.ts:233-250 slice of src/render/groundShape.ts#padsOfAsk (shared): the feature's statements; "…" marks code left out
export function padsOfAsk(ask: PadsAsk): (Pad | null)[] {
  // … 8 lines
  const out: (Pad | null)[] = [];
  // … 2 lines
  for (const b of eachRecord(ask.buildings.map((entry) => entry.record))) {
    const floor = ask.buildings[k++]!.floor;
    out.push(padOf(b, naturalAt, pavedAt, ask.apron, () => floor));
  }
  return out;
}

// src/render/groundShape.ts:255-255 slice of src/render/groundShape.ts#GroundJobAnswer (shared): the feature's statements; "…" marks code left out
export type GroundJobAnswer = PlateShapeAnswer[] | Float64Array[] | (Pad | null)[];
