// The code of src/ui/planSwatch.ts that is the feature's (the rest of the file is shared).

// src/ui/planSwatch.ts:6-31 slice of src/ui/planSwatch.ts#OUTLINES (shared): the feature's statements; "…" marks code left out
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

// src/ui/planSwatch.ts:36-87 slice of src/ui/planSwatch.ts#planSwatch (shared): the feature's statements; "…" marks code left out
export function planSwatch(shape: PlanShapeId, width = 148, height = 78, dpr = 2): string {
  const key = `${shape}|${width}x${height}@${dpr}`;
  const cached = CACHE.get(key);
  if (cached !== undefined) return cached;
  // … 29 lines
  const points = OUTLINES[shape];
  // … 1 lines
  points.forEach(([x, y], i) => {
    const px = x * width;
    const py = y * height;
    if (i === 0) ctx.moveTo(px, py);
    else ctx.lineTo(px, py);
  });
  // … 8 lines
  CACHE.set(key, url);
// … 1 lines
}
