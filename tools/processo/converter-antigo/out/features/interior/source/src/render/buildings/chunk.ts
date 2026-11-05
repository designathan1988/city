// The code of src/render/buildings/chunk.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/chunk.ts:46-47 function sameEntries: the feature's own
export const sameEntries = (a: readonly CellEntry[], b: readonly CellEntry[]): boolean =>
  a === b || a.length === b.length && a.every((e, i) => e.key === b[i]!.key && e.sig === b[i]!.sig && e.digest === b[i]!.digest);

// src/render/buildings/chunk.ts:50-54 interface PackedEntries: the feature's own
export interface PackedEntries {
  readonly ids: Float64Array;
  readonly sigs: Float64Array;
  readonly digest: number;
}

// src/render/buildings/chunk.ts:56-61 function samePacked: the feature's own
export const samePacked = (a: PackedEntries, b: PackedEntries): boolean => {
  if (a === b) return true;
  if (a.digest !== b.digest || a.ids.length !== b.ids.length) return false;
  for (let i = 0; i < a.ids.length; i++) if (a.ids[i] !== b.ids[i] || a.sigs[i] !== b.sigs[i]) return false;
  return true;
};

// src/render/buildings/chunk.ts:64-67 function unpack: the feature's own
export function unpack(p: PackedEntries, out: CellEntry[] = []): CellEntry[] {
  for (let i = 0; i < p.ids.length; i++) out.push({ key: String(p.ids[i]!), sig: p.sigs[i]!, digest: p.digest });
  return out;
}

// src/render/buildings/chunk.ts:108-122 slice of src/render/buildings/chunk.ts#BuildingChunk (shared): the feature's statements; "…" marks code left out
export interface BuildingChunk {
  // … 6 lines
  readonly furniture?: Readonly<Partial<Record<FurnitureKind, PartBatch>>>;
// … 6 lines
}
