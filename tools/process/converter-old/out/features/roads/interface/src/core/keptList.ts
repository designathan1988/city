// What the feature uses of src/core/keptList.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/keptList.ts:2-85 class KeptList
/** DO NOT write back the list a build could read */
export class KeptList { … }

// src/core/keptList.ts:30-42 method KeptList.readShared
/** As `read`, but the same list (by identity, not parsed again) while the stored text is the same: for callers that redraw by it. */
  readShared<T>(read: (entry: unknown) => T | null): readonly T[];

// src/core/keptList.ts:45-55 method KeptList.update
/** Stores `change(entries)`; false when nothing was written (storage blocked or full). */
  update(change: (entries: readonly unknown[]) => readonly unknown[]): boolean;
