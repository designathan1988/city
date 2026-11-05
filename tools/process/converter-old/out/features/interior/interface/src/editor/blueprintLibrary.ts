// What the feature uses of src/editor/blueprintLibrary.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/blueprintLibrary.ts:10-16 interface StoredBlueprint
interface StoredBlueprint {
  readonly key: string;
  readonly name: string;
  readonly body: unknown;
  /** Metres per world unit it was saved in; absent: 0.4, before 2026-10-10. */
  readonly unit?: number;
}

// src/editor/blueprintLibrary.ts:18-19 function isStored
const isStored = (entry: unknown): entry is StoredBlueprint => …;

// src/editor/blueprintLibrary.ts:52-56 function readEntry
/** One stored entry as a blueprint, or null when this build cannot read it (it stays stored). */
function readEntry(entry: unknown): Blueprint | null;
