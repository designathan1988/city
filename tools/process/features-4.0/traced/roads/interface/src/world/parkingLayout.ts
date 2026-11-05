// What the feature uses of src/world/parkingLayout.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/parkingLayout.ts:17-28 interface ParkingBay
/** Where the parking bays of every street lie, and the paint that marks them. */

interface ParkingBay {
  readonly segment: SegmentId;
  /** +1: left of the segment's a -> b direction; -1: right. */
  readonly side: 1 | -1;
  readonly kind: ParkingKind;
  /** The stall as drawn: kerb start, kerb end, inner end, inner start. */
  readonly corners: readonly [Vec2, Vec2, Vec2, Vec2];
  /** Middle of the stall. */
  readonly centre: Vec2;
  /** Unit direction a parked car faces (its nose). */
  readonly facing: Vec2;
}

// src/world/parkingLayout.ts:30-37 interface ParkingLine
export interface ParkingLine {
  readonly segment: SegmentId;
  readonly points: readonly Vec2[];
  readonly width: number;
  readonly color: string;
  /** Paint and gap along the line, world units; null for a continuous line. */
  readonly dash: readonly number[] | null;
}

// src/world/parkingLayout.ts:39-42 interface ParkingLayout
export interface ParkingLayout {
  readonly bays: readonly ParkingBay[];
  readonly lines: readonly ParkingLine[];
}

// src/world/parkingLayout.ts:44-44 variable BAY_LINE
const BAY_LINE = m(0.12);

// src/world/parkingLayout.ts:45-45 variable NO_PARKING_LINE
const NO_PARKING_LINE = m(0.12);

// src/world/parkingLayout.ts:46-46 variable WHITE
const WHITE = '#efece0';

// src/world/parkingLayout.ts:48-48 variable MER_DASH
/** The MER's dashes along the parking lane: 1 m of paint, 1 m clear. */
const MER_DASH: readonly number[] = [m(1), m(1)];

// src/world/parkingLayout.ts:49-49 variable YELLOW
const YELLOW = '#e1c45a';

// src/world/parkingLayout.ts:51-51 variable CYCLE_RED
/** The red of a ciclofaixa's paving. */
const CYCLE_RED = '#a8473e';

// src/world/parkingLayout.ts:53-53 variable PARKING_PAINT
/** Every colour the parking's lines are painted in (`render/markings.ts` `PAINT_COLOURS`). */
export const PARKING_PAINT: readonly string[] = Object.freeze([WHITE, YELLOW, CYCLE_RED]);

// src/world/parkingLayout.ts:59-59 variable STRAIGHT
/** A point of a run's line nearer than this to the line through its neighbours carries no shape: left out. */
const STRAIGHT = m(0.001);

// src/world/parkingLayout.ts:62-67 function parkingLayout
/** DO NOT hand every entrance of the map to every street: each street with parking tested every gate of the city. */
export function parkingLayout(net: Network, entrances: readonly [Vec2, Vec2][] | null = null): ParkingLayout;

// src/world/parkingLayout.ts:90-93 function parksAlong
/** Whether a street has parking laid out at all (`parkingOf`): only then do the buildings near it move its paint. */
export function parksAlong(net: Network, ribbon: SegmentRibbon): boolean;

// src/world/parkingLayout.ts:96-104 function parkingDigest
/** DO NOT read anything in `parkingOf` without adding it here: what one street's bays are laid out from besides the entrances it is handed, into `digest` (a tile's baked kerb bays are taken where it is the same, `sim/tileBays.ts`). */
export function parkingDigest(net: Network, ribbon: SegmentRibbon, digest: Digest): Digest;

// src/world/parkingLayout.ts:107-189 function parkingOf
/** One street's bays and lines, onto `bays` and `lines`. */
function parkingOf(net: Network, ribbon: SegmentRibbon, entrances: readonly [Vec2, Vec2][] | null,
  bays: ParkingBay[], lines: ParkingLine[]): void;

// src/world/parkingLayout.ts:200-209 function endClear
/** How far from a node no bay may start: the corner clearance, or clear of the crossing. */
function endClear(net: Network, seg: SegmentId, node: NodeId): number;

// src/world/parkingLayout.ts:212-218 function entrancesNear
/** The entrances of the buildings within reach of a street (`frontagesOn`'s reach). */
function entrancesNear(doc: RoadDoc, line: Polyline, half: number): [Vec2, Vec2][];

// src/world/parkingLayout.ts:221-242 function frontagesOn
/** Intervals along `line` in front of a lot entrance on one side. */
function frontagesOn(line: Polyline, half: number, side: 1 | -1, entrances: readonly [Vec2, Vec2][]): [number, number][];

// src/world/parkingLayout.ts:245-257 function subtract
/** `from` with every interval of `cut` removed. */
function subtract(from: readonly [number, number][], cut: readonly [number, number][]): [number, number][];
