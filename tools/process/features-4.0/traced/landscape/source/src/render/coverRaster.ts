// The code of src/render/coverRaster.ts that is the feature's (the rest of the file is shared).

// src/render/coverRaster.ts:7-7 variable GRID: the feature's own
const GRID = TERRAIN_SEGMENTS + 1;

// src/render/coverRaster.ts:10-32 function rasterGeology: the feature's own
export function rasterGeology(sand: Float32Array, basalt: Float32Array, dab: PaintDab): readonly [number, number, number, number] {
  const toSand = dab.kind === 'sandstone' ? 1 : 0;
  const toBasalt = dab.kind === 'basalt' ? 1 : 0;
  // Rows run from +y downwards, as the plane lays them.
  const cx = (dab.x + TERRAIN_HALF) / TERRAIN_CELL;
  const cy = (TERRAIN_HALF - dab.y) / TERRAIN_CELL;
  const r = dab.radius / TERRAIN_CELL;
  const x0 = Math.max(0, Math.floor(cx - r)), x1 = Math.min(GRID - 1, Math.ceil(cx + r));
  const y0 = Math.max(0, Math.floor(cy - r)), y1 = Math.min(GRID - 1, Math.ceil(cy + r));
  for (let iy = y0; iy <= y1; iy++) {
    for (let ix = x0; ix <= x1; ix++) {
      const d = Math.hypot(ix - cx, iy - cy) / Math.max(1e-6, r);
      if (d >= 1) continue;
      // Flat to near its rim: a landform's cliff stands at the rim of its dabs
      const t = Math.min(1, Math.max(0, (d - 0.78) / 0.22));
      const w = Math.min(1, dab.strength * (1 - t * t * (3 - 2 * t)));
      const k = iy * GRID + ix;
      sand[k] = (sand[k] as number) + (toSand - (sand[k] as number)) * w;
      basalt[k] = (basalt[k] as number) + (toBasalt - (basalt[k] as number)) * w;
    }
  }
  return [dab.x - dab.radius, dab.y - dab.radius, dab.x + dab.radius, dab.y + dab.radius];
}

// src/render/coverRaster.ts:35-35 variable FOREST_RES: the feature's own
export const FOREST_RES = 256;

// src/render/coverRaster.ts:59-64 function coverOf: the feature's own
export function coverOf(covers: Readonly<Record<CoverKind, Uint8Array>>, kind: CoverKind, x: number, y: number): number {
  const cell = TERRAIN_SIZE / FOREST_RES;
  const gx = Math.floor((x + TERRAIN_HALF) / cell), gy = Math.floor((y + TERRAIN_HALF) / cell);
  if (gx < 0 || gy < 0 || gx >= FOREST_RES || gy >= FOREST_RES) return 0;
  return covers[kind][gy * FOREST_RES + gx]! / 255;
}

// src/render/coverRaster.ts:67-73 function geologyOf: the feature's own
export function geologyOf(sand: Float32Array, basalt: Float32Array, x: number, y: number): GeologyKind {
  const ix = Math.round((x + TERRAIN_HALF) / TERRAIN_CELL);
  const iy = Math.round((TERRAIN_HALF - y) / TERRAIN_CELL);
  if (ix < 0 || iy < 0 || ix >= GRID || iy >= GRID) return 'granite';
  const s = sand[iy * GRID + ix] as number, b = basalt[iy * GRID + ix] as number;
  return s >= 0.5 && s >= b ? 'sandstone' : b >= 0.5 ? 'basalt' : 'granite';
}

// src/render/coverRaster.ts:76-94 function rasterCover: the feature's own
export function rasterCover(covers: Readonly<Record<CoverKind, Uint8Array>>, dab: PaintDab): void {
  const cell = TERRAIN_SIZE / FOREST_RES;
  const cx = (dab.x + TERRAIN_HALF) / cell, cy = (dab.y + TERRAIN_HALF) / cell, r = dab.radius / cell;
  const x0 = Math.max(0, Math.floor(cx - r)), x1 = Math.min(FOREST_RES - 1, Math.ceil(cx + r));
  const y0 = Math.max(0, Math.floor(cy - r)), y1 = Math.min(FOREST_RES - 1, Math.ceil(cy + r));
  for (let y = y0; y <= y1; y++) {
    for (let x = x0; x <= x1; x++) {
      const d = Math.hypot(x + 0.5 - cx, y + 0.5 - cy) / Math.max(1e-6, r);
      if (d >= 1) continue;
      const w = Math.min(1, dab.strength * (1 - d * d * (3 - 2 * d)));
      const i = y * FOREST_RES + x;
      for (const kind of COVER_KINDS) {
        const density = covers[kind];
        const target = dab.kind === kind ? 255 : 0;
        density[i] = Math.round(density[i]! + (target - density[i]!) * w);
      }
    }
  }
}
