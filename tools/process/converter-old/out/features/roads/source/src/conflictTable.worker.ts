// The code of src/conflictTable.worker.ts that is the feature's (the rest of the file is shared).

// src/conflictTable.worker.ts:13-164 slice of src/conflictTable.worker.ts#serve (shared): the feature's statements; "…" marks code left out
export function serve(self: JobScope): void {
  // … 57 lines
  self.onmessage = async (event: MessageEvent<PairsAsk | TurnsAsk | SweepsAsk | FinishAsk | CornersAsk | JunctionsAsk>) => {
    // … 6 lines
    try {
      // … 11 lines
      if (ask.kind === 'junctions') {
        // … 3 lines
        for (const junction of ask.asks) {
          const built = buildJunctionOf(junction, lines, rings);
          for (const c of built.connectors) transfer.add(c.path.buffer as ArrayBuffer);
          junctions.push(built);
        // … 1 lines
        }
      // … 2 lines
      }
    // … 55 lines
    } catch (error) {
    // … 3 lines
    }
  };
}
