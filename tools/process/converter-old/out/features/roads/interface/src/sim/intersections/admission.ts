// What the feature uses of src/sim/intersections/admission.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/intersections/admission.ts:22-22 type RowClass
export type RowClass = 'signalGreen' | 'priority' | 'stop' | 'yield' | 'none';

// src/sim/intersections/admission.ts:24-31 interface Request
interface Request {
  readonly v: Vehicle;
  readonly conn: Connector;
  /** Distance from the vehicle's nose to the stop line. */
  readonly d: number;
  readonly requestTick: number;
  readonly row: RowClass;
}

// src/sim/intersections/admission.ts:33-39 interface Verdict
interface Verdict {
  readonly ok: boolean;
  readonly reason?: ObstacleKind;
  readonly reservations?: readonly ConnectorReservation[];
  /** A soft maximum claim was declared, but the red still forbids entry. */
  readonly reserveOnly?: boolean;
}

// src/sim/intersections/admission.ts:42-42 variable KEEP_DECLARED
/** A head on red whose declared chain stands: reserve only, keep `reservedConnectors`. */
const KEEP_DECLARED: Verdict = { ok: true, reserveOnly: true };

// src/sim/intersections/admission.ts:44-47 interface ConnectorReservation
interface ConnectorReservation {
  readonly connector: Connector;
  readonly points: readonly number[];
}

// src/sim/intersections/admission.ts:49-55 variable RANK
const RANK: Record<RowClass, number> = {
  signalGreen: 3,
  priority: 2,
  stop: 1,
  yield: 0,
  none: -1,
};

// src/sim/intersections/admission.ts:83-83 variable admittedBy
/** This pass's vehicles by the connector they were admitted to. Null outside `stepAdmission`. */
let admittedBy: Map<string, Vehicle[]> | null = null;

// src/sim/intersections/admission.ts:87-93 function granted
/** A vehicle granted something in this pass: it holds now, and is admitted to `conn`. */
function granted(v: Vehicle, conn: string | null): void;

// src/sim/intersections/admission.ts:96-96 variable pending
/** This pass's requests, by node. Null outside `stepAdmission`. */
let pending: Map<NodeId, Request[]> | null = null;

// src/sim/intersections/admission.ts:98-214 function admit
function admit(w: SimWorld, banker: BankerPass): void;

// src/sim/intersections/admission.ts:217-257 function revokeStaleGrants
/** Takes back a grant whose signal went red before the vehicle used it. */
function revokeStaleGrants(w: SimWorld): void;

// src/sim/intersections/admission.ts:266-347 function evaluate
function evaluate(w: SimWorld, r: Request, banker: BankerPass): Verdict;

// src/sim/intersections/admission.ts:350-361 function zipperHolds
/** Whether this vehicle leaves the next place in a merge to the other lane. */
function zipperHolds(w: SimWorld, r: Request): boolean;

// src/sim/intersections/admission.ts:364-381 function mergeFollows
/** Whether this vehicle may join a merge behind a holder on the other movement. */
function mergeFollows(
  w: SimWorld,
  r: Pick<Request, 'v' | 'conn' | 'd'>,
  point: ConflictPoint,
  claim: Claim,
): boolean;

// src/sim/intersections/admission.ts:384-392 function mergeRemaining
/** Distance from a vehicle's front to the start of the lane its movement runs into; negative once the front is past it. */
export function mergeRemaining(w: SimWorld, v: Vehicle, conn: Connector): number | null;

// src/sim/intersections/admission.ts:397-397 variable ZIP_READY
/** How close to its line a rolling head of the other lane must be to take its turn, world units (standing heads always may). */
const ZIP_READY = 8;

// src/sim/intersections/admission.ts:400-408 function outranksForExit
/** Whether a vehicle with a better right of way is waiting for the same lane out. */
function outranksForExit(r: Request): boolean;

// src/sim/intersections/admission.ts:411-443 function compactReservationChain
/** Movement chain that must be declared before entering `first`. */
export function compactReservationChain(
  w: SimWorld,
  v: Vehicle,
  first: Connector,
): Connector[] | null;

// src/sim/intersections/admission.ts:446-461 function movementReservedByOther
/** Whether a connector with no conflict points is already spoken for. */
function movementReservedByOther(w: SimWorld, v: Vehicle, conn: Connector): boolean;

// src/sim/intersections/admission.ts:465-476 function exitSwingBlocked
/** Whether a vehicle WAITING at another movement's stop line is inside the area this one would sweep. */
/** Whether a long body turning into a lane would sweep a vehicle beside it. */
function exitSwingBlocked(w: SimWorld, v: Vehicle, conn: Connector): boolean;

// src/sim/intersections/admission.ts:478-497 function queuedBodyInZone
function queuedBodyInZone(w: SimWorld, v: Vehicle, conn: Connector): boolean;

// src/sim/intersections/admission.ts:500-522 function stopShortOfIntrusion
/** Distance to stop at so the body stays outside a zone that reaches back over the stop line while another movement holds */
function stopShortOfIntrusion(w: SimWorld, v: Vehicle, conn: Connector, d: number): number | null;

// src/sim/intersections/admission.ts:525-547 function convoyCanEnter
/** A follower may share a same-path claim only at the stop line */
function convoyCanEnter(w: SimWorld, r: Request, conn: Connector): boolean;

// src/sim/intersections/admission.ts:550-550 type ResourceKey
/** A resource of the Banker's test as a number: a conflict point is its own index */
type ResourceKey = number;

// src/sim/intersections/admission.ts:554-558 function movementToken
function movementToken(connector: string): ResourceKey;

// src/sim/intersections/admission.ts:566-566 variable CONNECTOR_RESOURCES
/** Conflict points plus a connector token for zero-point movements. */
const CONNECTOR_RESOURCES = new WeakMap<readonly ConflictRef[], readonly ResourceKey[]>();

// src/sim/intersections/admission.ts:567-579 function connectorResources
function connectorResources(
  w: SimWorld,
  connector: Connector,
): readonly ResourceKey[];

// src/sim/intersections/admission.ts:582-588 function holdsAnything
/** Whether a vehicle holds anything at all, without building its sets. */
function holdsAnything(w: SimWorld, v: Vehicle): boolean;

// src/sim/intersections/admission.ts:590-603 function actualAllocation
function actualAllocation(w: SimWorld, v: Vehicle): Set<ResourceKey>;

// src/sim/intersections/admission.ts:612-666 class BankerPass
/** The Banker's tests' view of one admission pass (`stepAdmission` makes one and drops it at the end). */
class BankerPass { … }

// src/sim/intersections/admission.ts:669-671 variable admissionProbe
/** Each Banker's verdict with the applicant and the movements asked for: for the specs only (`tests/sim/admissionGroup.spec.ts`). */
export const admissionProbe: {
  banker: ((w: SimWorld, applicant: Vehicle, proposal: readonly Connector[], safe: boolean) => void) | null;
} = { banker: null };

// src/sim/intersections/admission.ts:674-683 function bankerSafeAfterGrant
/** DO NOT go back to iterating every holder (`holders`, `w.vehicles`) here. */
function bankerSafeAfterGrant(
  w: SimWorld,
  applicant: Vehicle,
  proposal: readonly ConnectorReservation[],
  banker: BankerPass,
): boolean;

// src/sim/intersections/admission.ts:808-822 function holderState
/** Where a claim holder's body is along the movement it holds. */
export function holderState(w: SimWorld, claim: Claim): HolderState | null;

// src/sim/intersections/admission.ts:825-874 function rightOfWay
/** Right of way for one movement. */
export function rightOfWay(
  w: SimWorld,
  node: NodeId,
  v: Vehicle,
  conn: Connector,
  distanceToStop: number,
): RowClass;

// src/sim/intersections/admission.ts:877-882 function throughRank
/** Authored priority, retaining the exact legacy class ordering on old roads. */
function throughRank(w: SimWorld, conn: Connector): number;

// src/sim/intersections/admission.ts:885-887 function continues
/** Whether a movement carries its road on through the node */
function continues(w: SimWorld, node: NodeId, conn: Connector): boolean;

// src/sim/intersections/admission.ts:890-890 variable mainRoads
/** Rank of the road a junction is on, per topology build. */
const mainRoads = new WeakMap<JunctionTopology, number>();

// src/sim/intersections/admission.ts:892-903 function mainRoadRank
function mainRoadRank(w: SimWorld, junction: JunctionTopology): number;

// src/sim/intersections/admission.ts:906-946 function hasAcceptableGap
/** Gap acceptance against the movements this one yields to. */
function hasAcceptableGap(w: SimWorld, r: Request): boolean;

// src/sim/intersections/admission.ts:949-960 function clearTime
/** Seconds for the FRONT to reach the point where the body has left the last conflict zone of this movement. */
function clearTime(w: SimWorld, r: Request): number;

// src/sim/intersections/admission.ts:963-968 function protectedNow
/** True when this movement is green and protected in the current stage. */
function protectedNow(w: SimWorld, conn: Connector): boolean;

// src/sim/intersections/admission.ts:971-976 function movementIsActive
/** True when the other movement currently has permission to run. */
function movementIsActive(w: SimWorld, other: Connector): boolean;

// src/sim/intersections/admission.ts:979-983 function stoppedForWalker
/** True when the vehicle stands for somebody on foot between its nose and its stop line */
function stoppedForWalker(v: Vehicle, toLine: number): boolean;

// src/sim/intersections/admission.ts:986-991 function crossingBusy
/** True when a pedestrian is on, or about to step onto, the stretch of a zebra this movement drives over (`CrossingSpans`). */
function crossingBusy(w: SimWorld, conn: Connector): boolean;

// src/sim/intersections/admission.ts:995-1001 function crossingLegs
function crossingLegs(w: SimWorld, node: NodeId): readonly { readonly segment: SegmentId; readonly id: string }[];

// src/sim/intersections/admission.ts:1005-1005 variable CLEAR_MARGIN
/** Allowance over the estimated time for a front to reach a zebra it turns across. */
const CLEAR_MARGIN = 1.2;

// src/sim/intersections/admission.ts:1008-1014 function travelTime
/** Seconds to cover `d` from `v0`, accelerating at `a` up to `top`. */
function travelTime(d: number, v0: number, top: number, a: number): number;

// src/sim/intersections/admission.ts:1017-1044 function crossingReachedFirst
/** Whether somebody already on a zebra this movement crosses will reach the stretch it drives over before its body has */
function crossingReachedFirst(w: SimWorld, r: Request): boolean;

// src/sim/intersections/admission.ts:1047-1066 function aheadOnMovement
/** The latest time, with `CLEAR_MARGIN`, at which a vehicle already admitted onto this movement */
function aheadOnMovement(w: SimWorld, r: Request, along: number, top: number): number;

// src/sim/intersections/admission.ts:1069-1100 function pedestrianHasPriority
/** Whether somebody waiting at a kerb has the right to cross in front of this movement before it is admitted */
function pedestrianHasPriority(w: SimWorld, r: Request): boolean;

// src/sim/intersections/admission.ts:1103-1110 function nextConnector
/** The connector this vehicle intends to take at the end of its lane. */
export function nextConnector(w: SimWorld, v: Vehicle): Connector | undefined;

// src/sim/intersections/admission.ts:1113-1120 function yieldSide
/** The side a movement gives way to at a junction whose rule is a side */
export function yieldSide(w: SimWorld, conn: Connector): 'right' | 'left' | null;

// src/sim/intersections/admission.ts:1123-1130 function comesFrom
/** Whether `other` arrives from `side` of `conn`'s driver: by the directions the two lanes come in on. */
export function comesFrom(w: SimWorld, conn: Connector, other: Connector, side: 'right' | 'left'): boolean;

// src/sim/intersections/admission.ts:1132-1139 function arrivalDirection
function arrivalDirection(w: SimWorld, lane: string): { x: number; y: number } | null;
