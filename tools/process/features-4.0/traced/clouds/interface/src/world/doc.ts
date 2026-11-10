// What the feature uses of src/world/doc.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/doc.ts:153-2888 class RoadDoc
export class RoadDoc { … }

// src/world/doc.ts:216-216 field RoadDoc.clouds
/** Clouds the player placed in the sky (`clouds.ts`). */
  readonly clouds: PlacedCloud[] = [];

// src/world/doc.ts:262-262 field RoadDoc.changes
/** What changed in this document, where and why (`changes.ts`) */
  readonly changes = new ChangeJournal();

// src/world/doc.ts:1297-1302 method RoadDoc.removeCloud
removeCloud(id: number): void;

// src/world/doc.ts:1969-1978 method RoadDoc.ownList
/** A list about to be written in place: a working copy's own, copied from the original's the first time (`fork`). */
  private ownList(part: DocListPart): void;

// src/world/doc.ts:2942-2943 type ListPart
export type ListPart = 'terrainStamps' | 'terrainPaint' | 'fogDabs' | 'gullyDabs' | 'clouds' | 'trees' | 'treeClearings' | 'elements'
  | 'zones' | 'zoneMarks' | 'lotKeys' | 'people';

// src/world/doc.ts:2947-2947 type DocListPart
/** The lists the document holds itself (the lots' keys are the lot store's): what a working copy shares until it writes (`RoadDoc.fork`). */
type DocListPart = Exclude<ListPart, 'lotKeys'>;

// src/world/doc.ts:3314-3315 function placedCloud
/** A cloud placed, its keys in the order a map read back has them (`readCloud`): DO NOT let an undo of the whole map write them in another. */
const placedCloud = (v: Omit<PlacedCloud, 'id'>, id: number): PlacedCloud => …;
