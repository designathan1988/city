// What the feature uses of src/sim/host/simClient.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/simClient.ts:28-52 interface VehicleView
/** THE PAGE'S SIDE OF THE SIMULATION'S WORKER (`simHost.ts`): the map's changes sent, the frame read, the panels' questions asked */

/** A vehicle as the page draws it: its numbers of the last tick and what it carries (`VehicleRich`). */
export interface VehicleView {
  readonly id: number;
  archetype: Archetype;
  color: string;
  doors: readonly number[];
  kerbStop: KerbStop | null;
  people: readonly number[];
  peopleAge: readonly (PersonAgeClass | undefined)[];
  seats: number;
  slain: boolean;
  age: number;
  v: number;
  readonly prev: { v: number };
  /** Off the road: its pose a tick ago and now, and the lot it stands on; null on a lane. */
  free: { px: number; py: number; pangle: number; x: number; y: number; angle: number; lot: number | null } | null;
  readonly frames: VehicleFrames;
  /** The road its lane belongs to (undefined on a junction's connector), and the road it came from on one. */
  segment: SegmentId | undefined;
  inSegment: SegmentId | undefined;
  indicator: -1 | 0 | 1;
  curvature: number;
  odometer: number;
  /** The client's own: the tick it was last read in (`readTick`), those not read in a tick being gone. */
  tickRead: number;
}

// src/sim/host/simClient.ts:84-755 class SimClient
export class SimClient implements JournalReader { … }

// src/sim/host/simClient.ts:387-392 method SimClient.lotPlan
/** The zoning tool's proposed lots, planned in the worker on its copy of the region (`SimHost.lotPlan`): the diary sent first, so it plans the land as the page has it now; null when not now (no worker, a tile's network still read there). */
  lotPlan(): Promise<LotPlan | null>;

// src/sim/host/simClient.ts:432-439 method SimClient.markSent
private markSent(): void;

// src/sim/host/simClient.ts:442-506 method SimClient.sendChanges
/** The map's changes the worker's copy follows, since the last sent: by the records the diary names, or the whole map. */
  private sendChanges(): void;

// src/sim/host/simClient.ts:509-525 method SimClient.keyChanges
/** The worker's keys brought to the store's: each stretch no longer there taken out (from the end, so the places before it hold), then the keys it lacks put at its end. */
  private keyChanges(): Restoration['lists'][number][];

// src/sim/host/simClient.ts:789-796 function recordOf
/** A record of the map by its part, for the worker's copy (never a building's: `buildingChangesOf`). */
function recordOf(doc: RoadDoc, part: RecordPart, id: number): unknown;
