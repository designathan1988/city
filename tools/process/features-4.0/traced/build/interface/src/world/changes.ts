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

// src/world/changes.ts:19-23 variable DOC_KINDS
/** Every kind of change to the document, listed once (checked complete by the type) */
const DOC_KINDS = {
  roads: 1, traffic: 1, terrain: 1, paint: 1, buildings: 1, zones: 1, lots: 1, utilities: 1, barriers: 1, landscape: 1,
  transit: 1, people: 1, trees: 1, clearings: 1, elements: 1, fog: 1, clouds: 1, weather: 1, nature: 1, gullies: 1, economy: 1,
  lotKeys: 1, markingStyle: 1, buildingWear: 1,
} as const satisfies Record<DocChangeKind, 1>;

// src/world/changes.ts:24-24 variable DOC_CHANGE_KINDS
export const DOC_CHANGE_KINDS = Object.keys(DOC_KINDS) as DocChangeKind[];

// src/world/changes.ts:26-26 type DerivedChangeKind
/** What the game works out from it. */
export type DerivedChangeKind = 'elevation' | 'ground' | 'light' | 'surfaces';

// src/world/changes.ts:27-27 type ChangeKind
export type ChangeKind = DocChangeKind | DerivedChangeKind;

// src/world/changes.ts:33-33 function isStreaming
export const isStreaming = (entry: { readonly cause: string }): boolean => …;

// src/world/changes.ts:35-55 interface Change
export interface Change {
  /** Order of writing; every entry has its own. */
  readonly serial: number;
  readonly kind: ChangeKind;
  /** Where, in world units; `null` the whole map. */
  readonly rects: readonly ChangeRect[] | null;
  /** The ids it touched (segments, nodes, stamps, poles...), when it has them. */
  readonly ids?: readonly number[];
  /** A road entry's ids told apart (`'roads'`, `'traffic'`): within a tile a node and a segment may share an id (both counted from `tileIdBase`). */
  readonly nodes?: readonly number[];
  readonly segments?: readonly number[];
  /** Why: the tool and gesture, or what derived it. */
  readonly cause: string;
  /** The entry it follows from (a derived change). */
  readonly parent?: number;
  /** When it was written (`performance.now()`), and how long its work took, ms. */
  readonly at: number;
  readonly ms?: number;
  /** A line for whoever reads the diary: counts, sizes. */
  readonly detail?: string;
}

// src/world/changes.ts:68-72 interface JournalReader
/** A reader of some kinds that follows the diary (`ChangeJournal.follow`) */
export interface JournalReader {
  readonly kinds: readonly ChangeKind[];
  /** The serial it has read up to; negative: it has read nothing yet (it reads everything first). */
  readonly seenSerial: number;
}

// src/world/changes.ts:147-154 function rectAround
export function rectAround(points: Iterable<{ readonly x: number; readonly y: number }>, pad = 0): ChangeRect | null;

// src/world/changes.ts:156-381 class ChangeJournal
export class ChangeJournal { … }

// src/world/changes.ts:198-200 getter ChangeJournal.version
/** The serial of the latest entry: what a reader that has seen everything holds. */
  get version(): number;

// src/world/changes.ts:203-207 method ChangeJournal.serialOf
/** The serial of the latest entry of any of these kinds, 0 when none was written: the document's revisions */
  serialOf(...kinds: readonly ChangeKind[]): number;

// src/world/changes.ts:263-280 method ChangeJournal.record
/** Writes an entry; returns its serial. An empty list of rectangles writes nothing (0). */
  record(kind: ChangeKind, rects: readonly ChangeRect[] | null, extra: {
    readonly ids?: readonly number[]; readonly nodes?: readonly number[]; readonly segments?: readonly number[];
    readonly cause?: string; readonly parent?: number; readonly ms?: number; readonly detail?: string;
  } = {}): number;

// src/world/changes.ts:309-328 method ChangeJournal.since
/** The entries after `since` (oldest first), of these kinds only when given; `null` if some were forgotten. */
  since(since: number, kinds?: readonly ChangeKind[]): Change[] | null;
