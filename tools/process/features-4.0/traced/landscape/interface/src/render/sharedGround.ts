// What the feature uses of src/render/sharedGround.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/sharedGround.ts:8-13 interface PlateHeights
/** THE GROUND'S HEIGHTS IN SHARED MEMORY: each plate's corners as drawn and as they were before the roads, the plate's own arrays (`terrain.ts`), read by any worker as the page reads them (`flatAtlas.ts`) */

/** One plate's corners and the diagonals they are drawn with, as drawn (`renderedHeightAt`) and natural (`naturalRenderedHeightAt`). */
export interface PlateHeights {
  readonly rendered: Float64Array;
  readonly natural: Float64Array;
  readonly flip: Uint8Array;
  readonly naturalFlip: Uint8Array;
}

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

// src/render/sharedGround.ts:36-39 interface SharedGroundAt
/** The ground as drawn, as the page reads it, with the shared plates it reads (`flatAtlas.ts` `renderedHeightAt`): a job handed it reads the same ground in a worker. */
export interface SharedGroundAt {
  (x: number, y: number): number;
  readonly share?: GroundShare;
}

// src/render/sharedGround.ts:62-98 class GroundShare
/** DO NOT copy the plates' heights for the workers: the plate writes its corners into the shared buffer itself, a stroke or a road reaching the workers as it lands, no message */
export class GroundShare { … }

// src/render/sharedGround.ts:65-65 field GroundShare.relief
private relief: ReliefVersion | null = null;

// src/render/sharedGround.ts:87-91 method GroundShare.setRelief
setRelief(relief: ReliefVersion | null): void;

// src/render/sharedGround.ts:112-137 function sharedGroundReaders
/** The ground read off the shared plates, point for point as `flatAtlas.ts` `ask` and `terrain.ts` `sampleGrid` read it on the page; `missed`, told of a read on a plate left `away` (its height then the relief's: the job is asked again over it). */
export function sharedGroundReaders(data: SharedGroundData, missed?: (x: number, y: number) => void): GroundReaders;
