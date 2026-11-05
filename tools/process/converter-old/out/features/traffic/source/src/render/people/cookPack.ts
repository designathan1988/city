// The code of src/render/people/cookPack.ts that is the feature's (the rest of the file is shared).

// src/render/people/cookPack.ts:39-52 function unpackRecord: the feature's own
export function unpackRecord(buffer: ArrayBuffer): Record<string, PackValue> {
  const length = new DataView(buffer).getUint32(0, true);
  const { header, layout } = JSON.parse(new TextDecoder().decode(new Uint8Array(buffer, 8, length))) as {
    header: Record<string, PackValue>; layout: { name: string; kind: KindName; offset: number; length: number }[];
  };
  const start = Math.ceil((8 + length) / 8) * 8;
  const out: Record<string, PackValue> = { ...header };
  // Each array copied out of the file: a view would pin the whole file for as long as any one array is kept.
  for (const { name, kind, offset, length: n } of layout) {
    const at = start + offset;
    out[name] = new KINDS[kind](buffer.slice(at, at + n * KINDS[kind].BYTES_PER_ELEMENT));
  }
  return out;
}
