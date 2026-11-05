// The code of src/bayPool.ts that is the feature's (the rest of the file is shared).

// src/bayPool.ts:7-14 slice of src/bayPool.ts#Lane (shared): the feature's statements; "…" marks code left out
interface Lane {
  // … 1 lines
  readonly waiting: Map<number, { resolve(answer: Bay[][]): void; reject(error: Error): void }>;
// … 4 lines
}

// src/bayPool.ts:22-124 slice of src/bayPool.ts#BayPool (shared): the feature's statements; "…" marks code left out
export class BayPool implements BayWorkers {
  // … 9 lines
  work(asks: readonly LotAsk[]): Promise<Bay[][]> {
    const lanes = this.start();
    if (!lanes) return Promise.reject(new Error('no workers in this browser'));
    // Contiguous shares, one a lane: neighbouring lots read the same lanes and walls, copied once a share.
    const per = Math.ceil(asks.length / lanes.length);
    const parts: Promise<Bay[][]>[] = [];
    for (let k = 0; k * per < asks.length; k++) {
      const lane = lanes[(this.turn + k) % lanes.length]!;
      const share = shareOf(asks.slice(k * per, (k + 1) * per));
      const id = this.nextId++;
      parts.push(new Promise<Bay[][]>((resolve, reject) => {
        lane.waiting.set(id, { resolve, reject });
        lane.heard = performance.now();
        lane.worker.postMessage({ id, share });
      }));
    }
    this.turn = (this.turn + parts.length) % lanes.length;
    return Promise.all(parts).then((answers) => answers.flat());
  }
  // … 1 lines
  check(now = performance.now()): void {
    for (const lane of this.lanes ?? []) {
      if (lane.told || !lane.waiting.size || now - lane.heard <= SILENCE_MS) continue;
      // Said once a stretch, and waited for: the page working the lots out itself would stall it.
      lane.told = true;
      console.warn(`[parking] a bays worker has been silent for ${Math.round(now - lane.heard)} ms with lots waiting (busy or without CPU); still waiting`);
    }
  }
  // … 1 lines
  private start(): Lane[] | null {
    // … 4 lines
    try {
      for (let k = 0; k < count; k++) {
        // … 2 lines
        worker.onmessage = (event: MessageEvent<{ id: number; beat?: true; bays?: Bay[][]; error?: string; laneFailed?: string }>) => {
          if (this.lanes !== lanes) return;
          if (event.data.laneFailed !== undefined) { this.fail(`a bays lane could not load: ${event.data.laneFailed}`); return; }
          // Any word from the worker, a beat or an answer: it is alive.
          lane.heard = performance.now();
          lane.told = false;
          const job = event.data.beat ? undefined : lane.waiting.get(event.data.id);
          if (!job) return;
          lane.waiting.delete(event.data.id);
          if (event.data.bays) job.resolve(event.data.bays);
          else job.reject(new Error(event.data.error ?? 'bays worker failed'));
        };
      // … 3 lines
      }
    // … 1 lines
    } catch (error) {
    // … 5 lines
    }
  // … 1 lines
  }
// … 28 lines
}

// src/bayPool.ts:32-50 slice of src/bayPool.ts#BayPool.work (shared): the feature's statements; "…" marks code left out
  work(asks: readonly LotAsk[]): Promise<Bay[][]> {
    // … 3 lines
    const per = Math.ceil(asks.length / lanes.length);
    const parts: Promise<Bay[][]>[] = [];
    for (let k = 0; k * per < asks.length; k++) {
      const lane = lanes[(this.turn + k) % lanes.length]!;
      const share = shareOf(asks.slice(k * per, (k + 1) * per));
      const id = this.nextId++;
      parts.push(new Promise<Bay[][]>((resolve, reject) => {
        lane.waiting.set(id, { resolve, reject });
        lane.heard = performance.now();
        lane.worker.postMessage({ id, share });
      }));
    }
    this.turn = (this.turn + parts.length) % lanes.length;
    return Promise.all(parts).then((answers) => answers.flat());
  }

// src/bayPool.ts:52-59 method BayPool.check: the feature's own
  check(now = performance.now()): void {
    for (const lane of this.lanes ?? []) {
      if (lane.told || !lane.waiting.size || now - lane.heard <= SILENCE_MS) continue;
      // Said once a stretch, and waited for: the page working the lots out itself would stall it.
      lane.told = true;
      console.warn(`[parking] a bays worker has been silent for ${Math.round(now - lane.heard)} ms with lots waiting (busy or without CPU); still waiting`);
    }
  }

// src/bayPool.ts:61-95 slice of src/bayPool.ts#BayPool.start (shared): the feature's statements; "…" marks code left out
  private start(): Lane[] | null {
    // … 4 lines
    try {
      for (let k = 0; k < count; k++) {
        // … 2 lines
        worker.onmessage = (event: MessageEvent<{ id: number; beat?: true; bays?: Bay[][]; error?: string; laneFailed?: string }>) => {
          if (this.lanes !== lanes) return;
          if (event.data.laneFailed !== undefined) { this.fail(`a bays lane could not load: ${event.data.laneFailed}`); return; }
          // Any word from the worker, a beat or an answer: it is alive.
          lane.heard = performance.now();
          lane.told = false;
          const job = event.data.beat ? undefined : lane.waiting.get(event.data.id);
          if (!job) return;
          lane.waiting.delete(event.data.id);
          if (event.data.bays) job.resolve(event.data.bays);
          else job.reject(new Error(event.data.error ?? 'bays worker failed'));
        };
      // … 3 lines
      }
    // … 1 lines
    } catch (error) {
    // … 5 lines
    }
  // … 1 lines
  }
