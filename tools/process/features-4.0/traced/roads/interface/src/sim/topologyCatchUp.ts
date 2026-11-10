// What the feature uses of src/sim/topologyCatchUp.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/topologyCatchUp.ts:15-22 method TopologyCatchUp.stepWindow
/** The micro window's move (`SimWorld.moveWindowSteps`), as far as it goes this frame without holding the simulation; none while the topology catches up. */
  stepWindow(sim: SimWorld, revision: number): void;

// src/sim/topologyCatchUp.ts:25-43 method TopologyCatchUp.step
/** This frame's work towards `revision` (the network's `trafficRevision`). */
  step(sim: SimWorld, revision: number, worldBusy: boolean): boolean;
