// The code of src/core/forkMap.ts that is the feature's (the rest of the file is shared).

// src/core/forkMap.ts:37-47 slice of src/core/forkMap.ts#ForkMap.writable (shared): the feature's statements; "…" marks code left out
  writable(key: K, copy: (value: V) => V): V | undefined {
    // … 6 lines
    forkWork.copied++;
  // … 2 lines
  }
