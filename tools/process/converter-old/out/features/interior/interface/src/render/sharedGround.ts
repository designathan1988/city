// What the feature uses of src/render/sharedGround.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/sharedGround.ts:16-27 interface SharedGroundData
/** What a job reads the ground from: each plate's buffer (null: a plate not made, read off the relief), its grid and the relief. */
export interface SharedGroundData {
  /** Corners a side, the cell, half the plate, the base level the corners carry. */
  readonly n: number;
  readonly cell: number;
  readonly half: number;
  readonly base: number;
  readonly relief: ReliefVersion | null;
  /** The plates made, by key (`world/bounds.ts` `plateOf`); a plate not in it is read off the relief. */
  readonly plates: ReadonlyMap<number, SharedArrayBuffer>;
  /** Plates made but left out of `plates` (`groundAround`): a read there is a miss, not the relief. */
  readonly away?: readonly number[];
}

// src/render/sharedGround.ts:30-33 interface GroundReaders
/** The ground as a building reads it: as drawn, and as it was before the roads cut it. */
export interface GroundReaders {
  readonly groundAt: (x: number, y: number) => number;
  readonly naturalAt: (x: number, y: number) => number;
}
