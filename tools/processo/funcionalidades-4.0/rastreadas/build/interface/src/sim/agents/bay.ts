// What the feature uses of src/sim/agents/bay.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/bay.ts:7-18 interface BayLane
/** A parking bay and the lane it is reached from */

export interface BayLane {
  readonly lanelet: LaneletId;
  /** Arc position of the point where a car's body centre joins or leaves the lane. */
  readonly at: number;
  readonly x: number;
  readonly y: number;
  /** Unit direction of travel there. */
  readonly tx: number;
  readonly ty: number;
  /** For a lot entered by a car gate: the arc position */
  readonly entryAt?: number;
}

// src/sim/agents/bay.ts:20-41 interface Bay
export interface Bay {
  readonly id: number;
  readonly building: BuildingId;
  /** Centre of the stall. */
  readonly x: number;
  readonly y: number;
  /** Unit direction out of the stall, towards its open end. */
  readonly ox: number;
  readonly oy: number;
  /** Depth of the stall. */
  readonly depth: number;
  /** The road it is reached from; null when no lane is near enough. */
  readonly lane: BayLane | null;
  /** The way through its lot, from the aisle in front of the stall to the lot's street edge */
  readonly via: readonly { readonly x: number; readonly y: number }[];
  /** Half the stall's clear length: from its centre to its nearer end */
  readonly room: number;
  /** The car standing in it, or coming to it (reserved). */
  car: VehicleId | null;
  /** A bay along a street's kerb (`world/parkingLayout.ts`), not in a lot */
  readonly kerb?: true;
}
