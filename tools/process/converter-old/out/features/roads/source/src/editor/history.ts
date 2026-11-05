// The code of src/editor/history.ts that is the feature's (the rest of the file is shared).

// src/editor/history.ts:159-161 slice of src/editor/history.ts#followNetwork (shared): the feature's statements; "…" marks code left out
export function followNetwork(doc: RoadDoc, net: Network): void {
  if (net.revision !== doc.revision) net.rebuild();
}

// src/editor/history.ts:176-185 slice of src/editor/history.ts#restoreInto (shared): the feature's statements; "…" marks code left out
export function restoreInto(target: RoadDoc, data: SerializedDoc, net: Network, table: ArrayBufferLike | null = null): void {
  // … 4 lines
  net.rebuild({ full: true });
  // … 1 lines
  if (!generated) repairNearConnections(target, net);
// … 1 lines
}

// src/editor/history.ts:188-192 slice of src/editor/history.ts#restoreSnapshot (shared): the feature's statements; "…" marks code left out
export function restoreSnapshot(target: RoadDoc, data: SerializedDoc, net: Network): void {
  // … 2 lines
  if (net.revision !== target.revision) net.rebuild();
}
