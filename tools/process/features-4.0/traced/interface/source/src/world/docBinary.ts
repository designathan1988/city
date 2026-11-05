// The code of src/world/docBinary.ts that is the feature's (the rest of the file is shared).

// src/world/docBinary.ts:51-55 function imageBytes: the feature's own
export function imageBytes(image: DocImage): number {
  let bytes = sectionsBytes(image.buildings);
  for (const part of image.parts) bytes += part.bytes.byteLength;
  return bytes;
}

// src/world/docBinary.ts:68-72 function isDocBinary: the feature's own
export function isDocBinary(bytes: ArrayBufferLike): boolean {
  if (bytes.byteLength < HEAD * 4) return false;
  const head = new Uint32Array(bytes, 0, 2);
  return head[0] === MAGIC && head[1] === FORMAT;
}

// src/world/docBinary.ts:105-130 function unpackImage: the feature's own
export function unpackImage(buffer: ArrayBufferLike, own: (bytes: Uint8Array) => ArrayBufferLike = (bytes) => bytes.slice().buffer): DocImage {
  if (!isDocBinary(buffer)) throw new Error('[save] not a map in binary sections of this format');
  const words = new Uint32Array(buffer, 0, HEAD);
  const count = words[2]!;
  const dir = new Uint32Array(buffer, 0, HEAD + count * ENTRY);
  const parts: DocPart[] = [];
  let base: ArrayBufferLike | null = null, replaced: ArrayBufferLike | null = null, added: ArrayBufferLike | null = null;
  let gone = new Float64Array(0);
  for (let i = 0; i < count; i++) {
    const w = HEAD + i * ENTRY;
    const kind = dir[w]!, nameLength = dir[w + 1]!, nameAt = dir[w + 2]!, length = dir[w + 3]!;
    const bodyAt = align8(nameAt + nameLength);
    const body = new Uint8Array(buffer, bodyAt, length);
    switch (kind) {
      case S_PART: case S_RUN:
        parts.push({ name: decoder.decode(new Uint8Array(buffer, nameAt, nameLength).slice()), run: kind === S_RUN, bytes: body.slice() });
        break;
      case S_BASE: base = own(body); break;
      case S_GONE: gone = new Float64Array(body.slice().buffer); break;
      case S_REPLACED: replaced = own(body); break;
      case S_ADDED: added = own(body); break;
      default: throw new Error(`[save] a map in binary sections holds an unknown section ${kind}`);
    }
  }
  return { parts, buildings: { base, gone, replaced, added } };
}
