// What the feature uses of src/sim/intersections/crossingSpans.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/intersections/crossingSpans.ts:16-21 interface CrossingSpan
/** Arc interval of a crossing, measured from its `from` kerb. */
export interface CrossingSpan {
  readonly s0: number;
  readonly s1: number;
  /** Arc position along the MOVEMENT where its body first reaches the span. */
  readonly along: number;
}

// src/sim/intersections/crossingSpans.ts:138-140 method CrossingSpans.span
/** Undefined: never measured (treat conservatively). Null: never crossed. */
  span(connector: ConnectorId, crossing: CrossingId): CrossingSpan | null | undefined;

// src/sim/intersections/crossingSpans.ts:203-219 function reservationCoversCrossing
/** Whether this vehicle's current reservation still protects a zebra span. */
export function reservationCoversCrossing(w: SimWorld, v: Vehicle, connector: Connector,
  segment: number, span: CrossingSpan | null | undefined): boolean;
