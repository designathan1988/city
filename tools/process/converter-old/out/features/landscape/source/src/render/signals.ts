// The code of src/render/signals.ts that is the feature's (the rest of the file is shared).

// src/render/signals.ts:105-730 slice of src/render/signals.ts#createSignalHeads (shared): the feature's statements; "…" marks code left out
export function createSignalHeads(
// … 2 lines
): SignalHeads {
  // … 3 lines
  holdStill(group);
  // … 44 lines
  applyStructureSway(housing, HEAD_CENTRE, POST_HEIGHT, SWAY, 'signal-housing');
  // … 44 lines
  applyPartSway(frame, POST_HEIGHT, SWAY, 'signal-frame');
  // … 48 lines
  for (const [k, mat] of [['lens', lenses], ['halo', halo]] as const) {
    applyStructureSway(mat, HEAD_CENTRE, POST_HEIGHT, SWAY, `signal-${k}`);
  }
// … 476 lines
}
