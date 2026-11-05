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

// src/sim/host/simClient.ts:197-197 getter SimClient.paused
/** The simulation stopped by the player, as the last frame sent it. */
  get paused(): boolean;

// src/sim/host/simClient.ts:201-201 getter SimClient.topologyRevision
/** The topology the worker's simulation stands at (the network's `trafficRevision` it caught up to). */
  get topologyRevision(): number;

// src/sim/host/simClient.ts:336-357 method SimClient.frame
/** A frame of the page: the map's changes since the last sent, then the wall time and the controls (one frame asked at a time). */
  frame(wall: number, hold: boolean, run: { readonly traffic: boolean; readonly pedestrians: boolean; readonly maxShare: number; readonly startShare?: number; readonly opening?: boolean }): void;

// src/sim/host/simClient.ts:373-384 method SimClient.asked
/** A panel's question: the last answer to it now (null before the first), a fresh one asked for the next refresh. */
  asked(ask: SimAsk): unknown;

// src/sim/host/simClient.ts:400-402 method SimClient.segmentTraffic
segmentTraffic(id: SegmentId): TrafficCount | null;

// src/sim/host/simClient.ts:411-421 field SimClient.signals
readonly signals: SignalFeed = {
    changesSince: (serial) => {
      const now = this.signalBase + this.signalLog.length;
      if (serial < this.signalAll || serial < this.signalBase || serial > now) return { serial: now, nodes: null };
      return { serial: now, nodes: this.signalLog.slice(serial - this.signalBase) };
    },
    layout: (node) => this.layouts.get(node),
    state: (node, group) => this.lights.get(node)?.states.get(group) ?? 'red',
    stage: (node) => this.lights.get(node)?.stage ?? '',
    nodes: () => this.layouts.keys(),
  };

// src/sim/host/simClient.ts:423-425 method SimClient.send
private send(message: ToSim): void;
