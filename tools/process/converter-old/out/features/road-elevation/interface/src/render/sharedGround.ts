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

// src/render/sharedGround.ts:36-39 interface SharedGroundAt
/** The ground as drawn, as the page reads it, with the shared plates it reads (`flatAtlas.ts` `renderedHeightAt`): a job handed it reads the same ground in a worker. */
export interface SharedGroundAt {
  (x: number, y: number): number;
  readonly share?: GroundShare;
}

// src/render/sharedGround.ts:62-98 class GroundShare
/** DO NOT copy the plates' heights for the workers: the plate writes its corners into the shared buffer itself, a stroke or a road reaching the workers as it lands, no message */
export class GroundShare { … }
