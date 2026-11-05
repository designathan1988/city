// The code of src/render/plateTexels.ts that is the feature's (the rest of the file is shared).

// src/render/plateTexels.ts:8-8 function byte: the feature's own
const byte = (value: number): number => Math.round(Math.min(1, Math.max(0, value)) * 255);

// src/render/plateTexels.ts:11-26 function landTexelsInto: the feature's own
export function landTexelsInto(heights: ArrayLike<number>, flip: ArrayLike<number>, side: number, half: number, cell: number,
  into: Float32Array, box: TexelBox | null): void {
  const seg = side - 1;
  const x0 = Math.max(0, box?.x0 ?? 0), x1 = Math.min(seg, box?.x1 ?? seg);
  const y0 = Math.max(0, box?.y0 ?? 0), y1 = Math.min(seg, box?.y1 ?? seg);
  if (x1 < x0 || y1 < y0) return;
  for (let iy = y0; iy <= y1; iy++) {
    for (let ix = x0; ix <= x1; ix++) {
      const v = ix + iy * side;
      // A corner of the last row or column has no cell of its own.
      into[v * 4 + 2] = ix < seg && iy < seg ? flip[ix + iy * seg] as number : 0;
      into[v * 4 + 3] = heights[v] as number;
    }
  }
  landNormalsInto(heights, flip, side, half, cell, into, x0, x1, y0, y1);
}

// src/render/plateTexels.ts:29-33 interface CornerStore: the feature's own
export interface CornerStore {
  readonly levels: Float32Array;
  readonly sand: Float32Array;
  readonly basalt: Float32Array;
}

// src/render/plateTexels.ts:36-36 type CornerCover: the feature's own
export type CornerCover = (corner: number, kind: 'flowers' | 'scrub' | 'forest') => number;

// src/render/plateTexels.ts:40-54 function groundCornersInto: the feature's own
export function groundCornersInto(into: Uint16Array, store: CornerStore, cover: CornerCover, side: number, box: TexelBox | null = null): void {
  const { sand, basalt } = store;
  const seg = side - 1;
  const x0 = Math.max(0, box?.x0 ?? 0), x1 = Math.min(seg, box?.x1 ?? seg);
  const y0 = Math.max(0, box?.y0 ?? 0), y1 = Math.min(seg, box?.y1 ?? seg);
  for (let iy = y0; iy <= y1; iy++) {
    for (let ix = x0; ix <= x1; ix++) {
      const k = ix + iy * side;
      into[k * 4] = byte(cover(k, 'flowers')) + 256 * byte(sand[k] as number);
      into[k * 4 + 1] = byte(cover(k, 'scrub')) + 256 * byte(basalt[k] as number);
      into[k * 4 + 2] = byte(cover(k, 'forest'));
      into[k * 4 + 3] = 0;
    }
  }
}

// src/render/plateTexels.ts:57-67 function ecologyCornersInto: the feature's own
export function ecologyCornersInto(into: Float32Array, offset: number, count: number, field: EcologyField | null): void {
  if (!field) { into.fill(0, offset, offset + count * 4); return; }
  for (let i = 0; i < count; i++) {
    const canopy = field.canopy[i] as number, trees = field.trees[i] as number, grass = field.grass[i] as number;
    const o = offset + i * 4;
    into[o] = byte(canopy + trees * 0.3 + (field.palm[i] as number) * 0.25) / 255;
    into[o + 1] = byte(grass * (field.dry[i] as number)) / 255;
    into[o + 2] = byte(field.wet[i] as number) / 255;
    into[o + 3] = byte((1 - grass) * (1 - Math.min(1, canopy + trees))) / 255;
  }
}

// src/render/plateTexels.ts:70-73 function lightLayerInto: the feature's own
export function lightLayerInto(layer: Uint8Array, res: number, light: Uint8Array, side: number): void {
  layer.fill(0);
  for (let y = 0; y < side; y++) layer.set(light.subarray(y * side * 4, (y + 1) * side * 4), y * res * 4);
}
