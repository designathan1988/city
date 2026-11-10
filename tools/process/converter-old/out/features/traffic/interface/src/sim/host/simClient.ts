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

// src/sim/host/simClient.ts:58-66 interface SignalFeed
/** A junction's signal heads and their lights as the worker last told them (`render/signals.ts`). */
export interface SignalFeed {
  /** The junctions whose heads moved since `serial` (null: every junction), and the serial now. */
  changesSince(serial: number): { readonly serial: number; readonly nodes: readonly NodeId[] | null };
  layout(node: NodeId): SignalLayout | undefined;
  /** A group's colour, and the junction's stage key (moves when any of its lights does). */
  state(node: NodeId, group: number): SignalState;
  stage(node: NodeId): string;
  nodes(): Iterable<NodeId>;
}

// src/sim/host/simClient.ts:84-755 class SimClient
export class SimClient implements JournalReader { … }

// src/sim/host/simClient.ts:103-103 field SimClient.pending
private readonly pending = new Map<number, (value: unknown) => void>();

// src/sim/host/simClient.ts:104-104 field SimClient.nextAsk
private nextAsk = 1;

// src/sim/host/simClient.ts:113-113 field SimClient.traffic
/** The traffic and the cars parked in bays, whole; DO NOT walk them per frame or per query: ask by place (`vehiclesIn`). */
  readonly traffic: VehicleView[] = [];

// src/sim/host/simClient.ts:114-114 field SimClient.parked
readonly parked: VehicleView[] = [];

// src/sim/host/simClient.ts:120-120 field SimClient.parkedGrid
private readonly parkedGrid = new FleetGrid<VehicleView>();

// src/sim/host/simClient.ts:123-123 field SimClient.parkedSlack
private parkedSlack = 0;

// src/sim/host/simClient.ts:129-129 field SimClient.pedViews
readonly pedViews: PedView[] = [];

// src/sim/host/simClient.ts:131-131 field SimClient.onVehicleGone
/** Told each view let go, traffic or parked, as it goes (the casting forgets its people, `render/agents.ts`): DO NOT make the casting walk every vehicle it cast to find the gone ones. */
  onVehicleGone: ((view: VehicleView) => void) | null = null;

// src/sim/host/simClient.ts:138-144 method SimClient.vehiclesIn
/** The views that may be drawn in the box this frame, appended to `out` after clearing it: the traffic, then the parked cars (`which` keeps one of them). */
  vehiclesIn(box: Readonly<GroundBox>, out: VehicleView[], which: 'all' | 'traffic' | 'parked' = 'all'): VehicleView[];

// src/sim/host/simClient.ts:158-169 method SimClient.dropParked
private dropParked(view: VehicleView): void;

// src/sim/host/simClient.ts:172-181 method SimClient.clearParked
/** Another map: every parked view let go at once. */
  private clearParked(): void;

// src/sim/host/simClient.ts:185-185 field SimClient.time
/** The simulation's clock as last told: seconds, the city's minutes, the tick's share gone by (`alpha`). */
  time = 0;

// src/sim/host/simClient.ts:192-192 field SimClient.clock
/** The player's settings of the simulation, sent with every frame (`frame`). */
  readonly clock = { speed: 1, paused: false, client: this as SimClient, get time(): number { return this.client.time; } };

// src/sim/host/simClient.ts:207-207 field SimClient.lights
private readonly lights = new Map<NodeId, { stage: string; readonly states: Map<number, SignalState> }>();

// src/sim/host/simClient.ts:226-226 field SimClient.tick
/** The frames read so far (a tick of the worker each). */
  tick = 0;

// src/sim/host/simClient.ts:259-282 method SimClient.start
/** The worker made and given the map; resolved once it stands. */
  /** `lanes`: the page's job lanes for the worker's bays and conflict pairs (`main.ts`, `jobPool.ts` jobChannel), handed before `init`; none: it opens its own. */
  start(seed: number, zones: ReadonlyMap<string, Float64Array | null>, zonesPrefix: string | null, ambient: 'view' | 'edges',
    lanes: { readonly bays: readonly MessagePort[]; readonly conflicts: readonly MessagePort[] } | null = null): Promise<void>;

// src/sim/host/simClient.ts:290-294 method SimClient.load
/** Another map (opened, generated, new): the worker's copy made whole again (only what traffic reads), the simulation started over. */
  load(): void;

// src/sim/host/simClient.ts:297-300 method SimClient.resync
/** The same map whole again, when the diary cannot say what changed: the worker's copy brought up to it, its traffic and the views kept (SIM-18). */
  private resync(): void;

// src/sim/host/simClient.ts:308-311 method SimClient.window
/** The micro window round (x, y); `whole`: the page's copy of the meso links is new, every link to be sent (`render/densityCars.ts`). */
  window(x: number, y: number, r: number, whole = false): void;

// src/sim/host/simClient.ts:323-323 field SimClient.mesoDeltas
/** The meso deltas not taken yet, and the window last told (asked again whole when the deltas were let go). */
  private mesoDeltas: MesoDelta[] = [];

// src/sim/host/simClient.ts:327-333 method SimClient.keepMeso
/** A tick's meso delta kept for the page: a whole one lets go of those before it; DO NOT keep them without bound when nobody takes them (the far cars not drawn) - let go, and every link asked for again. */
  private keepMeso(d: MesoDelta): void;

// src/sim/host/simClient.ts:404-406 method SimClient.junction
junction(node: NodeId): JunctionAnswer | null;

// src/sim/host/simClient.ts:423-425 method SimClient.send
private send(message: ToSim): void;

// src/sim/host/simClient.ts:442-506 method SimClient.sendChanges
/** The map's changes the worker's copy follows, since the last sent: by the records the diary names, or the whole map. */
  private sendChanges(): void;

// src/sim/host/simClient.ts:548-707 method SimClient.readTick
/** The frame the worker wrote: read from its slot (marked as being read), its rich parts from the message. */
  private readTick(message: Extract<FromSim, { type: 'tick' }>): void;

// src/sim/host/simClient.ts:710-733 method SimClient.applyParked
/** The parked cars' numbers (`PK`) applied by id: a known view written in place, a new one made, a gone one let go; with `whole`, those not named let go too. */
  private applyParked(told: ParkedChanges, stamp: number): void;

// src/sim/host/simClient.ts:735-754 method SimClient.clearViews
private clearViews(): void;

// src/sim/host/simClient.ts:770-776 function newView
function newView(id: number): VehicleView;
