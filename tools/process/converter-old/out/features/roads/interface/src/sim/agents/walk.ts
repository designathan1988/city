// What the feature uses of src/sim/agents/walk.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/walk.ts:32-32 variable LOOK
/** How far ahead others are looked for. */
const LOOK = m(8);

// src/sim/agents/walk.ts:96-96 variable CELL
/** Spatial cells for the walkers and for the walkways. */
/** As wide as a walker looks ahead (`LOOK`) */
const CELL = LOOK;

// src/sim/agents/walk.ts:122-122 variable walkWork
/** The work of the walkers' derived products, counted (counts, not times: the same on every machine), for the scale specs */
export const walkWork = { hailLooked: 0, routeNodes: 0, parkedFiled: 0 };

// src/sim/agents/walk.ts:139-253 class ParkedZoneIndex
/** DO NOT file every parked car again when one of them changes */
export class ParkedZoneIndex { … }

// src/sim/agents/walk.ts:259-272 interface Step
interface Step {
  /** The walkway, or null for a straight stretch off it (to a door or a car). */
  readonly way: Walkway | null;
  /** +1 along the way from `a` to `b`, -1 the other way. */
  readonly dir: 1 | -1;
  /** Arc position on the way where the step starts, and where it ends. */
  readonly from: number;
  readonly to: number;
  /** Off the walkways: the straight line walked. */
  readonly a?: Vec2;
  readonly b?: Vec2;
  /** A stretch inside a lot, between its door and its people's gate (`agents/lotDoors.ts`) */
  readonly lot?: true;
}

// src/sim/agents/walk.ts:274-346 interface Walker
interface Walker {
  readonly id: number;
  readonly trip: number;
  readonly view: PedView;
  readonly pace: number;
  readonly steps: Step[];
  leg: number;
  /** Progress along this step, and the offset left of it: the body projected on it. */
  s: number;
  d: number;
  /** The stripe it walks in: offset left of the way walked. */
  aim: number;
  v: number;
  x: number; y: number; heading: number;
  prevX: number; prevY: number; prevHeading: number;
  turnV: number;
  age: number;
  /** Seconds held still while meaning to walk. */
  held: number;
  /** Where along its step it last made progress (`PROGRESS`), and how long ago, s. */
  progress?: { leg: number; s: number; since: number } | undefined;
  /** The step it got jammed on (held past JAM_AFTER) */
  jammedOn?: number | undefined;
  /** Age at which its stripe (`aim`) last changed (`AIM_HOLD`). */
  aimAt?: number | undefined;
  /** Waiting for a zebra: seconds waited, and the time to the next ask. */
  waited: number;
  asked: number;
  /** The zebra it has been let onto. */
  granted: CrossingId | null;
  /** When (its `age`) it was let onto it: a leave to step off now, not for good (`GRANT_LIFE`). */
  grantedAt?: number | undefined;
  /** Not out yet: waiting inside the door (or the car) until the spot it steps out onto is clear. */
  inside: boolean;
  /** The zebra it is coming up to, and the one it stands waiting for. */
  zebra: Zebra | null;
  waiting: Zebra | null;
  done: boolean;
  /** Moved by the player's hand, not by its route (`movePlayerWalker`). */
  player: boolean;
  /** Stopped to do something a while (talk, fall down), facing a point: until `age` reaches `until`. */
  act: { readonly kind: GestureKind; readonly from: number; readonly until: number; readonly faceX: number; readonly faceY: number } | null;
  /** Running away from something: pace times `by` until `age` reaches `until`; then on to `goal`. */
  rush: { readonly by: number; readonly until: number; readonly goal: Vec2 } | null;
  /** Terrified (a blow, a robbery) until `age` reaches this: the face screams, zebras are not waited at. */
  fright?: number | undefined;
  /** When somebody running in panic trips and falls (age, seconds), once. */
  tripAt?: number | undefined;
  /** A limb lost to a blow (`PedView.maimed`). */
  maimed?: 'armL' | 'armR' | 'legL' | 'legR' | undefined;
  /** Health, 100 whole (`shot`); absent: never hurt. */
  hp?: number | undefined;
  /** Shots taken. */
  hits?: number | undefined;
  /** Shot open at the belly already (`shot`: the guts out once). */
  opened?: boolean | undefined;
  /** The walker they walk with (a companion keeps to their side, `stepWalkers`). */
  leader?: number | undefined;
  /** Down and to crawl off once the body is on hands and knees (`getUp` crawl). */
  crawlNext?: boolean | undefined;
  /** The last shot that struck them: where, how much it took, when (their age). */
  lastHit?: { part: BodyPart; damage: number; at: number } | undefined;
  /** Damage taken by each part, for a limb shot off once it has taken enough. */
  hurt?: Partial<Record<BodyPart, number>> | undefined;
  /** Every limb lost (`PedView.lost`). */
  lost?: Severable[] | undefined;
  /** The step its view's `stretch` was last spelt for (`publish`). */
  stretchOf?: Step | undefined;
  /** The act its view's `gesture` was made for (`publish`): made again only when the act changes, its clock kept by the page (`SimClient.readTick`). */
  gestureOf?: Walker['act'] | undefined;
  /** Health lost a second to a wound bleeding (a limb gone): they die of it. */
  bleeding?: number | undefined;
}

// src/sim/agents/walk.ts:355-384 interface State
interface State {
  graph: WalkGraph | null;
  /** The graph's state its index was brought up to (`WalkGraph.changesSince`). */
  graphState: object | null;
  wayIndex: Map<number, number[]>;
  walkers: Walker[];
  byId: Map<number, Walker>;
  /** For each car holding (or asking for) its way across the footway, the walkers who were on that ground when it took it */
  onCarWay: Map<number, Set<number>>;
  /** Shocking events (as GTA's peds have them) */
  shocks: { x: number; y: number; radius: number; until: number }[];
  /** Seconds of walking stepped (the shocks' clock). */
  clock: number;
  /** When the walkers were last checked against the shocks. */
  shockLook: number;
  /** The walkers by cell for the cars' questions */
  near: WalkerGrid;
  nearAt: number;
  /** Each crossing's state of the last tick (`publish`), taken back if it has somebody again. */
  crossingPool: Map<CrossingId, CrossingState>;
  /** The fastest vehicle's speed for `gapOpen`, read once a tick when somebody first asks. */
  trafficAt: number;
  fastest: number;
  /** The walkers by cell, filed again at the start of every step (`WalkerGrid`). */
  grid: WalkerGrid;
  /** The parked cars' zones by cell, kept as the cars come, go and move (`ParkedZoneIndex`). */
  parked: ParkedZoneIndex;
  /** DO NOT find them in `walkers`: every walker of the map, for each companion made. */
  parties: Map<number, Walker[]>;
}

// src/sim/agents/walk.ts:834-834 interface Zebra
/** A crossing on a walker's way: a zebra as the vehicles and the signals know it (`edge`), or a crossing with no zebra */
interface Zebra { readonly id: CrossingId; readonly edge: SidewalkEdge | null; readonly way: Walkway }

// src/sim/agents/walk.ts:911-911 variable HAIL_REACH
/** How far from a lane's centre somebody on the footway may be hailed from (`hailable`). */
const HAIL_REACH = m(9);

// src/sim/agents/walk.ts:917-1192 function createAgentWalkEngine
export function createAgentWalkEngine(): PedestrianEngine;

// src/sim/agents/walk.ts:1369-1434 class WalkerGrid
/** The walkers out on the street by cell of `CELL`, as Matthias Müller's dense spatial hash files particles */
class WalkerGrid { … }

// src/sim/agents/walk.ts:1437-1443 function nearNow
/** The walkers by cell for the cars' questions, filed at the first one after the walkers stepped (`State.near`) */
function nearNow(s: State): WalkerGrid;
