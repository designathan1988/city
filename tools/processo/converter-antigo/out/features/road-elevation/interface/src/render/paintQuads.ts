// What the feature uses of src/render/paintQuads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/paintQuads.ts:18-18 type QuadSink
/** Where a stroke's quads go, one ring each, with its colour (`roadSurfaceInputs.ts`). */
export type QuadSink = (color: string, ring: number[][]) => void;

// src/render/paintQuads.ts:36-90 function strokeQuads
/** One stroke as quads, dashed as its pattern says. */
export function strokeQuads(sink: QuadSink, spec: StrokeSpec): void;
