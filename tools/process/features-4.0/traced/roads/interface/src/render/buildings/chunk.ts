// What the feature uses of src/render/buildings/chunk.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/chunk.ts:8-30 type PartKind
/** A part instanced per kind for the whole near ring (`kit.ts` PART_KINDS): declared with the cell's data, which the kit's materials do not need. */
export type PartKind =
  | 'glass'
  | 'frame'
  | 'concrete'
  | 'door'
  | 'shutter'
  | 'railing'
  | 'roofRailing'
  | 'awning'
  | 'column'
  | 'glassDark'
  | 'water'
  | 'lamp'
  | 'wash'
  | 'fin'
  | 'louvre'
  | 'glassRail'
  | 'brise'
  | 'lotBox'
  | 'lotRail'
  | 'lotSlab'
  | 'lotSides'
  | FacadePropKind;

// src/render/buildings/chunk.ts:35-43 interface CellEntry
/** WHAT ONE BUILDING CONTRIBUTES TO THE BATCHES */

/** A building of a cell as the cell workers are sent it (`cellWork.ts`): its chunk's key (an id, or a building cut open), its record as text and the ground under it. */
export interface CellEntry {
  readonly key: string;
  /** The record's digest (`Digest.addPlain`): two entries alike are the same record. */
  readonly sig: number;
  /** The ground under it, digested (`groundDigest`). */
  readonly digest: number;
  /** Its ground floor as an earlier job measured it on the same record, ground and paving (`cellPool.ts` `floorKnown`): null with no closed block; absent, measured by the job. */
  readonly floor?: number | null;
}

// src/render/buildings/chunk.ts:70-72 interface CellJobEntry
/** An entry as a job is sent it: where its worker reads the record (its tile's shared table and row, `recordTable.ts`), never the record. */
export interface CellJobEntry extends CellEntry {
  readonly record: RecordRef;
}

// src/render/buildings/chunk.ts:132-135 interface LotGroundRecipe
/** What a lot ground is made from besides the record: the level its lots meet and whether they were laid (`emitLots`). */
export interface LotGroundRecipe {
  readonly level: number | undefined;
  readonly lots: boolean;
}
