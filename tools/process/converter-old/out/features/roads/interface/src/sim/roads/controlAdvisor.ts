// What the feature uses of src/sim/roads/controlAdvisor.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/roads/controlAdvisor.ts:5-5 type AutoControl
/** THE CONTROL A JUNCTION LEFT ON "AUTOMATIC" GETS: chosen from the measured trend of its flows */
export type AutoControl = 'signal' | 'stop' | 'priority';

// src/sim/roads/controlAdvisor.ts:7-7 variable F
const F = ROAD_TUNING.flow;

// src/sim/roads/controlAdvisor.ts:10-17 function warrantedControl
/** The control a junction's flows warrant, given what it has now (hysteresis). */
export function warrantedControl(major: number, minorMax: number, minorTotal: number, current: AutoControl): AutoControl;

// src/sim/roads/controlAdvisor.ts:19-19 function rank
const rank = (c: AutoControl): number => …;

// src/sim/roads/controlAdvisor.ts:22-22 variable QUIET_FLOW
/** Vehicles an hour into a junction below which `warrantedControl` is 'priority' whatever it has now (the least major flow of a step up, with the hysteresis). */
export const QUIET_FLOW = Math.min(F.stopMajor, F.signalMajor, F.signalMajorB) * Math.min(1, F.stepDown);

// src/sim/roads/controlAdvisor.ts:45-55 method ControlAdvisor.evaluate
/** One junction's evaluation; returns true when its choice changed. */
  evaluate(node: NodeId, major: number, minorMax: number, minorTotal: number, initial: AutoControl, now: number): boolean;
