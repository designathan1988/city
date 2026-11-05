// The code of src/render/poleBake.ts that is the feature's (the rest of the file is shared).

// src/render/poleBake.ts:20-20 interface PartBounds: the feature's own
export interface PartBounds { readonly box: number[]; scale: number; cover: number }

// src/render/poleBake.ts:22-22 type PartShape: the feature's own
export type PartShape = readonly [number, number, number] | null;

// src/render/poleBake.ts:25-42 function growPart: the feature's own
export function growPart(b: PartBounds, placement: Placement, shape: PartShape): boolean {
  let moved = false;
  const box = b.box;
  const x = placement.x, y = placement.z, z = -placement.y;
  if (x < box[0]! || y < box[1]! || z < box[2]! || x > box[3]! || y > box[4]! || z > box[5]!) {
    box[0] = Math.min(box[0]!, x); box[1] = Math.min(box[1]!, y); box[2] = Math.min(box[2]!, z);
    box[3] = Math.max(box[3]!, x); box[4] = Math.max(box[4]!, y); box[5] = Math.max(box[5]!, z);
    moved = true;
  }
  const s = Math.max(Math.abs(placement.sx), Math.abs(placement.sy), Math.abs(placement.sz));
  if (s > b.scale) { b.scale = s; moved = true; }
  // The piece's box as placed (a turn keeps its sides): its two longest sides.
  if (shape) {
    const [long, wide] = [shape[0] * Math.abs(placement.sx), shape[1] * Math.abs(placement.sy), shape[2] * Math.abs(placement.sz)].sort((p, q) => q - p);
    b.cover = Math.max(b.cover, long! * wide!);
  }
  return moved;
}

// src/render/poleBake.ts:45-48 function partShape: the feature's own
export function partShape(geometry: { readonly boundingBox: { readonly min: { readonly x: number; readonly y: number; readonly z: number }; readonly max: { readonly x: number; readonly y: number; readonly z: number } } | null }): PartShape {
  const b = geometry.boundingBox;
  return b ? [b.max.x - b.min.x, b.max.y - b.min.y, b.max.z - b.min.z] : null;
}

// src/render/poleBake.ts:51-51 interface WireBounds: the feature's own
export interface WireBounds { readonly box: number[]; sag: number }

// src/render/poleBake.ts:56-70 function growWires: the feature's own
export function growWires(b: WireBounds, d: Float32Array, at: number): boolean {
  let moved = false;
  const x = b.box;
  for (let i = at; i < at + WIRE_BLOCK; i += WIRE_STRIDE) {
    for (const k of [i, i + 3]) {
      if (d[k]! < x[0]! || d[k + 1]! < x[1]! || d[k + 2]! < x[2]! || d[k]! > x[3]! || d[k + 1]! > x[4]! || d[k + 2]! > x[5]!) {
        x[0] = Math.min(x[0]!, d[k]!); x[1] = Math.min(x[1]!, d[k + 1]!); x[2] = Math.min(x[2]!, d[k + 2]!);
        x[3] = Math.max(x[3]!, d[k]!); x[4] = Math.max(x[4]!, d[k + 1]!); x[5] = Math.max(x[5]!, d[k + 2]!);
        moved = true;
      }
    }
    if (d[i + 6]! > b.sag) { b.sag = d[i + 6]!; moved = true; }
  }
  return moved;
}

// src/render/poleBake.ts:72-72 function emptyBox: the feature's own
export const emptyBox = (): number[] => [Infinity, Infinity, Infinity, -Infinity, -Infinity, -Infinity];
