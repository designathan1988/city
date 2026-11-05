// The code of src/ui/roads/cutWalls.ts that is the feature's (the rest of the file is shared).

// src/ui/roads/cutWalls.ts:13-17 function setCutWalls: the feature's own
export function setCutWalls(on: boolean): void {
  if (on === chosen) return;
  chosen = on;
  try { globalThis.localStorage?.setItem(STORAGE_KEY, on ? '1' : '0'); } catch { /* kept for this session */ }
}
