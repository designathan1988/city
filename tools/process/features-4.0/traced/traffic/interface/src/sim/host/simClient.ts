// What the feature uses of src/sim/host/simClient.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/simClient.ts:84-755 class SimClient
export class SimClient implements JournalReader { … }

// src/sim/host/simClient.ts:192-192 field SimClient.clock
/** The player's settings of the simulation, sent with every frame (`frame`). */
  readonly clock = { speed: 1, paused: false, client: this as SimClient, get time(): number { return this.client.time; } };

// src/sim/host/simClient.ts:308-311 method SimClient.window
/** The micro window round (x, y); `whole`: the page's copy of the meso links is new, every link to be sent (`render/densityCars.ts`). */
  window(x: number, y: number, r: number, whole = false): void;

// src/sim/host/simClient.ts:323-323 field SimClient.mesoDeltas
/** The meso deltas not taken yet, and the window last told (asked again whole when the deltas were let go). */
  private mesoDeltas: MesoDelta[] = [];
