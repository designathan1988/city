// What the feature uses of src/sim/vehicles/laneChange.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/laneChange.ts:12-12 variable COMMUTE_HOLD
/** How near the place it stops at a vehicle no longer changes lane, u. */
const COMMUTE_HOLD = m(60);

// src/sim/vehicles/laneChange.ts:29-29 variable KEEP_SIDE_BIAS
/** Bias towards the kerb side, in units of acceleration. */
const KEEP_SIDE_BIAS = 0.14;

// src/sim/vehicles/laneChange.ts:32-32 variable SAFE_BRAKE_SHARE
/** Hardest braking a discretionary change may impose on the car behind. */
const SAFE_BRAKE_SHARE = 0.55;

// src/sim/vehicles/laneChange.ts:35-35 variable CLEAR_ROAD
/** An obstacle far enough away to be no obstacle at all. */
const CLEAR_ROAD: Obstacle = { gap: 1e6, speed: 1e6, kind: 'vehicle' };

// src/sim/vehicles/laneChange.ts:38-38 variable SIGNAL_LEAD
/** Seconds a driver signals before moving across. */
const SIGNAL_LEAD = 1.4;

// src/sim/vehicles/laneChange.ts:39-39 variable SIGNAL_LEAD_CRAWLING
const SIGNAL_LEAD_CRAWLING = 0.8;

// src/sim/vehicles/laneChange.ts:51-108 function decideLaneChanges
function decideLaneChanges(w: SimWorld): void;

// src/sim/vehicles/laneChange.ts:111-138 function mandatory
/** The adjacent lane the route asks for, or null when there is no longer room to get there */
function mandatory(w: SimWorld, v: Vehicle, target: LaneletId): LaneletId | null;

// src/sim/vehicles/laneChange.ts:141-143 function roomNeeded
/** Road a mandatory change of `steps` lanes needs at `speed`: each lane is signalled for */
function roomNeeded(w: SimWorld, v: Vehicle, adjacent: LaneletId, steps: number, speed: number): number;

// src/sim/vehicles/laneChange.ts:146-149 function roomAt
/** `roomNeeded` with the lateral distance to the adjacent lane given (`sideways`): DO NOT project on the lanes again for each speed tried. */
function roomAt(v: Vehicle, across: number | null, steps: number, speed: number): number;

// src/sim/vehicles/laneChange.ts:152-178 function positioningSpeedCap
/** The speed a driver holds while working across to the lane the route needs. */
export function positioningSpeedCap(w: SimWorld, v: Vehicle): number;

// src/sim/vehicles/laneChange.ts:181-181 variable FINISH_MARGIN
/** Kept clear of a stop line or a queue's tail when a change must be finished. */
const FINISH_MARGIN = m(6);

// src/sim/vehicles/laneChange.ts:183-183 variable CRAWL
/** Below this a vehicle ahead is treated as standing, not as traffic to follow. */
const CRAWL = m(2);

// src/sim/vehicles/laneChange.ts:186-189 function changeLength
/** Road a change into `target` from here would be driven over. */
function changeLength(w: SimWorld, v: Vehicle, target: LaneletId, speed = v.v): number;

// src/sim/vehicles/laneChange.ts:192-206 function sideways
/** How far across `target`'s centre lies from this vehicle's place on its lane; null when either lane is gone. */
function sideways(w: SimWorld, v: Vehicle, target: LaneletId): number | null;

// src/sim/vehicles/laneChange.ts:208-208 variable sidewaysFrame
const sidewaysFrame = newFrame();

// src/sim/vehicles/laneChange.ts:209-209 variable lastSideways
const lastSideways: { from: Polyline | null; to: Polyline | null; at: number; across: number } = { from: null, to: null, at: NaN, across: 0 };

// src/sim/vehicles/laneChange.ts:212-212 variable MOBIL_EVERY
/** Ticks between two weighings of a discretionary change by the same driver (MOBIL): a quarter of a second. */
const MOBIL_EVERY = Math.max(1, Math.round(0.25 / DT));

// src/sim/vehicles/laneChange.ts:215-219 function standingIntent
/** Between two weighings: the change being signalled, while it can still be made from here (its lane beside this one, room left). */
function standingIntent(w: SimWorld, v: Vehicle, laneId: LaneletId, laneLength: number): LaneletId | null;

// src/sim/vehicles/laneChange.ts:222-312 function discretionary
/** The change the driver wants: the best neighbouring lane, or none. */
function discretionary(
  w: SimWorld,
  v: Vehicle,
  laneId: LaneletId,
  laneLength: number,
  signalling: LaneletId | null,
): LaneletId | null;

// src/sim/vehicles/laneChange.ts:315-318 function nextOutSegment
/** Segment the planned next junction movement leaves by, or null when none is planned. */
function nextOutSegment(w: SimWorld, v: Vehicle): number | null;

// src/sim/vehicles/laneChange.ts:320-325 interface Neighbour
interface Neighbour {
  readonly vehicle: Vehicle;
  readonly gap: number;
  readonly speed: number;
  readonly kind: 'vehicle';
}

// src/sim/vehicles/laneChange.ts:328-349 function leaderIn
/** The nearest vehicle ahead of `s` in a lane, as an obstacle. */
function leaderIn(w: SimWorld, laneId: LaneletId, s: number, self: number): Neighbour | null;

// src/sim/vehicles/laneChange.ts:352-373 function followerIn
/** The nearest vehicle behind `rear` in a lane. */
function followerIn(w: SimWorld, laneId: LaneletId, rear: number, self: number): Neighbour | null;

// src/sim/vehicles/laneChange.ts:376-393 function canLeaveLane
/** Whether the body can get out of the lane it is in before whatever stands in that lane stops it. */
function canLeaveLane(w: SimWorld, v: Vehicle, target: LaneletId): boolean;

// src/sim/vehicles/laneChange.ts:396-410 function restingRearAhead
/** Where the rear of the nearest vehicle ahead of `s` in a lane will come to rest. */
function restingRearAhead(w: SimWorld, laneId: LaneletId, s: number, self: number): number;

// src/sim/vehicles/laneChange.ts:418-490 class RestingRears
/**
 * `restingRearAhead` for every lane asked in one lane-change stage, each lane's cascade worked out once, head to tail (SIM-12):
 * the rear each vehicle comes to rest at, given the ones ahead of it, and a query finds where the walk from the head would stop.
 * Exact: the walk's answer is the rest of the last vehicle it passed, which does not depend on where it is stopped.
 * A lane whose order is not sorted by `s`, or a query whose own vehicle stands ahead of `s` in the lane, is walked as before.
 */
class RestingRears { … }

// src/sim/vehicles/laneChange.ts:420-420 field RestingRears.lanes
private readonly lanes = new Map<LaneletId, number>();

// src/sim/vehicles/laneChange.ts:435-454 method RestingRears.ahead
ahead(w: SimWorld, laneId: LaneletId, s: number, self: number): number;

// src/sim/vehicles/laneChange.ts:456-489 method RestingRears.build
private build(w: SimWorld, laneId: LaneletId): number;

// src/sim/vehicles/laneChange.ts:492-492 variable resting
const resting = new RestingRears();

// src/sim/vehicles/laneChange.ts:495-502 function restingReach
/** How much further a vehicle rolls before something standing holds it; Infinity when nothing does. */
function restingReach(v: Vehicle): number;

// src/sim/vehicles/laneChange.ts:505-520 function pullOutRoom
/** Room a driver leaves behind a vehicle standing at the kerb */
export function pullOutRoom(w: SimWorld, v: Vehicle): number;

// src/sim/vehicles/laneChange.ts:522-522 variable pullOutFrame
const pullOutFrame = newFrame();

// src/sim/vehicles/laneChange.ts:525-541 function shadowClearDistance
/** Road driven before no corner of the body is over the line between two lanes `start` apart, on a change `length` long */
function shadowClearDistance(v: Vehicle, start: number, length: number): number;

// src/sim/vehicles/laneChange.ts:544-577 function gapIsSafe
/** Whether the target lane has room beside this vehicle right now. */
function gapIsSafe(w: SimWorld, v: Vehicle, target: LaneletId): boolean;
