// What the feature uses of src/sim/roads/controlAdvisor.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/roads/controlAdvisor.ts:5-5 type AutoControl
/** THE CONTROL A JUNCTION LEFT ON "AUTOMATIC" GETS: chosen from the measured trend of its flows */
export type AutoControl = 'signal' | 'stop' | 'priority';

// src/sim/roads/controlAdvisor.ts:24-65 class ControlAdvisor
export class ControlAdvisor { … }

// src/sim/roads/controlAdvisor.ts:32-34 method ControlAdvisor.choice
/** The control chosen for `node`, or undefined before the advisor has spoken (the game's geometric default holds). */
  choice(node: NodeId): AutoControl | undefined;
