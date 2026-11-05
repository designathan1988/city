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

// src/sim/host/simClient.ts:103-103 field SimClient.pending
private readonly pending = new Map<number, (value: unknown) => void>();

// src/sim/host/simClient.ts:104-104 field SimClient.nextAsk
private nextAsk = 1;

// src/sim/host/simClient.ts:129-129 field SimClient.pedViews
readonly pedViews: PedView[] = [];

// src/sim/host/simClient.ts:138-144 method SimClient.vehiclesIn
/** The views that may be drawn in the box this frame, appended to `out` after clearing it: the traffic, then the parked cars (`which` keeps one of them). */
  vehiclesIn(box: Readonly<GroundBox>, out: VehicleView[], which: 'all' | 'traffic' | 'parked' = 'all'): VehicleView[];

// src/sim/host/simClient.ts:185-185 field SimClient.time
/** The simulation's clock as last told: seconds, the city's minutes, the tick's share gone by (`alpha`). */
  time = 0;

// src/sim/host/simClient.ts:228-228 field SimClient.wounds
/** The wounds told since the effects last took them (`takeWounds`), and the casualties of blows, their clocks kept here. */
  private wounds: Wound[] = [];

// src/sim/host/simClient.ts:229-229 field SimClient.casualtyList
private casualtyList: Casualty[] = [];

// src/sim/host/simClient.ts:232-232 constructor SimClient.constructor
/** `net`: the page's network of `doc`, whose junctions and ribbons the worker takes as solved (`bake`, SIM-04); none: the worker solves them. */
  constructor(readonly doc: RoadDoc, private readonly net: Network | null = null);

// src/sim/host/simClient.ts:409-409 field SimClient.signalJunctions
/** The signalised junctions as the blows' posts read them (`world/signalPosts.ts`). */
  readonly signalJunctions = { junctions: new Map<NodeId, { readonly signalised: boolean; readonly groups: SignalLayout['groups'] }>() };

// src/sim/host/simClient.ts:423-425 method SimClient.send
private send(message: ToSim): void;

// src/sim/host/simClient.ts:548-707 method SimClient.readTick
/** The frame the worker wrote: read from its slot (marked as being read), its rich parts from the message. */
  private readTick(message: Extract<FromSim, { type: 'tick' }>): void;
