// The code of src/lab/inspector/raycast.ts that is the feature's (the rest of the file is shared).

// src/lab/inspector/raycast.ts:14-19 slice of src/lab/inspector/raycast.ts#rayAtPixel (shared): the feature's statements; "…" marks code left out
export function rayAtPixel(ctx: LabContext, px: number, py: number): Ray | null {
  // … 1 lines
  const sight = ctx.scene.viewport.rayAt?.(px, py, rect.width, rect.height);
  if (!sight) return null;
  return new Ray(new Vector3(sight.ox, sight.oz, -sight.oy), new Vector3(sight.dx, sight.dz, -sight.dy).normalize());
}
