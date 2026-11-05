// What the feature uses of src/sim/intersections/crossingSpans.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/intersections/crossingSpans.ts:16-21 interface CrossingSpan
/** Arc interval of a crossing, measured from its `from` kerb. */
export interface CrossingSpan {
  readonly s0: number;
  readonly s1: number;
  /** Arc position along the MOVEMENT where its body first reaches the span. */
  readonly along: number;
}

// src/sim/intersections/crossingSpans.ts:29-29 variable PERSON_CLEAR
/** A walker's clearance from a vehicle body (`PedestrianClearance`), plus margin. */
const PERSON_CLEAR = m(0.6);

// src/sim/intersections/crossingSpans.ts:34-34 field CrossingSpans.spans
/** By connector, then by crossing: the two ids as they are, never a key string joined from them on every ask */
  private readonly spans = new Map<ConnectorId, Map<CrossingId, CrossingSpan | null>>();

// src/sim/intersections/crossingSpans.ts:41-41 field CrossingSpans.ofJunction
/** What each junction measured: its connectors, its approach lanes and the keys of `measured` it made. */
  private readonly ofJunction = new Map<NodeId, { readonly connectors: ConnectorId[]; readonly lanes: string[]; readonly keys: string[] }>();

// src/sim/intersections/crossingSpans.ts:44-51 method CrossingSpans.build
/** @rebuild Every junction: a world's first topology, or a lane graph built whole. */
  build(w: SimWorld): void;

// src/sim/intersections/crossingSpans.ts:73-131 method CrossingSpans.measureJunction
/** One junction's spans: each movement over each zebra, and each approach's stop before another leg's. */
  private measureJunction(w: SimWorld, node: NodeId, previous: Map<string, Measured>): void;

// src/sim/intersections/crossingSpans.ts:138-140 method CrossingSpans.span
/** Undefined: never measured (treat conservatively). Null: never crossed. */
  span(connector: ConnectorId, crossing: CrossingId): CrossingSpan | null | undefined;

// src/sim/intersections/crossingSpans.ts:143-143 function key
const key = (connector: ConnectorId, crossing: CrossingId): string => …;

// src/sim/intersections/crossingSpans.ts:145-150 interface Measured
interface Measured {
  readonly path: Float64Array;
  readonly edge: Float64Array;
  readonly length: number;
  readonly span: CrossingSpan | null;
}

// src/sim/intersections/crossingSpans.ts:163-185 function measure
/** The stretch of a crossing (its `samples`, `sampleZebra`) that a movement along `path` drives over. */
function measure(path: Polyline, crossing: Polyline, samples: Float64Array): CrossingSpan | null;

// src/sim/intersections/crossingSpans.ts:187-192 function sameFloats
function sameFloats(a: Float64Array, b: Float64Array): boolean;

// src/sim/intersections/crossingSpans.ts:195-195 variable PED_BODY
/** A person's radius, a minimum pace assumed for someone about to move, and the look-ahead. */
export const PED_BODY = 1;

// src/sim/intersections/crossingSpans.ts:196-196 variable PED_MIN_PACE
export const PED_MIN_PACE = m(0.8);

// src/sim/intersections/crossingSpans.ts:197-197 variable PED_REACH_TIME
export const PED_REACH_TIME = 4;

// src/sim/intersections/crossingSpans.ts:199-199 variable PED_CROSSING_STOP_BUFFER
/** Reserve the near half of a zebra before stopping a car for a pedestrian. */
export const PED_CROSSING_STOP_BUFFER = CROSSWALK_DEPTH / 2 + m(0.2);

// src/sim/intersections/crossingSpans.ts:200-200 variable CLEAR_PAST
const CLEAR_PAST = m(2.4);

// src/sim/intersections/crossingSpans.ts:203-219 function reservationCoversCrossing
/** Whether this vehicle's current reservation still protects a zebra span. */
export function reservationCoversCrossing(w: SimWorld, v: Vehicle, connector: Connector,
  segment: number, span: CrossingSpan | null | undefined): boolean;

// src/sim/intersections/crossingSpans.ts:222-227 function pedestrianAffectsSpan
/** Whether somebody on a crossing occupies or will soon reach this movement's part of it. */
export function pedestrianAffectsSpan(p: CrossingOccupant, span: CrossingSpan): boolean;

// src/sim/intersections/crossingSpans.ts:230-240 function pedestrianInSpan
/** Whether somebody on this crossing is in, or about to enter, the span. */
export function pedestrianInSpan(w: SimWorld, connector: ConnectorId, crossing: CrossingId): CrossingSpan | null;
