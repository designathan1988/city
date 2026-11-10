// The code of src/conflictTable.worker.ts that is the feature's (the rest of the file is shared).

// src/conflictTable.worker.ts:13-164 slice of src/conflictTable.worker.ts#serve (shared): the feature's statements; "…" marks code left out
export function serve(self: JobScope): void {
  // … 42 lines
  interface JunctionsAsk {
    readonly kind: 'junctions';
    readonly id: number;
    readonly asks: readonly JunctionAsk[];
  }
  // … 10 lines
  self.onmessage = async (event: MessageEvent<PairsAsk | TurnsAsk | SweepsAsk | FinishAsk | CornersAsk | JunctionsAsk>) => {
    // … 6 lines
    try {
      if (ask.kind === 'turns') {
        const made = new Map<string, JunctionSurface>();
        const turns: ChosenTurn[] = [];
        for (const t of ask.turns) {
          // … 1 lines
          turns.push(chooseTurnOf({ ...t, surfaceKey: s.key }, s, made));
        // … 1 lines
        }
        self.postMessage({ id: ask.id, turns });
      // … 1 lines
      }
      if (ask.kind === 'junctions') {
        // … 1 lines
        const junctions: JunctionAnswer[] = [];
        // … 1 lines
        for (const junction of ask.asks) {
          // … 1 lines
          for (const c of built.connectors) transfer.add(c.path.buffer as ArrayBuffer);
          junctions.push(built);
        // … 1 lines
        }
        (self as unknown as { postMessage(message: unknown, transfer: Transferable[]): void }).postMessage({ id: ask.id, junctions }, [...transfer]);
      // … 1 lines
      }
      // … 12 lines
      if (ask.kind === 'finish') {
        // … 2 lines
        const finished: FinishedPair[] = [];
        for (let k = 0; k < ask.refs.length; k += 2) {
          // … 1 lines
          finished.push(finishPair(points[ask.refs[k]!]!, points[ask.refs[k + 1]!]!, ask.tables.subarray(q * TABLE_SIZE, (q + 1) * TABLE_SIZE)));
        // … 1 lines
        }
        self.postMessage({ id: ask.id, finished });
      // … 1 lines
      }
      if (ask.kind === 'sweeps') {
        // … 1 lines
        const sweeps: SampledSweep[] = [];
        // … 1 lines
        for (let q = 0; q < ask.c0.length; q++) {
          const sweep = sampleSweep({
            inXY: ask.lines[ask.refs[q * 3]!]!, crossXY: ask.lines[ask.refs[q * 3 + 1]!]!, outXY: ask.lines[ask.refs[q * 3 + 2]!]!,
            c0: ask.c0[q]!, count: ask.count[q]!,
          }, made);
          sweeps.push(sweep);
          transfer.push(sweep.frame.buffer as ArrayBuffer, sweep.frameLengthSq.buffer as ArrayBuffer);
        // … 1 lines
        }
        (self as unknown as { postMessage(message: unknown, transfer: Transferable[]): void }).postMessage({ id: ask.id, sweeps }, transfer);
      // … 1 lines
      }
      // … 1 lines
      const samples: SweepSamples[] = ask.sweeps.map((s) => ({ ...s, bounds: null, grid: null, range: null }));
      // … 3 lines
      const work = new PairWork();
      for (let k = 0; k < ask.pairs.length; k += 2) {
        const table = pairTable(samples[ask.pairs[k]!]!, samples[ask.pairs[k + 1]!]!, work);
        tables.push(table);
        if (table) transfer.push(table.buffer as ArrayBuffer);
      // … 1 lines
      }
    // … 3 lines
    } catch (error) {
    // … 3 lines
    }
  };
}

// src/conflictTable.worker.ts:56-60 slice of src/conflictTable.worker.ts#JunctionsAsk (shared): the feature's statements; "…" marks code left out
  interface JunctionsAsk {
    // … 2 lines
    readonly asks: readonly JunctionAsk[];
  }
