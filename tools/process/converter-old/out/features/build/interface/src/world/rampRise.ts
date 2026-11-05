// What the feature uses of src/world/rampRise.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/rampRise.ts:7-26 interface CurbRamp
/** How high the walking surface stands over a dropped kerb (`curbRamps.ts`), from the ramps near a point alone. */

/** A dropped kerb: at each end of every zebra the footway ramps down to the carriageway (`curbRamps.ts`). */
export interface CurbRamp {
  /** The middle of the ramp's foot, on the kerb face (the carriageway's edge). */
  readonly x: number;
  readonly y: number;
  /** Along the street. */
  readonly tx: number;
  readonly ty: number;
  /** Across the street, from the carriageway into the footway. */
  readonly nx: number;
  readonly ny: number;
  /** Half the ramp's width along the street (half the zebra's). */
  readonly half: number;
  /** How far into the footway the slope runs, to the footway's full height. */
  readonly run: number;
  /** Length of each flared side along the street. */
  readonly flare: number;
  /** The street whose zebra it serves. */
  readonly segment: SegmentId;
  readonly structure: string;
}
