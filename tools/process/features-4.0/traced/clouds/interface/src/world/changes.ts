// What the feature uses of src/world/changes.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/changes.ts:4-4 type ChangeRect
/** THE WORLD'S DIARY OF CHANGES: what changed, where, and why - one record for the whole game */

/** A world rectangle: [minX, minY, maxX, maxY]. */
export type ChangeRect = readonly [number, number, number, number];

// src/world/changes.ts:7-17 type DocChangeKind
/** What the player changes in the document. */
export type DocChangeKind =
  | 'roads' | 'traffic' | 'terrain' | 'paint' | 'buildings' | 'zones' | 'lots' | 'utilities' | 'barriers' | 'landscape'
  | 'transit' | 'people' | 'trees' | 'clearings' | 'elements' | 'fog' | 'clouds' | 'weather' | 'nature' | 'gullies'
  /** The balance (`economy.ts`): an edit paid for, or a demolition paid back. */
  | 'economy'
  /** The land already cut into lots (`LotStore.addKeys`): no lot came or went, so a reader of the lots by id */
  | 'lotKeys'
  /** The regional standard of the road paint (`RoadDoc.markingStyle`) */
  | 'markingStyle'
  /** A building's wear (`Building.decay`: age, soot) and nothing else of it (`BuildingStore.wear`). */
  | 'buildingWear';

// src/world/changes.ts:26-26 type DerivedChangeKind
/** What the game works out from it. */
export type DerivedChangeKind = 'elevation' | 'ground' | 'light' | 'surfaces';

// src/world/changes.ts:27-27 type ChangeKind
export type ChangeKind = DocChangeKind | DerivedChangeKind;

// src/world/changes.ts:147-154 function rectAround
export function rectAround(points: Iterable<{ readonly x: number; readonly y: number }>, pad = 0): ChangeRect | null;

// src/world/changes.ts:263-280 method ChangeJournal.record
/** Writes an entry; returns its serial. An empty list of rectangles writes nothing (0). */
  record(kind: ChangeKind, rects: readonly ChangeRect[] | null, extra: {
    readonly ids?: readonly number[]; readonly nodes?: readonly number[]; readonly segments?: readonly number[];
    readonly cause?: string; readonly parent?: number; readonly ms?: number; readonly detail?: string;
  } = {}): number;
