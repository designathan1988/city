// The code of src/lab/inspector/entity.ts that is the feature's (the rest of the file is shared).

// src/lab/inspector/entity.ts:67-102 slice of src/lab/inspector/entity.ts#entitiesAt (shared): the feature's statements; "…" marks code left out
export function entitiesAt(ctx: LabContext, px: number, py: number, ray: Ray | null, first: Hit | null): EntityRef[] {
  // … 19 lines
  if (first?.point) { const p = toMap(first.point); ground = { kind: 'terrain', x: p.x, y: p.y }; }
  else {
    // … 1 lines
    const p = ctx.scene.viewport.toWorld(px, py, rect.width, rect.height);
    ground = { kind: 'terrain', x: p.x, y: p.y };
  }
// … 9 lines
}
