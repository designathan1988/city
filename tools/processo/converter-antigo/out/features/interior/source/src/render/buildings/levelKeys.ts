// The code of src/render/buildings/levelKeys.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/levelKeys.ts:12-12 function tileOf: the feature's own
export const tileOf = (i: number, j: number): string => `t:${Math.floor(i / TILE_CELLS)},${Math.floor(j / TILE_CELLS)}`;

// src/render/buildings/levelKeys.ts:13-13 function plateOf: the feature's own
export const plateOf = (i: number, j: number): string => `p:${Math.floor((i + PLATE_SHIFT) / PLATE_CELLS)},${Math.floor((j + PLATE_SHIFT) / PLATE_CELLS)}`;

// src/render/buildings/levelKeys.ts:15-15 function baseOf: the feature's own
export const baseOf = (i: number, j: number): string => `b:${Math.floor((i + PLATE_SHIFT) / PLATE_CELLS)},${Math.floor((j + PLATE_SHIFT) / PLATE_CELLS)}`;

// src/render/buildings/levelKeys.ts:18-21 function cellIndex: the feature's own
export function cellIndex(key: string): readonly [number, number] {
  const comma = key.indexOf(',');
  return [Number(key.slice(0, comma)), Number(key.slice(comma + 1))];
}
