// The code of src/render/signals.ts that is the feature's (the rest of the file is shared).

// src/render/signals.ts:72-80 slice of src/render/signals.ts#Head (shared): the feature's statements; "…" marks code left out
interface Head {
  // … 6 lines
  state: SignalState | null;
}

// src/render/signals.ts:105-730 slice of src/render/signals.ts#createSignalHeads (shared): the feature's statements; "…" marks code left out
export function createSignalHeads(
// … 2 lines
): SignalHeads {
  // … 379 lines
  const shown = new Map<string, SignalState>();
  group.userData.heads = shown;
// … 240 lines
}
