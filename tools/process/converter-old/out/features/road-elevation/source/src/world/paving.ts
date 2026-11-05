// The code of src/world/paving.ts that is the feature's (the rest of the file is shared).

// src/world/paving.ts:9-9 slice of src/world/paving.ts#SAMPLE (shared): the feature's statements; "…" marks code left out
const SAMPLE = roadSampleOut();

// src/world/paving.ts:12-24 slice of src/world/paving.ts#pavedHeightFrom (shared): the feature's statements; "…" marks code left out
export function pavedHeightFrom(field: Pick<RoadReads, 'roadAt' | 'at' | 'roadAtInto'>, structures: ReadonlySet<RoadStructure>,
  riseAt: (x: number, y: number) => number, x: number, y: number): number {
  const road = field.roadAtInto ? field.roadAtInto(x, y, structures, SAMPLE) : field.roadAt(x, y, structures);
  const rt = road.type >= 0 ? ROAD_TYPES[road.type] : undefined;
  if (!rt) return NaN;
  // … 1 lines
  const walk = (road.across >= 0 ? road.sidewalkLeft : road.sidewalkRight) ?? road.sidewalk ?? rt.sidewalk;
  const footway = road.half - (casingHalf(rt) - sidewalkHalf(rt)) - ((road.sidewalk ?? rt.sidewalk) - walk);
  const across = Math.abs(road.across);
  if (across > footway) return NaN;
  const deck = field.at(x, y, structures);
  return across > footway - walk ? deck + riseAt(x, y) : deck;
}

// src/world/paving.ts:27-31 slice of src/world/paving.ts#PavingData (shared): the feature's statements; "…" marks code left out
export interface PavingData {
  // … 1 lines
  readonly field: FieldData | null;
// … 1 lines
}

// src/world/paving.ts:41-63 slice of src/world/paving.ts#pavingReader (shared): the feature's statements; "…" marks code left out
export function pavingReader(data: PavingData, naturalAt?: (x: number, y: number) => number): (x: number, y: number) => number | undefined {
  // … 5 lines
  const ground: ReadonlySet<RoadStructure> = new Set<RoadStructure>(['ground']);
  // … 2 lines
  const field = fieldOf(data.field, naturalAt ?? (() => { missed = true; return NaN; }));
  // … 3 lines
  const asked = new PointHeights();
  return (x, y) => {
    if (!inside(x, y) || asked.get(x, y) !== undefined) return undefined;
    // … 1 lines
    const value = pavedHeightFrom(field, ground, riseAt, x, y);
    if (!missed) return value;
    asked.add(x, y, 0);
  // … 1 lines
  };
}
