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

// src/sim/host/simClient.ts:55-55 type PedRead
/** A walker's view as the page makes it (`readTick`): the tick it was last read in, as a vehicle's. */
type PedRead = PedView & { tickRead: number };

// src/sim/host/simClient.ts:84-755 class SimClient
export class SimClient implements JournalReader { … }

// src/sim/host/simClient.ts:129-129 field SimClient.pedViews
readonly pedViews: PedView[] = [];

// src/sim/host/simClient.ts:229-229 field SimClient.casualtyList
private casualtyList: Casualty[] = [];

// src/sim/host/simClient.ts:251-255 method SimClient.casualties
/** The casualties of blows, their clocks advanced by `dt`, a minute's worth kept (`people/casualties.ts` impactCasualties). */
  casualties(dt = 0): readonly Casualty[];

// src/sim/host/simClient.ts:548-707 method SimClient.readTick
/** The frame the worker wrote: read from its slot (marked as being read), its rich parts from the message. */
  private readTick(message: Extract<FromSim, { type: 'tick' }>): void;
