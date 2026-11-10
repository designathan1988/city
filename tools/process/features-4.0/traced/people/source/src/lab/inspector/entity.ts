// The code of src/lab/inspector/entity.ts that is the feature's (the rest of the file is shared).

// src/lab/inspector/entity.ts:46-64 slice of src/lab/inspector/entity.ts#agentsNear (shared): the feature's statements; "…" marks code left out
function agentsNear(ctx: LabContext, ray: Ray, before: number): EntityRef[] {
  // … 11 lines
  for (const p of ctx.sim.pedViews) {
    const { off, along } = offRay(ray, p.x, p.y, ground(p.x, p.y) + m(0.9));
    if (off < m(0.7) && along > 0 && along < before + m(1)) out.push({ ref: { kind: 'person', id: p.id }, score: off / m(0.7) - 0.2 });
  }
// … 2 lines
}

// src/lab/inspector/entity.ts:115-207 slice of src/lab/inspector/entity.ts#entityInfo (shared): the feature's statements; "…" marks code left out
export function entityInfo(ctx: LabContext, ref: EntityRef): EntityInfo {
  // … 2 lines
  switch (ref.kind) {
    // … 63 lines
    case 'person': {
      const p = ctx.sim.pedViews.find((q) => q.id === ref.id);
      if (!p) return { title: `Person #${ref.id} (left the view)`, rows: [], record: null, at: null };
      return {
        // … 1 lines
        rows: [['position', `${fixed(p.x)}, ${fixed(p.y)}`], ['heading (rad)', fixed(p.heading, 3)], ['speed', `${(p.v * 3.6).toFixed(1)} km/h`], ['panic', !!p.panic], ['wound', p.wound ? `${p.wound.part}${p.wound.grave ? ' (severe)' : ''}` : 'no']],
        record: p,
        at: { x: p.x, y: p.y },
      };
    }
  // … 14 lines
  }
}
