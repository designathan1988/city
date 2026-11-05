// What the feature uses of src/world/landscape.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/landscape.ts:227-234 interface CrossingAccess
/** The landing of a crossing on the footways: kept clear of everything. */
export interface CrossingAccess {
  readonly x: number;
  readonly y: number;
  readonly tx: number;
  readonly ty: number;
  readonly across: number;
  readonly structure: string;
}
