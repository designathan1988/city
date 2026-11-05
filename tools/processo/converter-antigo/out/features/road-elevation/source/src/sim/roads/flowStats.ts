// The code of src/sim/roads/flowStats.ts that is the feature's (the rest of the file is shared).

// src/sim/roads/flowStats.ts:5-5 slice of src/sim/roads/flowStats.ts#TAU (shared): the feature's statements; "…" marks code left out
const TAU = ROAD_TUNING.flow.trendWindow / 3;

// src/sim/roads/flowStats.ts:7-7 slice of src/sim/roads/flowStats.ts#KEEP (shared): the feature's statements; "…" marks code left out
const KEEP = 1 - 1 / TAU;

// src/sim/roads/flowStats.ts:35-55 slice of src/sim/roads/flowStats.ts#FlowStats.step (shared): the feature's statements; "…" marks code left out
  step(dt: number): void {
    // … 2 lines
    while (this.carry >= 1) {
      // … 3 lines
      for (const [node, counted] of this.counts) {
        // … 2 lines
        for (const [seg, n] of counted) {
          // … 1 lines
          if (!known) { legs.set(seg, { rate: (n * 3600) / TAU, at: this.second }); continue; }
          // … 1 lines
          known.rate = aged + (n * 3600 - aged) / TAU;
        // … 1 lines
        }
      }
    // … 1 lines
    }
  }

// src/sim/roads/flowStats.ts:94-97 slice of src/sim/roads/flowStats.ts#agedTo (shared): the feature's statements; "…" marks code left out
function agedTo(r: Rate, second: number): number {
  // … 1 lines
  return quiet <= 0 ? r.rate : r.rate * Math.pow(KEEP, quiet);
}
