// The code of src/conflictPool.ts that is the feature's (the rest of the file is shared).

// src/conflictPool.ts:21-280 slice of src/conflictPool.ts#ConflictPool (shared): the feature's statements; "…" marks code left out
export class ConflictPool implements PairMeasurer {
  // … 12 lines
  measure(pairs: readonly { readonly a: SweepSamples; readonly b: SweepSamples; readonly key?: string }[]): Promise<readonly (Float64Array | null)[]> {
    const lanes = this.start();
    if (!lanes) return Promise.reject(new Error('no workers in this browser'));
    // Contiguous shares, one a lane, each with only the sweeps its pairs read.
    const per = Math.ceil(pairs.length / lanes.length);
    const parts: Promise<(Float64Array | null)[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < pairs.length; k++) {
      const share = pairs.slice(k * per, (k + 1) * per);
      const index = new Map<SweepSamples, number>();
      const sweeps: { c0: number; count: number; length: number; frame: Float64Array; frameLengthSq: Float64Array }[] = [];
      const at = (s: SweepSamples): number => {
        let i = index.get(s);
        if (i === undefined) {
          i = sweeps.length;
          index.set(s, i);
          sweeps.push({ c0: s.c0, count: s.count, length: s.length, frame: s.frame, frameLengthSq: s.frameLengthSq });
        }
        return i;
      };
      const indices = new Int32Array(share.length * 2);
      share.forEach((p, q) => { indices[q * 2] = at(p.a); indices[q * 2 + 1] = at(p.b); });
      const keep = this.keepPrefix !== null ? { prefix: this.keepPrefix, keys: share.map((p) => p.key ?? '') } : null;
      parts.push(this.ask(lanes[k]!, sweeps, indices, keep));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }
  // … 12 lines
  chooseTurns(asks: readonly TurnAsk[], rings: ReadonlyMap<string, SurfaceRings>): Promise<readonly ChosenTurn[]> {
    const lanes = this.start();
    if (!lanes) return Promise.reject(new Error('no workers in this browser'));
    const per = Math.ceil(asks.length / lanes.length);
    const parts: Promise<ChosenTurn[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < asks.length; k++) {
      const share = asks.slice(k * per, (k + 1) * per);
      const index = new Map<string, number>();
      const surfaces: { centre: { x: number; y: number }; rings: readonly Float64Array[]; key: string }[] = [];
      const turns = share.map((a) => {
        let at = index.get(a.surfaceKey);
        if (at === undefined) {
          at = surfaces.length;
          index.set(a.surfaceKey, at);
          const r = rings.get(a.surfaceKey)!;
          surfaces.push({ centre: r.centre, rings: r.rings, key: a.surfaceKey });
        }
        return { surface: at, inXY: a.inXY, outXY: a.outXY, waiting: a.waiting, fromRoadEnd: a.fromRoadEnd, toRoadEnd: a.toRoadEnd };
      });
      const lane = lanes[k]!;
      const id = this.nextId++;
      parts.push(new Promise<ChosenTurn[]>((resolve, reject) => {
        lane.waiting.set(id, { resolve: (answer) => resolve(answer as ChosenTurn[]), reject });
        lane.worker.postMessage({ kind: 'turns', id, surfaces, turns });
        this.arm(lane);
      }));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }
  // … 2 lines
  sampleSweeps(asks: readonly SweepAsk[]): Promise<readonly SampledSweep[]> {
    const lanes = this.start();
    if (!lanes) return Promise.reject(new Error('no workers in this browser'));
    const per = Math.ceil(asks.length / lanes.length);
    const parts: Promise<SampledSweep[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < asks.length; k++) {
      const share = asks.slice(k * per, (k + 1) * per);
      const index = new Map<Float64Array, number>();
      const lines: Float64Array[] = [];
      const at = (xy: Float64Array): number => {
        let i = index.get(xy);
        if (i === undefined) { i = lines.length; index.set(xy, i); lines.push(xy); }
        return i;
      };
      const refs = new Int32Array(share.length * 3);
      const c0 = new Float64Array(share.length), count = new Int32Array(share.length);
      share.forEach((a, q) => {
        refs[q * 3] = at(a.inXY); refs[q * 3 + 1] = at(a.crossXY); refs[q * 3 + 2] = at(a.outXY);
        c0[q] = a.c0; count[q] = a.count;
      });
      const lane = lanes[k]!;
      const id = this.nextId++;
      parts.push(new Promise<SampledSweep[]>((resolve, reject) => {
        lane.waiting.set(id, { resolve: (answer) => resolve(answer as SampledSweep[]), reject });
        // The lanes' points are copied (the page keeps its own); the indices and the numbers handed over.
        lane.worker.postMessage({ kind: 'sweeps', id, lines, refs, c0, count }, [refs.buffer, c0.buffer, count.buffer]);
        this.arm(lane);
      }));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }
  // … 2 lines
  finishPairs(pairs: readonly FinishAsk[]): Promise<readonly FinishedPair[]> {
    const lanes = this.start();
    if (!lanes) return Promise.reject(new Error('no workers in this browser'));
    const per = Math.ceil(pairs.length / lanes.length);
    const parts: Promise<FinishedPair[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < pairs.length; k++) {
      const share = pairs.slice(k * per, (k + 1) * per);
      const index = new Map<Float64Array, number>();
      const lines: Float64Array[] = [];
      const at = (xy: Float64Array): number => {
        let i = index.get(xy);
        if (i === undefined) { i = lines.length; index.set(xy, i); lines.push(xy); }
        return i;
      };
      const refs = new Int32Array(share.length * 2);
      const tables = new Float64Array(share.length * TABLE_SIZE);
      share.forEach((p, q) => { refs[q * 2] = at(p.a); refs[q * 2 + 1] = at(p.b); tables.set(p.table, q * TABLE_SIZE); });
      const lane = lanes[k]!;
      const id = this.nextId++;
      parts.push(new Promise<FinishedPair[]>((resolve, reject) => {
        lane.waiting.set(id, { resolve: (answer) => resolve(answer as FinishedPair[]), reject });
        lane.worker.postMessage({ kind: 'finish', id, lines, refs, tables }, [refs.buffer, tables.buffer]);
        this.arm(lane);
      }));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }
  // … 2 lines
  buildJunctions(asks: readonly JunctionAsk[]): Promise<readonly JunctionAnswer[]> {
    const lanes = this.start();
    if (!lanes) return Promise.reject(new Error('no workers in this browser'));
    const per = Math.ceil(asks.length / lanes.length);
    const parts: Promise<JunctionAnswer[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < asks.length; k++) {
      const share = asks.slice(k * per, (k + 1) * per);
      const lane = lanes[k]!;
      const id = this.nextId++;
      parts.push(new Promise<JunctionAnswer[]>((resolve, reject) => {
        lane.waiting.set(id, { resolve: (answer) => resolve(answer as JunctionAnswer[]), reject });
        lane.worker.postMessage({ kind: 'junctions', id, asks: share });
        this.arm(lane);
      }));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }
  // … 28 lines
  private start(): Lane[] | null {
    // … 4 lines
    try {
      for (let k = 0; k < count; k++) {
        // … 2 lines
        worker.onmessage = (event: MessageEvent<{ id: number; beat?: true; tables?: (Float64Array | null)[]; turns?: ChosenTurn[]; sweeps?: SampledSweep[]; finished?: FinishedPair[]; corners?: (Float64Array | null)[][]; junctions?: JunctionAnswer[]; error?: string; laneFailed?: string }>) => {
          if (this.lanes !== lanes) return;
          if (event.data.laneFailed !== undefined) { this.fail(`a conflict lane could not load: ${event.data.laneFailed}`); return; }
          const job = event.data.beat ? undefined : lane.waiting.get(event.data.id);
          if (job) lane.waiting.delete(event.data.id);
          // Any word from the worker, a beat or an answer: it is alive.
          this.arm(lane);
          if (!job) return;
          if (event.data.tables) job.resolve(event.data.tables);
          else if (event.data.turns) job.resolve(event.data.turns);
          else if (event.data.sweeps) job.resolve(event.data.sweeps);
          else if (event.data.finished) job.resolve(event.data.finished);
          else if (event.data.corners) job.resolve(event.data.corners);
          else if (event.data.junctions) job.resolve(event.data.junctions);
          else job.reject(new Error(event.data.error ?? 'conflict worker failed'));
        };
      // … 4 lines
      }
    // … 1 lines
    } catch (error) {
    // … 5 lines
    }
  // … 1 lines
  }
// … 30 lines
}

// src/conflictPool.ts:34-59 method ConflictPool.measure: the feature's own
  measure(pairs: readonly { readonly a: SweepSamples; readonly b: SweepSamples; readonly key?: string }[]): Promise<readonly (Float64Array | null)[]> {
    const lanes = this.start();
    if (!lanes) return Promise.reject(new Error('no workers in this browser'));
    // Contiguous shares, one a lane, each with only the sweeps its pairs read.
    const per = Math.ceil(pairs.length / lanes.length);
    const parts: Promise<(Float64Array | null)[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < pairs.length; k++) {
      const share = pairs.slice(k * per, (k + 1) * per);
      const index = new Map<SweepSamples, number>();
      const sweeps: { c0: number; count: number; length: number; frame: Float64Array; frameLengthSq: Float64Array }[] = [];
      const at = (s: SweepSamples): number => {
        let i = index.get(s);
        if (i === undefined) {
          i = sweeps.length;
          index.set(s, i);
          sweeps.push({ c0: s.c0, count: s.count, length: s.length, frame: s.frame, frameLengthSq: s.frameLengthSq });
        }
        return i;
      };
      const indices = new Int32Array(share.length * 2);
      share.forEach((p, q) => { indices[q * 2] = at(p.a); indices[q * 2 + 1] = at(p.b); });
      const keep = this.keepPrefix !== null ? { prefix: this.keepPrefix, keys: share.map((p) => p.key ?? '') } : null;
      parts.push(this.ask(lanes[k]!, sweeps, indices, keep));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }

// src/conflictPool.ts:72-100 slice of src/conflictPool.ts#ConflictPool.chooseTurns (shared): the feature's statements; "…" marks code left out
  chooseTurns(asks: readonly TurnAsk[], rings: ReadonlyMap<string, SurfaceRings>): Promise<readonly ChosenTurn[]> {
    // … 2 lines
    const per = Math.ceil(asks.length / lanes.length);
    const parts: Promise<ChosenTurn[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < asks.length; k++) {
      const share = asks.slice(k * per, (k + 1) * per);
      const index = new Map<string, number>();
      const surfaces: { centre: { x: number; y: number }; rings: readonly Float64Array[]; key: string }[] = [];
      const turns = share.map((a) => {
        let at = index.get(a.surfaceKey);
        if (at === undefined) {
          at = surfaces.length;
          index.set(a.surfaceKey, at);
          const r = rings.get(a.surfaceKey)!;
          surfaces.push({ centre: r.centre, rings: r.rings, key: a.surfaceKey });
        }
        return { surface: at, inXY: a.inXY, outXY: a.outXY, waiting: a.waiting, fromRoadEnd: a.fromRoadEnd, toRoadEnd: a.toRoadEnd };
      });
      const lane = lanes[k]!;
      const id = this.nextId++;
      parts.push(new Promise<ChosenTurn[]>((resolve, reject) => {
        lane.waiting.set(id, { resolve: (answer) => resolve(answer as ChosenTurn[]), reject });
        lane.worker.postMessage({ kind: 'turns', id, surfaces, turns });
        this.arm(lane);
      }));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }

// src/conflictPool.ts:103-133 slice of src/conflictPool.ts#ConflictPool.sampleSweeps (shared): the feature's statements; "…" marks code left out
  sampleSweeps(asks: readonly SweepAsk[]): Promise<readonly SampledSweep[]> {
    // … 2 lines
    const per = Math.ceil(asks.length / lanes.length);
    const parts: Promise<SampledSweep[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < asks.length; k++) {
      const share = asks.slice(k * per, (k + 1) * per);
      const index = new Map<Float64Array, number>();
      const lines: Float64Array[] = [];
      const at = (xy: Float64Array): number => {
        let i = index.get(xy);
        if (i === undefined) { i = lines.length; index.set(xy, i); lines.push(xy); }
        return i;
      };
      const refs = new Int32Array(share.length * 3);
      const c0 = new Float64Array(share.length), count = new Int32Array(share.length);
      share.forEach((a, q) => {
        refs[q * 3] = at(a.inXY); refs[q * 3 + 1] = at(a.crossXY); refs[q * 3 + 2] = at(a.outXY);
        c0[q] = a.c0; count[q] = a.count;
      });
      const lane = lanes[k]!;
      const id = this.nextId++;
      parts.push(new Promise<SampledSweep[]>((resolve, reject) => {
        lane.waiting.set(id, { resolve: (answer) => resolve(answer as SampledSweep[]), reject });
        // The lanes' points are copied (the page keeps its own); the indices and the numbers handed over.
        lane.worker.postMessage({ kind: 'sweeps', id, lines, refs, c0, count }, [refs.buffer, c0.buffer, count.buffer]);
        this.arm(lane);
      }));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }

// src/conflictPool.ts:136-162 slice of src/conflictPool.ts#ConflictPool.finishPairs (shared): the feature's statements; "…" marks code left out
  finishPairs(pairs: readonly FinishAsk[]): Promise<readonly FinishedPair[]> {
    // … 2 lines
    const per = Math.ceil(pairs.length / lanes.length);
    const parts: Promise<FinishedPair[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < pairs.length; k++) {
      const share = pairs.slice(k * per, (k + 1) * per);
      const index = new Map<Float64Array, number>();
      const lines: Float64Array[] = [];
      const at = (xy: Float64Array): number => {
        let i = index.get(xy);
        if (i === undefined) { i = lines.length; index.set(xy, i); lines.push(xy); }
        return i;
      };
      const refs = new Int32Array(share.length * 2);
      const tables = new Float64Array(share.length * TABLE_SIZE);
      share.forEach((p, q) => { refs[q * 2] = at(p.a); refs[q * 2 + 1] = at(p.b); tables.set(p.table, q * TABLE_SIZE); });
      const lane = lanes[k]!;
      const id = this.nextId++;
      parts.push(new Promise<FinishedPair[]>((resolve, reject) => {
        lane.waiting.set(id, { resolve: (answer) => resolve(answer as FinishedPair[]), reject });
        lane.worker.postMessage({ kind: 'finish', id, lines, refs, tables }, [refs.buffer, tables.buffer]);
        this.arm(lane);
      }));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }

// src/conflictPool.ts:165-181 slice of src/conflictPool.ts#ConflictPool.buildJunctions (shared): the feature's statements; "…" marks code left out
  buildJunctions(asks: readonly JunctionAsk[]): Promise<readonly JunctionAnswer[]> {
    // … 2 lines
    const per = Math.ceil(asks.length / lanes.length);
    const parts: Promise<JunctionAnswer[]>[] = [];
    for (let k = 0; k < lanes.length && k * per < asks.length; k++) {
      const share = asks.slice(k * per, (k + 1) * per);
      const lane = lanes[k]!;
      const id = this.nextId++;
      parts.push(new Promise<JunctionAnswer[]>((resolve, reject) => {
        lane.waiting.set(id, { resolve: (answer) => resolve(answer as JunctionAnswer[]), reject });
        lane.worker.postMessage({ kind: 'junctions', id, asks: share });
        this.arm(lane);
      }));
    }
    return Promise.all(parts).then((answers) => answers.flat());
  }

// src/conflictPool.ts:210-249 slice of src/conflictPool.ts#ConflictPool.start (shared): the feature's statements; "…" marks code left out
  private start(): Lane[] | null {
    // … 4 lines
    try {
      for (let k = 0; k < count; k++) {
        // … 2 lines
        worker.onmessage = (event: MessageEvent<{ id: number; beat?: true; tables?: (Float64Array | null)[]; turns?: ChosenTurn[]; sweeps?: SampledSweep[]; finished?: FinishedPair[]; corners?: (Float64Array | null)[][]; junctions?: JunctionAnswer[]; error?: string; laneFailed?: string }>) => {
          if (this.lanes !== lanes) return;
          if (event.data.laneFailed !== undefined) { this.fail(`a conflict lane could not load: ${event.data.laneFailed}`); return; }
          const job = event.data.beat ? undefined : lane.waiting.get(event.data.id);
          if (job) lane.waiting.delete(event.data.id);
          // Any word from the worker, a beat or an answer: it is alive.
          this.arm(lane);
          if (!job) return;
          if (event.data.tables) job.resolve(event.data.tables);
          else if (event.data.turns) job.resolve(event.data.turns);
          else if (event.data.sweeps) job.resolve(event.data.sweeps);
          else if (event.data.finished) job.resolve(event.data.finished);
          else if (event.data.corners) job.resolve(event.data.corners);
          else if (event.data.junctions) job.resolve(event.data.junctions);
          else job.reject(new Error(event.data.error ?? 'conflict worker failed'));
        };
      // … 4 lines
      }
    // … 1 lines
    } catch (error) {
    // … 5 lines
    }
  // … 1 lines
  }
