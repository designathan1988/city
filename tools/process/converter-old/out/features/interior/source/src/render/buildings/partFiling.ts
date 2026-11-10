// The code of src/render/buildings/partFiling.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/partFiling.ts:205-209 function filingBytes: the feature's own
export function filingBytes(f: PartFiling): number {
  let bytes = 0;
  for (const b of f.bands) bytes += b.poses.byteLength + (b.colours?.byteLength ?? 0) + (b.looks?.byteLength ?? 0);
  return bytes;
}
