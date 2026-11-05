// What the feature uses of src/world/roads/streetChain.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/streetChain.ts:11-15 interface ChainPiece
export interface ChainPiece {
  readonly id: SegmentId;
  /** Drawn the other way round from the first segment. */
  readonly flipped: boolean;
}

// src/world/roads/streetChain.ts:17-60 function streetChain
export function streetChain(doc: RoadDoc, first: SegmentId): ChainPiece[];

// src/world/roads/streetChain.ts:63-68 function flipProfile
/** A profile seen from the other end: its elements in the other order, each lane's direction swapped. */
export function flipProfile(profile: RoadProfileSpec): RoadProfileSpec;
