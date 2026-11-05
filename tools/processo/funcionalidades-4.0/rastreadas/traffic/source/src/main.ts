// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:310-310 slice of src/main.ts#startPopulationShare (shared): the feature's statements; "…" marks code left out
const startPopulationShare = cpuPopulationShare(WEAK_MACHINE.threads ?? navigator.hardwareConcurrency ?? 8);

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 33 lines
  sim.frame(wall, holdSim, { traffic: !game.paused && effects.traffic, pedestrians: !game.paused && effects.pedestrians, maxShare: maxPopulationShare * effects.populationShare, startShare: startPopulationShare, opening: simOpening });
// … 66 lines
}
