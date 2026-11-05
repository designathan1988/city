// The code of src/lab/tests/items.ts that is the feature's (the rest of the file is shared).

// src/lab/tests/items.ts:33-36 slice of src/lab/tests/items.ts#SWITCH_GROUPS (shared): the feature's statements; "…" marks code left out
const SWITCH_GROUPS: readonly (readonly [string, readonly string[]])[] = [
  ['post', POST_SWITCHES], ['light', LIGHT_SWITCHES], ['layers', LAYER_SWITCHES], ['buildings', BUILDING_SWITCHES],
  ['cpu', ['simulation', 'traffic', 'pedestrians', 'growth', 'onDemand']],
];

// src/lab/tests/items.ts:52-102 slice of src/lab/tests/items.ts#sweepItems (shared): the feature's statements; "…" marks code left out
export function sweepItems(ctx: LabContext): SweepItem[] {
  // … 2 lines
  for (const [group, keys] of SWITCH_GROUPS) {
    for (const key of keys) {
      if (NOT_SWEPT.includes(key)) continue;
      items.push({
        id: `switch.${key}`, label: key, group: `switch: ${group}`,
        get: () => snap()[key],
        flipped: (now) => (typeof now === 'boolean' ? !now : null),
        set: (value) => { if (typeof value === 'boolean') ctx.effects.setMany({ [key]: value }); },
      });
    }
  }
// … 36 lines
}
