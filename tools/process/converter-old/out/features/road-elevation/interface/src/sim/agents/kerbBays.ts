// What the feature uses of src/sim/agents/kerbBays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/kerbBays.ts:97-97 interface Box
interface Box { x0: number; y0: number; x1: number; y1: number }

// src/sim/agents/kerbBays.ts:127-137 method EntranceBook.box
/** The box of a building's entrances kept; null for none. */
  box(id: BuildingId): Box | null;
