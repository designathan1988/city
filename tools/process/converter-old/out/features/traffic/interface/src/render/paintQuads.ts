// What the feature uses of src/render/paintQuads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/paintQuads.ts:18-18 type QuadSink
/** Where a stroke's quads go, one ring each, with its colour (`roadSurfaceInputs.ts`). */
export type QuadSink = (color: string, ring: number[][]) => void;

// src/render/paintQuads.ts:36-90 function strokeQuads
/** One stroke as quads, dashed as its pattern says. */
export function strokeQuads(sink: QuadSink, spec: StrokeSpec): void;

// src/render/paintQuads.ts:99-99 variable STOP_PAINT
/** Every colour the roads are painted in */
/** The stop lines' and the zebras' paint. */
export const STOP_PAINT = '#ece9d9';

// src/render/paintQuads.ts:100-100 variable ZEBRA_PAINT
export const ZEBRA_PAINT = '#f4f1e3';

// src/render/paintQuads.ts:101-101 variable PAINT_COLOURS
export const PAINT_COLOURS: readonly string[] = Object.freeze([...new Set([...ROAD_TYPES.map((rt) => rt.line), LANE_LINE, ...STYLE_PAINT, ...PARKING_PAINT, STOP_PAINT, ZEBRA_PAINT])]);
