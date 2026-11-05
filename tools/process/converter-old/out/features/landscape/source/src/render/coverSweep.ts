// The code of src/render/coverSweep.ts that is the feature's (the rest of the file is shared).

// src/render/coverSweep.ts:14-21 slice of src/render/coverSweep.ts#Candidate (shared): the feature's statements; "…" marks code left out
export interface Candidate {
  // … 5 lines
  readonly species: ForestSpecies;
}

// src/render/coverSweep.ts:76-83 slice of src/render/coverSweep.ts#insidePlate (shared): the feature's statements; "…" marks code left out
function insidePlate(i: number, j: number, spacing: number): boolean {
  const x0 = -TERRAIN_HALF + i * spacing, y0 = -TERRAIN_HALF + j * spacing;
  // … 1 lines
  const cx = Math.floor((x0 + spacing / 2) / PLATE_SIZE + 0.5) * PLATE_SIZE, cy = Math.floor((y0 + spacing / 2) / PLATE_SIZE + 0.5) * PLATE_SIZE;
  // … 1 lines
  return x0 >= cx - half && x0 + spacing <= cx + half
    && y0 >= cy - half && y0 + spacing <= cy + half;
}

// src/render/coverSweep.ts:86-91 slice of src/render/coverSweep.ts#latticeOver (shared): the feature's statements; "…" marks code left out
function latticeOver(rect: ChangeRect, spacing: number): [number, number, number, number] {
  return [
    Math.floor((rect[0] + TERRAIN_HALF) / spacing), Math.floor((rect[2] + TERRAIN_HALF) / spacing),
    Math.floor((rect[1] + TERRAIN_HALF) / spacing), Math.floor((rect[3] + TERRAIN_HALF) / spacing),
  ];
}

// src/render/coverSweep.ts:95-115 slice of src/render/coverSweep.ts#forestIn (shared): the feature's statements; "…" marks code left out
export function forestIn(key: number, field: CoverField): Candidate[] {
  // … 3 lines
  for (let j = j0; j <= j1; j++) {
    for (let i = i0; i <= i1; i++) {
      const cx = -TERRAIN_HALF + (i + 0.5) * FOREST_SPACING, cy = -TERRAIN_HALF + (j + 0.5) * FOREST_SPACING;
      const density = field.forestAt(cx, cy);
      if (density < 0.04 || cellHash(i, j, 1) > density * 0.92) continue;
      const x = cx + (cellHash(i, j, 2) - 0.5) * FOREST_SPACING * 0.9, y = cy + (cellHash(i, j, 3) - 0.5) * FOREST_SPACING * 0.9;
      if (keepCellAt(x, y) !== key || !insidePlate(i, j, FOREST_SPACING)) continue;
      const h = m(10) + m(8) * cellHash(i, j, 4) * (0.6 + 0.4 * density);
      // … 4 lines
      out.push({ x, y, size: h, yaw: cellHash(i, j, 5) * Math.PI * 2, seed: seedOfKind(kind, cellHash(i, j, 6)), species });
    }
  }
// … 1 lines
}

// src/render/coverSweep.ts:118-135 slice of src/render/coverSweep.ts#scrubIn (shared): the feature's statements; "…" marks code left out
export function scrubIn(key: number, field: CoverField): Candidate[] {
  // … 3 lines
  for (let j = j0; j <= j1; j++) {
    for (let i = i0; i <= i1; i++) {
      const cx = -TERRAIN_HALF + (i + 0.5) * SCRUB_SPACING, cy = -TERRAIN_HALF + (j + 0.5) * SCRUB_SPACING;
      const density = field.coverAt('scrub', cx, cy);
      if (density < 0.04) continue;
      const clump = 0.35 + 0.65 * Math.max(0, Math.min(1, (scrubClump(cx, cy) + 0.15) * 1.6));
      if (cellHash(i, j, 21) > Math.sqrt(density) * clump) continue;
      const x = cx + (cellHash(i, j, 22) - 0.5) * SCRUB_SPACING * 0.95, y = cy + (cellHash(i, j, 23) - 0.5) * SCRUB_SPACING * 0.95;
      if (keepCellAt(x, y) !== key || !insidePlate(i, j, SCRUB_SPACING)) continue;
      out.push({ x, y, size: m(1.5) + m(2) * cellHash(i, j, 24) * clump, yaw: cellHash(i, j, 26) * Math.PI * 2, seed: cellHash(i, j, 27), species: 'broadleaf' });
    }
  }
// … 1 lines
}

// src/render/coverSweep.ts:138-156 slice of src/render/coverSweep.ts#rocksIn (shared): the feature's statements; "…" marks code left out
export function rocksIn(key: number, field: CoverField): Candidate[] {
  // … 3 lines
  for (let j = j0; j <= j1; j++) {
    for (let i = i0; i <= i1; i++) {
      const cx = -TERRAIN_HALF + (i + 0.5) * ROCK_SPACING, cy = -TERRAIN_HALF + (j + 0.5) * ROCK_SPACING;
      const density = field.coverAt('rocks', cx, cy);
      if (density < 0.04 || cellHash(i, j, 41) > Math.sqrt(density) * 0.7) continue;
      const x = cx + (cellHash(i, j, 42) - 0.5) * ROCK_SPACING * 0.9, y = cy + (cellHash(i, j, 43) - 0.5) * ROCK_SPACING * 0.9;
      if (keepCellAt(x, y) !== key || !insidePlate(i, j, ROCK_SPACING)) continue;
      // … 3 lines
      out.push({ x, y, size, yaw: cellHash(i, j, 46) * Math.PI * 2, seed: cellHash(i, j, 47), species: 'broadleaf' });
    }
  }
// … 1 lines
}

// src/render/coverSweep.ts:162-183 slice of src/render/coverSweep.ts#shoreIn (shared): the feature's statements; "…" marks code left out
export function shoreIn(key: number, field: CoverField, water: WaterBox | null): ShoreCandidate[] {
  // … 5 lines
  const i0 = Math.max(Math.floor((water.minX - SHORE_REACH + TERRAIN_HALF) / s), Math.floor((cell[0] + TERRAIN_HALF) / s) - 1);
  const i1 = Math.min(Math.ceil((water.maxX + SHORE_REACH + TERRAIN_HALF) / s), Math.floor((cell[2] + TERRAIN_HALF) / s));
  const j0 = Math.max(Math.floor((water.minY - SHORE_REACH + TERRAIN_HALF) / s), Math.floor((cell[1] + TERRAIN_HALF) / s) - 1);
  const j1 = Math.min(Math.ceil((water.maxY + SHORE_REACH + TERRAIN_HALF) / s), Math.floor((cell[3] + TERRAIN_HALF) / s));
  for (let j = j0; j <= j1; j++) {
    for (let i = i0; i <= i1; i++) {
      const x = -TERRAIN_HALF + (i + cellHash(i, j, 51)) * s, y = -TERRAIN_HALF + (j + cellHash(i, j, 52)) * s;
      if (keepCellAt(x, y) !== key) continue;
      const level = field.shoreLevelAt(x, y);
      if (level === null) continue;
      const size = m(0.6) + m(1.6) * cellHash(i, j, 54) ** 1.6;
      out.push({ x, y, level, draw: cellHash(i, j, 53), size, yaw: cellHash(i, j, 55) * Math.PI * 2, seed: cellHash(i, j, 56), species: 'broadleaf' });
    }
  }
// … 1 lines
}
