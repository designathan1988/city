// The code of src/lab/inspector/entity.ts that is the feature's (the rest of the file is shared).

// src/lab/inspector/entity.ts:46-64 slice of src/lab/inspector/entity.ts#agentsNear (shared): the feature's statements; "…" marks code left out
function agentsNear(ctx: LabContext, ray: Ray, before: number): EntityRef[] {
  // … 5 lines
  for (const v of vehicles) {
    // … 1 lines
    const half = Math.max(m(1), (v.archetype?.length ?? m(4)) / 2);
    // … 1 lines
    if (off < half && along > 0 && along < before + half) out.push({ ref: { kind: 'vehicle', id: v.id }, score: off / half });
  }
// … 6 lines
}

// src/lab/inspector/entity.ts:115-207 slice of src/lab/inspector/entity.ts#entityInfo (shared): the feature's statements; "…" marks code left out
export function entityInfo(ctx: LabContext, ref: EntityRef): EntityInfo {
  // … 2 lines
  switch (ref.kind) {
    // … 48 lines
    case 'vehicle': {
      // … 3 lines
      return {
        title: `Vehicle #${ref.id} (${v.archetype.id})`,
        rows: [
          ['type', `${v.archetype.shape}${v.archetype.style ? ` / ${v.archetype.style}` : ''}`], ['colour', v.color], ['speed', `${(v.v * 3.6).toFixed(1)} km/h`],
          ['position', `${fixed(f.x)}, ${fixed(f.y)}`], ['street', v.segment ?? '— (connector)'], ['parked', ctx.sim.isParked(v)], ['off the road', v.free ? `lot ${v.free.lot ?? '—'}` : 'no'],
          ['people', `${v.people.length} of ${v.seats}`], ['indicator', v.indicator], ['odometer', fixed(v.odometer, 0)], ['slain', v.slain],
        ],
      // … 2 lines
      };
    }
  // … 24 lines
  }
}
