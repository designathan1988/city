// What the feature uses of src/render/buildings/cellPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/cellPool.ts:106-106 interface Floor
/** A building's ground floor as a job measured it, and what it was measured on: its record, its ground (`CellEntry`) and whether the paving was read. */
interface Floor { readonly sig: number; readonly digest: number; readonly paved: boolean; readonly floor: number | null }

// src/render/buildings/cellPool.ts:132-139 method FloorColumns.get
get(key: string): Floor | undefined;
