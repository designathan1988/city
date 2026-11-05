// What the feature uses of src/core/keptList.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/keptList.ts:2-85 class KeptList
/** DO NOT write back the list a build could read */
export class KeptList { … }

// src/core/keptList.ts:16-27 method KeptList.read
/** The entries this build reads, each by `read` (null: not read, kept as it is) */
  read<T>(read: (entry: unknown) => T | null): T[];

// src/core/keptList.ts:45-55 method KeptList.update
/** Stores `change(entries)`; false when nothing was written (storage blocked or full). */
  update(change: (entries: readonly unknown[]) => readonly unknown[]): boolean;
