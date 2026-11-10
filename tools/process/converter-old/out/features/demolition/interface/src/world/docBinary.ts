// What the feature uses of src/world/docBinary.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/docBinary.ts:6-11 interface DocPart
/** THE MAP AS BINARY SECTIONS, NEVER ONE TEXT (report "Arquitetura para cidades grandes", "A conta de memória": V8 strings stop near 512 MiB; region files of offsets and lengths, each section read on its own): each part of the map's JSON one short UTF-8 section (a long list cut in runs), the buildings their table and what was written over it (`BuildingSections`). An undo of a whole map keeps the same image unpacked (`DocImage`), the buildings' table shared with the map it was, never copied. */

/** A part of the map as `RoadDoc.toJSON` writes it: one key's value, or a run of a long list's items; its JSON in UTF-8. */
export interface DocPart {
  readonly name: string;
  /** Of a list cut in runs: this run's items, appended to the runs before it of the same name. */
  readonly run: boolean;
  readonly bytes: Uint8Array;
}

// src/world/docBinary.ts:14-17 interface DocImage
/** The map unpacked: its parts in `toJSON`'s order, its buildings as buffers. */
export interface DocImage {
  readonly parts: readonly DocPart[];
  readonly buildings: BuildingSections;
}
