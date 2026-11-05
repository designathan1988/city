// The code of src/networkBake.worker.ts that is the feature's (the rest of the file is shared).

// src/networkBake.worker.ts:21-21 slice of src/networkBake.worker.ts#NetworkBakeReply (shared): the feature's statements; "…" marks code left out
export type NetworkBakeReply = { readonly id: number; readonly bake?: NetworkBake; readonly error?: string };

// src/networkBake.worker.ts:24-30 slice of src/networkBake.worker.ts#bakeNetwork (shared): the feature's statements; "…" marks code left out
export function bakeNetwork(ask: Omit<NetworkBakeAsk, 'id'>): NetworkBake {
  // … 2 lines
  const net = new Network(doc);
  net.rebuild({ full: true });
  return net.exportBake(doc.nodes.keys(), doc.segments.keys());
}
