// What the feature uses of src/world/derived.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/derived.ts:6-37 class JournalCursor
/** DO NOT key a product on a global revision (`${doc.buildings.revision}`) and build the map again */

/** ONE WAY TO READ THE DIARY the cursor every reader shares, instead of fourteen hand-written  calls each with its own */
export class JournalCursor implements JournalReader { … }

// src/world/derived.ts:19-30 method JournalCursor.read
/** The entries of its kinds since the last read, oldest first; null: everything changed. */
  read(journal: ChangeJournal): readonly Change[] | null;
