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

// src/world/docBinary.ts:26-37 function imageOf
/** The map's JSON (no buildings) and its buildings as an image: each key's JSON on its own, a long list in runs. */
export function imageOf(data: object, buildings: BuildingSections): DocImage;

// src/world/docBinary.ts:51-55 function imageBytes
/** What an image holds, bytes. */
export function imageBytes(image: DocImage): number;

// src/world/docBinary.ts:62-62 variable S_PART,S_RUN,S_BASE,S_GONE,S_REPLACED,S_ADDED
/** Section kinds. */
const S_PART = 1, S_RUN = 2, S_BASE = 3, S_GONE = 4, S_REPLACED = 5, S_ADDED = 6;

// src/world/docBinary.ts:75-102 function packImage
/** An image packed in one buffer: a directory of sections (kind, name, offset, length), each section 8-byte aligned. Deterministic: the same image, the same bytes. */
export function packImage(image: DocImage): ArrayBuffer;

// src/world/docBinary.ts:105-130 function unpackImage
/** An image read back from its buffer: each section its own copy (the buildings' tables in buffers of their own, as the store keeps them). */
export function unpackImage(buffer: ArrayBufferLike, own: (bytes: Uint8Array) => ArrayBufferLike = (bytes) => bytes.slice().buffer): DocImage;
