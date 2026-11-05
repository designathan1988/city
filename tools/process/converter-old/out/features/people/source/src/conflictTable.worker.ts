// The code of src/conflictTable.worker.ts that is the feature's (the rest of the file is shared).

// src/conflictTable.worker.ts:13-164 slice of src/conflictTable.worker.ts#serve (shared): the feature's statements; "…" marks code left out
export function serve(self: JobScope): void {
  // … 48 lines
  interface CornersAsk {
    readonly kind: 'corners';
    readonly id: number;
    readonly asks: readonly CornerAsk[];
  }
  // … 4 lines
  self.onmessage = async (event: MessageEvent<PairsAsk | TurnsAsk | SweepsAsk | FinishAsk | CornersAsk | JunctionsAsk>) => {
    // … 6 lines
    try {
      // … 24 lines
      if (ask.kind === 'corners') {
        // … 2 lines
        for (const node of ask.asks) {
          const lines = roundCorners(node);
          for (const line of lines) if (line) transfer.push(line.buffer as ArrayBuffer);
          corners.push(lines);
        // … 1 lines
        }
      // … 2 lines
      }
    // … 43 lines
    } catch (error) {
    // … 3 lines
    }
  };
}

// src/conflictTable.worker.ts:62-66 slice of src/conflictTable.worker.ts#CornersAsk (shared): the feature's statements; "…" marks code left out
  interface CornersAsk {
    // … 2 lines
    readonly asks: readonly CornerAsk[];
  }
