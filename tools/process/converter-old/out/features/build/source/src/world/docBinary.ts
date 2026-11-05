// The code of src/world/docBinary.ts that is the feature's (the rest of the file is shared).

// src/world/docBinary.ts:14-17 slice of src/world/docBinary.ts#DocImage (shared): the feature's statements; "…" marks code left out
export interface DocImage {
  // … 1 lines
  readonly buildings: BuildingSections;
}

// src/world/docBinary.ts:26-37 slice of src/world/docBinary.ts#imageOf (shared): the feature's statements; "…" marks code left out
export function imageOf(data: object, buildings: BuildingSections): DocImage {
  // … 9 lines
  return { parts, buildings };
}

// src/world/docBinary.ts:51-55 slice of src/world/docBinary.ts#imageBytes (shared): the feature's statements; "…" marks code left out
export function imageBytes(image: DocImage): number {
  let bytes = sectionsBytes(image.buildings);
  for (const part of image.parts) bytes += part.bytes.byteLength;
  return bytes;
}

// src/world/docBinary.ts:75-102 slice of src/world/docBinary.ts#packImage (shared): the feature's statements; "…" marks code left out
export function packImage(image: DocImage): ArrayBuffer {
  // … 5 lines
  add(S_BASE, b.base);
  if (b.gone.length) sections.push({ kind: S_GONE, name: none, bytes: new Uint8Array(b.gone.buffer, b.gone.byteOffset, b.gone.byteLength) });
  add(S_REPLACED, b.replaced);
  add(S_ADDED, b.added);
// … 17 lines
}
