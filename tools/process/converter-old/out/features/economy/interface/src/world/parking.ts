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
