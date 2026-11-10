// What the feature uses of src/world/buildings/sections.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/sections.ts:7-13 interface BuildingSections
/** A STORE'S BUILDINGS AS BUFFERS, NEVER AS OBJECTS (the save's sections, the simulation's map, the undo of a whole map): the table they were opened from, as it is (shared, never copied), and what was written over it - the ids deleted, the records replaced in place and those added after, each kind one table. Read back in the store's order (`BuildingItems`). */
export interface BuildingSections {
  readonly base: ArrayBufferLike | null;
  /** The ids of the table deleted (set again: in `added`). */
  readonly gone: Float64Array;
  readonly replaced: ArrayBufferLike | null;
  readonly added: ArrayBufferLike | null;
}
