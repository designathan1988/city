// What the feature uses of src/ui/planSwatch.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/planSwatch.ts:6-31 variable OUTLINES
/** The footprint a shape tool draws, as a picture: the outline filled, on a grid, seen from above */

/** Outline points per shape, in a 0..1 box. */
const OUTLINES: Readonly<Record<PlanShapeId, readonly (readonly [number, number])[]>> = {
  rectangle: [[0.08, 0.2], [0.92, 0.2], [0.92, 0.8], [0.08, 0.8]],
  l: [[0.08, 0.14], [0.5, 0.14], [0.5, 0.5], [0.92, 0.5], [0.92, 0.86], [0.08, 0.86]],
  u: [
    [0.08, 0.14], [0.32, 0.14], [0.32, 0.6], [0.68, 0.6], [0.68, 0.14], [0.92, 0.14],
    [0.92, 0.86], [0.08, 0.86],
  ],
  circle: Array.from({ length: 28 }, (_, i) => {
    const a = (i / 28) * Math.PI * 2;
    return [0.5 + Math.cos(a) * 0.42, 0.5 + Math.sin(a) * 0.34] as const;
  }),
  hexagon: [[0.5, 0.1], [0.88, 0.3], [0.88, 0.7], [0.5, 0.9], [0.12, 0.7], [0.12, 0.3]],
  octagon: [
    [0.32, 0.1], [0.68, 0.1], [0.9, 0.32], [0.9, 0.68], [0.68, 0.9], [0.32, 0.9], [0.1, 0.68], [0.1, 0.32],
  ],
  chamfered: [[0.08, 0.16], [0.6, 0.16], [0.92, 0.5], [0.92, 0.84], [0.08, 0.84]],
  cross: [
    [0.38, 0.08], [0.62, 0.08], [0.62, 0.36], [0.86, 0.36], [0.86, 0.64], [0.62, 0.64], [0.62, 0.92], [0.38, 0.92],
    [0.38, 0.64], [0.14, 0.64], [0.14, 0.36], [0.38, 0.36],
  ],
  stepped: [
    [0.38, 0.08], [0.62, 0.08], [0.62, 0.2], [0.74, 0.2], [0.74, 0.34], [0.86, 0.34], [0.86, 0.66], [0.74, 0.66],
    [0.74, 0.8], [0.62, 0.8], [0.62, 0.92], [0.38, 0.92], [0.38, 0.8], [0.26, 0.8], [0.26, 0.66], [0.14, 0.66],
    [0.14, 0.34], [0.26, 0.34], [0.26, 0.2], [0.38, 0.2],
  ],
};

// src/ui/planSwatch.ts:34-34 variable CACHE
/** Drawn once per shape and size. */
const CACHE = new Map<string, string>();

// src/ui/planSwatch.ts:36-87 function planSwatch
export function planSwatch(shape: PlanShapeId, width = 148, height = 78, dpr = 2): string;
