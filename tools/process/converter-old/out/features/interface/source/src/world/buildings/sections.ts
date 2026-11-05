// The code of src/world/buildings/sections.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/sections.ts:48-50 function sectionsBytes: the feature's own
export function sectionsBytes(s: BuildingSections): number {
  return (s.base?.byteLength ?? 0) + s.gone.byteLength + (s.replaced?.byteLength ?? 0) + (s.added?.byteLength ?? 0);
}
