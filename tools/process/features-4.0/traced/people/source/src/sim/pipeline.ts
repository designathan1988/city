// The code of src/sim/pipeline.ts that is the feature's (the rest of the file is shared).

// src/sim/pipeline.ts:29-128 slice of src/sim/pipeline.ts#step (shared): the feature's statements; "…" marks code left out
export function step(w: SimWorld, opts: StepOptions = {}): void {
  // … 23 lines
  w.pedEngine.beginTick(w);
  // … 17 lines
  w.pedEngine.dispatch(w, pedestrians);
  // … 18 lines
  if (pedestrians) w.pedEngine.step(w);
  // … 33 lines
  w.pedEngine.publish(w);
// … 3 lines
}

// src/sim/pipeline.ts:173-175 slice of src/sim/pipeline.ts#rebindPeds (shared): the feature's statements; "…" marks code left out
export function rebindPeds(w: SimWorld): void {
  w.pedEngine.rebind(w);
}
