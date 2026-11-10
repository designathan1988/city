// The code of src/world/roadField.ts that is the feature's (the rest of the file is shared).

// src/world/roadField.ts:143-143 function cellKeyOf: the feature's own
export const cellKeyOf = (x: number, y: number): number => x * 73_856_093 + y * 19_349_663;

// src/world/roadField.ts:154-154 field SpatialIndex.cell: the feature's own
  private readonly cell = INDEX_CELL;

// src/world/roadField.ts:155-155 field SpatialIndex.buckets: the feature's own
  private readonly buckets = new Map<number, P[]>();

// src/world/roadField.ts:156-156 field SpatialIndex.oversized: the feature's own
  private readonly oversized: P[] = [];

// src/world/roadField.ts:157-157 field SpatialIndex.oversizedSet: the feature's own
  private readonly oversizedSet = new Set<P>();

// src/world/roadField.ts:162-162 field SpatialIndex.all: the feature's own
  private readonly all: readonly P[];

// src/world/roadField.ts:164-187 constructor SpatialIndex.constructor: the feature's own
  constructor(profiles: readonly P[], private readonly answers?: ReadonlySet<number>) {
    this.all = profiles;
    for (const profile of profiles) {
      const box = profile.bbox;
      const x0 = Math.floor(box.minX / this.cell);
      const x1 = Math.floor(box.maxX / this.cell);
      const y0 = Math.floor(box.minY / this.cell);
      const y1 = Math.floor(box.maxY / this.cell);
      // A pathological box (a road across the whole map) would fill the grid
      if ((x1 - x0 + 1) * (y1 - y0 + 1) > OVERSIZED_CELLS) {
        this.oversized.push(profile);
        this.oversizedSet.add(profile);
        continue;
      }
      for (let x = x0; x <= x1; x++) {
        for (let y = y0; y <= y1; y++) {
          const key = cellKeyOf(x, y);
          const bucket = this.buckets.get(key);
          if (bucket) bucket.push(profile);
          else this.buckets.set(key, [profile]);
        }
      }
    }
  }

// src/world/roadField.ts:250-262 method SpatialIndex.near: the feature's own
  near(x: number, y: number): readonly P[] {
    const key = cellKeyOf(Math.floor(x / this.cell), Math.floor(y / this.cell));
    // DO NOT answer a point the roads handed over do not cover: its list could lack a road.
    if (this.answers && !this.answers.has(key)) throw new Error(`the road field was not handed the roads at (${x}, ${y})`);
    if (this.buckets.size === 0) return this.all;
    const bucket = this.buckets.get(key);
    if (this.oversized.length === 0) return bucket ?? EMPTY;
    if (!bucket) return this.oversized;
    // DO NOT join a cell's roads to the oversized ones at each read: once per cell (the index never changes).
    let joined = this.joined.get(key);
    if (!joined) this.joined.set(key, joined = [...bucket, ...this.oversized]);
    return joined;
  }

// src/world/roadField.ts:264-264 field SpatialIndex.joined: the feature's own
  private readonly joined = new Map<number, readonly P[]>();

// src/world/roadField.ts:672-711 function packField: the feature's own
export function packField(profiles: readonly FieldProfile[], cells: readonly number[]): { data: FieldData; transfer: ArrayBuffer[] } {
  let lineLength = 0;
  let heightLength = 0;
  for (const p of profiles) { lineLength += p.line.xy.length; heightLength += p.h.length; }
  const ids = new Float64Array(profiles.length);
  const numbers = new Float64Array(profiles.length * STRIDE);
  const lineEnds = new Uint32Array(profiles.length);
  const lines = new Float64Array(lineLength);
  const heightEnds = new Uint32Array(profiles.length);
  const heights = new Float64Array(heightLength);
  const structures: RoadStructure[] = [];
  let atLine = 0;
  let atHeight = 0;
  profiles.forEach((p, i) => {
    ids[i] = p.id;
    structures.push(p.structure);
    const o = i * STRIDE;
    numbers[o] = p.manualVertical ? 1 : 0;
    numbers[o + 1] = p.type;
    numbers[o + 2] = p.half;
    numbers[o + 3] = p.median;
    numbers[o + 4] = p.sidewalk;
    numbers[o + 5] = p.sidewalkLeft;
    numbers[o + 6] = p.sidewalkRight;
    numbers[o + 7] = p.length;
    numbers[o + 8] = p.step;
    numbers[o + 9] = p.bbox.minX;
    numbers[o + 10] = p.bbox.minY;
    numbers[o + 11] = p.bbox.maxX;
    numbers[o + 12] = p.bbox.maxY;
    lines.set(p.line.xy, atLine);
    atLine += p.line.xy.length;
    lineEnds[i] = atLine;
    for (let k = 0; k < p.h.length; k++) heights[atHeight + k] = p.h[k]!;
    atHeight += p.h.length;
    heightEnds[i] = atHeight;
  });
  const data: FieldData = { ids, structures, numbers, lineEnds, lines, heightEnds, heights, cells: Float64Array.from(cells) };
  return { data, transfer: [ids, numbers, lineEnds, lines, heightEnds, heights, data.cells].map((array) => array.buffer as ArrayBuffer) };
}
