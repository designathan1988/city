// The code of src/world/poleLines.ts that is the feature's (the rest of the file is shared).

// src/world/poleLines.ts:28-37 slice of src/world/poleLines.ts#PoleLines (shared): the feature's statements; "…" marks code left out
export interface PoleLines {
  // … 5 lines
  readonly accesses: readonly CrossingAccess[];
// … 2 lines
}

// src/world/poleLines.ts:230-257 slice of src/world/poleLines.ts#poleLinesOf (shared): the feature's statements; "…" marks code left out
export function poleLinesOf(net: Network, rect: ChangeRect): PoleLines {
  // … 19 lines
  const lines: PoleLines = {
    // … 1 lines
    accesses: crossingAccessesAt(net, [...ends].sort((a, b) => a - b)),
  };
// … 3 lines
}
