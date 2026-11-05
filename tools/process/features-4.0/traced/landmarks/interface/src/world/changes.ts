// What the feature uses of src/world/changes.ts (shared with the program): declarations as the source writes them, bodies left out.

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

// src/world/changes.ts:68-72 interface JournalReader
/** A reader of some kinds that follows the diary (`ChangeJournal.follow`) */
export interface JournalReader {
  readonly kinds: readonly ChangeKind[];
  /** The serial it has read up to; negative: it has read nothing yet (it reads everything first). */
  readonly seenSerial: number;
}

// src/world/changes.ts:156-381 class ChangeJournal
export class ChangeJournal { … }
