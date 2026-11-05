// What the feature uses of src/world/lotRecord.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/lotRecord.ts:6-14 interface Lot
export interface Lot {
  readonly id: number;
  /** The boundary, counter-clockwise (the inside on the left of each side): any polygon */
  readonly corners: readonly Vec2[];
  readonly use?: ZoneUse;
  readonly density?: ZoneDensity;
  /** The building that grew on it, while it stands. */
  readonly building?: number;
}
