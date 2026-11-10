// The code of src/sim/roads/controlAdvisor.ts that is the feature's (the rest of the file is shared).

// src/sim/roads/controlAdvisor.ts:7-7 slice of src/sim/roads/controlAdvisor.ts#F (shared): the feature's statements; "…" marks code left out
const F = ROAD_TUNING.flow;

// src/sim/roads/controlAdvisor.ts:10-17 slice of src/sim/roads/controlAdvisor.ts#warrantedControl (shared): the feature's statements; "…" marks code left out
export function warrantedControl(major: number, minorMax: number, minorTotal: number, current: AutoControl): AutoControl {
  // … 1 lines
  const signal = (major >= F.signalMajor * k('signal') && minorMax >= F.signalMinor * k('signal')) ||
    (major >= F.signalMajorB * k('signal') && minorMax >= F.signalMinorB * k('signal'));
  if (signal) return 'signal';
  const stop = major >= F.stopMajor * k('stop') && minorTotal >= F.stopMinor * k('stop');
  return stop ? 'stop' : 'priority';
}

// src/sim/roads/controlAdvisor.ts:11-11 slice of src/sim/roads/controlAdvisor.ts#warrantedControl.k (shared): the feature's statements; "…" marks code left out
  const k = (step: AutoControl): number => (rank(current) >= rank(step) ? F.stepDown : 1);

// src/sim/roads/controlAdvisor.ts:22-22 slice of src/sim/roads/controlAdvisor.ts#QUIET_FLOW (shared): the feature's statements; "…" marks code left out
export const QUIET_FLOW = Math.min(F.stopMajor, F.signalMajor, F.signalMajorB) * Math.min(1, F.stepDown);

// src/sim/roads/controlAdvisor.ts:45-55 slice of src/sim/roads/controlAdvisor.ts#ControlAdvisor.evaluate (shared): the feature's statements; "…" marks code left out
  evaluate(node: NodeId, major: number, minorMax: number, minorTotal: number, initial: AutoControl, now: number): boolean {
    // … 2 lines
    if (now < F.trendWindow) return false;
    if (now - (this.changedAt.get(node) ?? -Infinity) < F.trendWindow) return false;
  // … 5 lines
  }
