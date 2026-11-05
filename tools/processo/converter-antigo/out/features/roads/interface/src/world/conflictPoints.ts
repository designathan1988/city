// What the feature uses of src/world/conflictPoints.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/conflictPoints.ts:27-40 interface ConflictPoint
export interface ConflictPoint {
  readonly id: number;
  readonly node: NodeId;
  readonly a: ConnectorId;
  readonly b: ConnectorId;
  /** Earliest arc position of the zone along connector `a`, body front. */
  readonly sA: number;
  /** Earliest arc position of the zone along connector `b`, body front. */
  readonly sB: number;
  readonly kind: ConflictKind;
  readonly at: Vec2;
  /** Zone on `a` (or `b`) for a body of class `mine` against a body of class `theirs` on the other movement */
  zone(on: ConnectorId, mine: BodyClass, theirs: BodyClass): Zone | null;
}

// src/world/conflictPoints.ts:43-488 class ConflictIndex
/** Where two movements through a junction physically conflict. */
export class ConflictIndex { … }

// src/world/conflictPoints.ts:46-46 field ConflictIndex.byConnector
/** References held by each connector, sorted by arc position. */
  readonly byConnector = new Map<ConnectorId, ConflictRef[]>();

// src/world/conflictPoints.ts:48-48 field ConflictIndex.diverges
/** Sibling movements from the same lane whose bodies overlap near the split. */
  readonly diverges = new Map<ConnectorId, Diverge[]>();

// src/world/conflictPoints.ts:50-50 field ConflictIndex.queueIntrusions
/** Movements whose zone reaches a body still waiting behind its stop line. */
  readonly queueIntrusions: { a: ConnectorId; b: ConnectorId; mine: BodyClass; theirs: BodyClass }[] = [];

// src/world/conflictPoints.ts:85-92 method ConflictIndex.setWindow
/** The window moved: the junctions it left let go (their pairs with them, `unuse`), those it reached built (`update`); false when none changed. */
  setWindow(graph: LaneletGraph, next: ReadonlySet<NodeId>): ReadonlySet<NodeId>;

// src/world/conflictPoints.ts:100-104 method ConflictIndex.entering
/** The junctions of `next` this index does not hold yet: what a window moved to must have prepared first (`SimWorld.moveWindowSteps`). */
  entering(graph: LaneletGraph, next: ReadonlySet<NodeId>): Set<NodeId>;

// src/world/conflictPoints.ts:141-201 method ConflictIndex.prepare
*prepare(graph: LaneletGraph, measurer?: PairMeasurer, only?: ReadonlySet<NodeId> | null, around?: ReadonlySet<NodeId>): Generator<void | typeof WAITING, void>;

// src/world/conflictPoints.ts:242-279 method ConflictIndex.sampleOffPage
/** DO NOT sample them on the page when there are workers */
  private *sampleOffPage(graph: LaneletGraph, junctions: readonly JunctionTopology[], measurer: PairMeasurer): Generator<void | typeof WAITING, void>;

// src/world/conflictPoints.ts:286-297 method ConflictIndex.takePrepared
/** The junction's record from `prepare` (`prepared`), if it was made from the very junction and lanes built now. */
  private takePrepared(graph: LaneletGraph, junction: JunctionTopology, previousSweeps: Map<string, Float64Array>, nextSweeps: Map<string, Float64Array>): PreparedJunction | null;

// src/world/conflictPoints.ts:299-299 field ConflictIndex.ofJunction
private readonly ofJunction = new Map<NodeId, { readonly points: number[]; readonly connectors: ConnectorId[]; readonly pairs: string[]; readonly sweeps: string[] }>();

// src/world/conflictPoints.ts:304-326 method ConflictIndex.build
build(graph: LaneletGraph): void;

// src/world/conflictPoints.ts:329-362 method ConflictIndex.update
/** DO NOT build the index of the whole map for an edit */
  update(graph: LaneletGraph, junctions: ReadonlySet<NodeId>): void;

// src/world/conflictPoints.ts:365-428 method ConflictIndex.buildJunction
/** One junction's conflicts, from the graph and the caches, with what it made recorded (`ofJunction`). */
  private buildJunction(graph: LaneletGraph, junction: JunctionTopology,
    previous: Map<string, CachedPair>, next: Map<string, CachedPair>,
    previousSweeps: Map<string, Float64Array>, nextSweeps: Map<string, Float64Array>): void;

// src/world/conflictPoints.ts:430-432 method ConflictIndex.recordIntrusions
private recordIntrusions(a: ConnectorId, b: ConnectorId, facts: PairFacts): void;

// src/world/conflictPoints.ts:434-473 method ConflictIndex.add
private add(
    node: NodeId,
    a: ConnectorId,
    b: ConnectorId,
    kind: ConflictKind,
    at: Vec2,
    zones: PairZones,
    facts: PairFacts,
  ): number;

// src/world/conflictPoints.ts:475-477 method ConflictIndex.refs
refs(connector: ConnectorId): readonly ConflictRef[];

// src/world/conflictPoints.ts:479-481 method ConflictIndex.divergesOf
divergesOf(connector: ConnectorId): readonly Diverge[];

// src/world/conflictPoints.ts:501-505 interface Sweep
/** A movement's sweep (`conflictTable.ts` `SweepSamples`) and the lanelets it was sampled along. */
interface Sweep extends SweepSamples {
  readonly crossing: Lanelet;
  /** Approach, movement and exit joined, parametrised from the stop line. */
  readonly path: { sampleAt(c: number): { p: Vec2; t: Vec2 } };
}

// src/world/conflictPoints.ts:507-529 function sweepOf
function sweepOf(graph: LaneletGraph, c: Connector,
  previous: Map<string, Float64Array>, next: Map<string, Float64Array>, used?: string[]): Sweep | null;

// src/world/conflictPoints.ts:532-534 function sweepKey
/** The key of a sweep's frame in `sweepCache`: its three centrelines' digests and its sample count. */
function sweepKey(inbound: number, crossing: number, outbound: number, count: number): string;

// src/world/conflictPoints.ts:536-536 variable FRAME_LENGTHS
const FRAME_LENGTHS = new WeakMap<Float64Array, Float64Array>();

// src/world/conflictPoints.ts:539-548 interface SweepShape
/** Everything `pairTable` reads from a sweep. Two equal shapes give equal zones. */
interface SweepShape {
  /** Hash of the fields below, for the cache key; equality is still checked in full. */
  readonly hash: string;
  readonly c0: number;
  readonly count: number;
  readonly length: number;
  readonly frame: Float64Array;
  /** The crossing's own centreline, which `firstCrossing` reads between samples. */
  readonly centre: Float64Array;
}

// src/world/conflictPoints.ts:550-562 interface CachedPair
interface CachedPair {
  readonly a: SweepShape;
  readonly b: SweepShape;
  readonly zones: PairZones | null;
  /** The table the zones read (`pairTable`): what a worker finishes the pair from (`finishOffPage`). */
  readonly table: Float64Array | null;
  /** Where the two centrelines first cross; undefined until asked (`buildJunction`, `finishOffPage`). */
  crossing?: Crossing | null;
  /** Middle of the heavy-body zone on `a`; undefined until asked. */
  centre?: Vec2;
  /** What the index makes of the zones (`pairFacts`); undefined until asked (`buildJunction`, `finishOffPage`). */
  facts?: PairFacts;
}

// src/world/conflictPoints.ts:565-573 interface Movement
/** A movement through a junction, swept: its connector, its sweep and shape and the key its frame is kept under. */
interface Movement {
  readonly connector: Connector;
  /** The lanes `prepare` swept it along (`takePrepared` checks they are the graph's); unset when built here. */
  readonly inbound: Lanelet | undefined;
  readonly outbound: Lanelet | undefined;
  readonly sweep: Sweep;
  readonly shape: SweepShape;
  readonly sweepKey: string;
}

// src/world/conflictPoints.ts:576-581 interface PreparedJunction
/** A junction as `prepare` read it (`ConflictIndex.prepared`): its connectors, its swept movements, its pairs' keys in `buildJunction`'s order. */
interface PreparedJunction {
  readonly junction: JunctionTopology;
  readonly ids: readonly ConnectorId[];
  readonly movements: readonly Movement[];
  readonly pairKeys: readonly string[];
}

// src/world/conflictPoints.ts:584-584 variable LINE_DIGESTS
/** Each centreline's digest, kept with its point list (`sweepOf`; made by a worker, `sampleOffPage`). */
const LINE_DIGESTS = new WeakMap<Float64Array, number>();

// src/world/conflictPoints.ts:585-592 function lineDigest
function lineDigest(xy: Float64Array): number;

// src/world/conflictPoints.ts:595-595 variable SHAPES
/** The shape of a sweep, kept with its frame */
const SHAPES = new WeakMap<Float64Array, SweepShape>();

// src/world/conflictPoints.ts:596-604 function shapeOf
function shapeOf(s: Sweep): SweepShape;

// src/world/conflictPoints.ts:606-609 function measureShape
function measureShape(s: Sweep): SweepShape;

// src/world/conflictPoints.ts:612-621 function shapeFrom
/** A sweep's shape from its digest (`shapeDigest`, taken here or by a worker). */
function shapeFrom(digest: number, c0: number, count: number, crossing: Lanelet, frame: Float64Array): SweepShape;

// src/world/conflictPoints.ts:623-627 function sameShape
function sameShape(p: SweepShape, q: SweepShape): boolean;

// src/world/conflictPoints.ts:640-645 interface KeptZones
/** The zones kept from earlier sessions and those measured in this one, by pair key */
interface KeptZones {
  readonly kept: Map<string, Float64Array | null>;
  readonly fresh: Map<string, Float64Array | null>;
  /** What the pairs measured here are measured in (`pairTable`). */
  readonly work: PairWork;
}

// src/world/conflictPoints.ts:648-652 function measurePair
/** A pair measured, or read back from an earlier session. */
function measurePair(key: string, a: Sweep, shapeA: SweepShape, b: Sweep, shapeB: SweepShape, store: KeptZones): CachedPair;

// src/world/conflictPoints.ts:664-682 function cachedPair
/** A pair's zones, answered from the previous build when neither sweep moved. */
function cachedPair(
  previous: Map<string, CachedPair>,
  next: Map<string, CachedPair>,
  key: string,
  a: Sweep,
  shapeA: SweepShape,
  b: Sweep,
  shapeB: SweepShape,
  store: KeptZones,
): CachedPair;

// src/world/conflictPoints.ts:684-687 function zoneCentre
function zoneCentre(a: Sweep, zones: PairZones): Vec2;
