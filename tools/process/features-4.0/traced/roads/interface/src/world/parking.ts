// What the feature uses of src/world/parking.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/parking.ts:6-6 variable PARKING_KINDS
/** On-street parking: a native part of a road's cross-section. */

/** A band by the kerb is parking (`parallel`) or a CYCLE LANE (`cycle`): the ciclofaixa of the Brazilian code */
export const PARKING_KINDS = ['none', 'parallel', 'cycle'] as const;

// src/world/parking.ts:7-7 type ParkingKind
export type ParkingKind = (typeof PARKING_KINDS)[number];

// src/world/parking.ts:10-13 interface SegmentParking
/** Parking on the two sides of a segment, left and right of its a -> b direction. */
export interface SegmentParking {
  readonly left: ParkingKind;
  readonly right: ParkingKind;
}

// src/world/parking.ts:16-21 variable PARKING_DEPTH
/** Depth of a parking lane from the kerb face, world units. */
export const PARKING_DEPTH: Readonly<Record<ParkingKind, number>> = {
  none: 0,
  parallel: m(2),
  // 1.5 m to 2 m in the manuals; whole metres of the grid.
  cycle: m(2),
};

// src/world/parking.ts:24-28 variable PARKING_PITCH
/** Distance along the kerb between bay lines, world units. */
export const PARKING_PITCH: Readonly<Record<ParkingKind, number>> = {
  none: 0,
  parallel: m(6),
  cycle: 0,
};

// src/world/parking.ts:33-33 variable CORNER_CLEAR
/** No bay within this distance of a junction's kerb line (CTB art. 181 I). */
export const CORNER_CLEAR = m(5);

// src/world/parking.ts:35-35 variable CROSSING_CLEAR
/** No bay within this distance of a pedestrian crossing. */
export const CROSSING_CLEAR = m(2);

// src/world/parking.ts:56-58 function sameParking
export function sameParking(a: SegmentParking | undefined, b: SegmentParking | undefined): boolean;

// src/world/parking.ts:66-69 function parkingAllowed
/** Roads without footways (highways, ramps) do not park at all; every street does. */
export function parkingAllowed(kind: ParkingKind, road: { readonly id: string }): boolean;
