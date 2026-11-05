// The code of src/ui/roads/furnitureChoice.ts that is the feature's (the rest of the file is shared).

// src/ui/roads/furnitureChoice.ts:18-22 function setFurnitureChosen: the feature's own
export function setFurnitureChosen(set: FurnitureSet): void {
  if (set === chosen) return;
  chosen = set;
  try { globalThis.localStorage?.setItem(STORAGE_KEY, set); } catch { /* kept for this session */ }
}
