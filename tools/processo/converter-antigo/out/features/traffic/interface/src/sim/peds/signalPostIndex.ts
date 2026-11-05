// What the feature uses of src/sim/peds/signalPostIndex.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/peds/signalPostIndex.ts:17-17 variable POST_CELL
/** Cell of the posts' grid, world units: a walker's look ahead (`walk.ts` LOOK). */
const POST_CELL = m(8);

// src/sim/peds/signalPostIndex.ts:21-105 class SignalPostIndex
/** DO NOT ask the world for every post of the map to keep those round one walker */
export class SignalPostIndex { … }

// src/sim/peds/signalPostIndex.ts:22-22 field SignalPostIndex.serial
private serial = -1;

// src/sim/peds/signalPostIndex.ts:23-23 field SignalPostIndex.byNode
private readonly byNode = new Map<NodeId, SignalPost[]>();

// src/sim/peds/signalPostIndex.ts:24-24 field SignalPostIndex.cells
private readonly cells = new Map<number, SignalPost[]>();

// src/sim/peds/signalPostIndex.ts:29-33 method SignalPostIndex.reset
/** DO NOT keep the posts across a reset */
  reset(): void;
