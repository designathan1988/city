// The code of src/lab/tests/measure.ts that is the feature's (the rest of the file is shared).

// src/lab/tests/measure.ts:109-129 slice of src/lab/tests/measure.ts#holdScene (shared): the feature's statements; "…" marks code left out
export function holdScene(ctx: LabContext, clock: 'held' | 'running'): Hold {
  // … 1 lines
  const wasPaused = ctx.sim.clock.paused;
  if (r?.measure) r.measure(clock === 'held' ? true : 'running');
  else if (clock === 'held') ctx.sim.clock.paused = true;
// … 15 lines
}

// src/lab/tests/measure.ts:122-127 slice of src/lab/tests/measure.ts#holdScene.release (shared): the feature's statements; "…" marks code left out
    release() {
      // … 1 lines
      if (r?.measure) r.measure(false);
      else ctx.sim.clock.paused = wasPaused;
    // … 1 lines
    },
