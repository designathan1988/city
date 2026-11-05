// The code of src/render/buildings/cellPool.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/cellPool.ts:72-77 slice of src/render/buildings/cellPool.ts#pavingRect (shared): the feature's statements; "…" marks code left out
function pavingRect(cell: string): [number, number, number, number] | null {
  // … 3 lines
  return [i0 * BATCH_CELL - PAVING_REACH, j0 * BATCH_CELL - PAVING_REACH, (i0 + n) * BATCH_CELL + PAVING_REACH, (j0 + n) * BATCH_CELL + PAVING_REACH];
}

// src/render/buildings/cellPool.ts:150-155 method FloorColumns.retain: the feature's own
  retain(keep: (key: string) => boolean): void {
    const gone: number[] = [];
    this.slots.forEach((id) => { if (!keep(String(id))) gone.push(id); });
    for (const id of gone) this.slots.delete(id);
    for (const key of [...this.cut.keys()]) if (!keep(key)) this.cut.delete(key);
  }

// src/render/buildings/cellPool.ts:158-396 class CellPool: the feature's own
export class CellPool {
  private readonly asked = new Map<number, Asked>();
  /** DO NOT measure a building's floor in each job of it (its cell's masses, its tile's, its plate's, its detail, its far level): measured once, handed with the next jobs while its record and ground are the same (`CellEntry.floor`). */
  private readonly floors = new FloorColumns();
  /** Each key's job asked: a newer one replaces it (the queue drops it too). */
  private readonly byKey = new Map<string, Asked>();
  private nextId = 1;
  private warned = false;
  /** DO NOT give the detail the queue first while the town is behind its curtain: the opening shows the masses and the far levels, the detail lands after (`renderer.ts` reveal), and the cells' detail took the queue's two workers while the coarse levels the curtain waits for stood behind it (2026-10-16, 90 s at the curtain's limit, the workers' profile). Set by the layer as it is hidden or shown. */
  get coarseFirst(): boolean {
    return this.coarse;
  }
  /** DO NOT leave the detail asked behind the curtain in its band once the town shows: the queue ranks a job by the band it was asked in, and the cell at the camera after a reload waited behind the whole re-bake (conferencia 97, picture 51: neither its detail nor its facade level 4 s on). The jobs no worker took are asked again in their band now. */
  set coarseFirst(on: boolean) {
    if (on === this.coarse) return;
    this.coarse = on;
    for (const asked of this.asked.values()) {
      if ((asked.kind !== 'cell' && asked.kind !== 'facade') || asked.sent || !asked.ticket?.pending) continue;
      const priority = cellPriority(asked.kind, 0, asked.first, on);
      if (priority === asked.priority) continue;
      asked.ticket.cancel();
      asked.priority = priority;
      this.submit(asked);
    }
  }
  private coarse = false;

  /** `workers`: false does every job on the page (tests). `tone`: each finish's colour from afar, for the masses (`kit.ts`). */
  /** `text`: where an entry's record is read (`recordTable.ts`), taken when the job is sent. */
  constructor(private readonly boxes: () => KitBoxes, private readonly text: (entry: CellEntry) => () => RecordRef, private readonly workers = true,
    private readonly tone?: Readonly<Record<Finish, readonly [number, number, number]>>) {
    // The kept keys read now, at the start, before the town's writes come (`keptCellKeys`).
    if (workers && canQueueJobs()) void keptCellKeys();
  }

  /** Jobs asked and not answered. */
  get running(): number {
    return this.asked.size;
  }

  /** A cell's job: its batches ('cell'), its far level ('facade'), its masses ('mass') or its buildings' summaries; `key`, what a newer job replaces */
  /** `hole`: nothing stands where it draws (a plate of the map with no masses yet): in the far levels' band (`FAR_TIER`), before the view's detail - a hole on the screen comes before any refinement of what is drawn. */
  run(kind: JobKind, cell: string, entries: readonly CellEntry[], env: CellEnv, done: (result: CellResult) => void, key: string | null = null, edited = false, hole = false): number {
    if (key !== null) { const older = this.byKey.get(key); if (older) this.cancel(older.id); }
    const at = squareOf(cell);
    // Its band; its distance to the view is the queue's, as the view moves (`JobSpec.at`). A building the player cut open goes first: what the click asked for (`layer.ts` `entryFor`).
    const place = at ? [(at.i0 + at.n / 2) * BATCH_CELL, (at.j0 + at.n / 2) * BATCH_CELL] as const : null;
    // A cell the player's edit changed (`edited`, `layer.ts` editedCells: a building demolished in a tile next to a road's) goes with the cut-open one: it draws what was just done.
    const first = edited || entries.some((e) => e.key.includes('|'));
    const priority = hole && !first ? FAR_TIER * TIER_SPAN : cellPriority(kind, 0, first, this.coarse);
    const offPage = this.offPage(env);
    const asked: Asked = {
      id: this.nextId++, kind, cell, entries, writers: entries.map((e) => this.text(e)), env, done, key, priority, first, sent: false, place,
      stored: offPage && !first ? this.storedKey(kind, cell, entries, env) : null, rect: pavingRect(cell), widened: 0, ticket: null,
    };
    if (!offPage) { this.onPage(asked); return asked.id; }
    this.asked.set(asked.id, asked);
    if (key !== null) this.byKey.set(key, asked);
    if (asked.stored === null) { this.submit(asked); return asked.id; }
    // Kept by an earlier session: taken as its job would answer it, the job never asked.
    const stored = asked.stored;
    void keptCellKeys().then((keys) => {
      if (this.asked.get(asked.id) !== asked) return undefined;
      if (!keys.has(stored)) { this.submit(asked); return undefined; }
      return readDerived<CellResult>(stored).catch(() => undefined).then((kept) => {
        if (this.asked.get(asked.id) !== asked) return;
        if (kept && typeof kept === 'object' && 'summaries' in kept) { this.forget(asked); this.learn(asked, kept); asked.done(kept); return; }
        this.submit(asked);
      });
    }, () => this.submit(asked));
    return asked.id;
  }

  /** The job system's workers made now, ahead of the first job. */
  warm(): void {
    if (this.workers && canQueueJobs()) warmJobWorkers();
  }

  /** A job no longer wanted: never sent if waiting, its answer dropped if begun (a worker runs a job through, one message). */
  cancel(id: number): void {
    const asked = this.asked.get(id);
    if (!asked) return;
    this.forget(asked);
    asked.ticket?.cancel();
  }

  /** A job a newer one for its cell replaces (`cancel`). */
  supersede(id: number): void {
    this.cancel(id);
  }

  /** Whether a job is still waiting or being worked on (one done on the page never is). */
  isRunning(id: number): boolean {
    return this.asked.has(id);
  }

  dispose(): void {
    for (const id of [...this.asked.keys()]) this.cancel(id);
  }

  /** DO NOT hand a worker a job it would have to ask the page about: only with the ground shared and the paving packable (`sharedGround.ts`, `PavingSource.pack`) */
  private offPage(env: CellEnv): boolean {
    if (!this.workers || !canQueueJobs()) return false;
    if (!(env.groundAt as SharedGroundAt).share?.current) return false;
    return env.pavedAt === undefined || env.pavedAt.pack !== undefined;
  }

  private submit(asked: Asked): void {
    asked.sent = false;
    asked.ticket = submitJob({
      service: 'cells',
      key: asked.key === null ? null : `cells:${asked.key}`,
      priority: asked.priority,
      ...(asked.place ? { at: asked.place } : {}),
      // Fit for a job process: its ground cut to the plates around it there (`write`).
      crosses: true,
      write: (to) => { asked.sent = true; return this.write(asked, to.sharesMemory); },
      done: (answer) => this.answer(asked, answer as WorkReply),
      failed: (message) => this.failed(asked, message),
    });
  }

  /** The job as its worker reads it: the records' shared tables and rows, the plates' shared buffers, the roads near the cell (MDN, transfer). */
  /** `sharesMemory`: false for a worker in another process (`jobs/processWorker.ts`), every shared buffer copied there: the plates around the cell alone (`groundAround`). */
  private write(asked: Asked, sharesMemory: boolean): { data: CellJob; transfer: ArrayBuffer[] } {
    const whole = (asked.env.groundAt as SharedGroundAt).share?.current;
    if (!whole) throw new Error('[buildings] the ground is not shared any more');
    const around = !sharesMemory && asked.rect ? asked.rect : null;
    const ground = around ? groundAround(whole, around) : whole;
    const packed = asked.env.pavedAt && asked.rect ? asked.env.pavedAt.pack!(asked.rect) : null;
    if (asked.env.pavedAt && asked.rect && !packed) throw new Error('[buildings] the paving cannot be handed to a worker');
    const job: CellJob = {
      ...this.base(asked), ground, ...(around ? { around } : {}),
      ...(asked.env.naturalAt === asked.env.groundAt ? { naturalIsGround: true } : {}),
      ...(packed ? { paving: packed.data } : {}),
    };
    return { data: job, transfer: packed?.transfer ?? [] };
  }

  /** What every job carries, on the page or off it. */
  private base(asked: Asked): CellJob {
    return {
      id: asked.id, kind: asked.kind, cell: asked.cell, paved: asked.env.pavedAt !== undefined, boxes: this.boxes(),
      entries: asked.entries.map((e, i) => {
        const floor = this.floorKnown(e, asked.env.pavedAt !== undefined);
        return { ...e, ...(floor !== undefined ? { floor } : {}), record: asked.writers[i]!() };
      }),
      ...(this.tone ? { tone: this.tone } : {}),
    };
  }

  /** A worker's answer: the result handed on, the job asked again over wider roads, or put together here. */
  private answer(asked: Asked, reply: WorkReply): void {
    if (this.asked.get(asked.id) !== asked) return;
    if (reply.kind === 'wider' && asked.widened < MOST_WIDER) {
      asked.widened++;
      asked.rect = [reply.rect[0], reply.rect[1], reply.rect[2], reply.rect[3]];
      this.submit(asked);
      return;
    }
    this.forget(asked);
    if (reply.kind === 'done') {
      // Kept before it is handed on: the store clones it now, its arrays are the layer's after (`keepDerivedNow`).
      const stored = asked.stored;
      if (stored !== null && keepDerivedNow(stored, reply.result)) void keptCellKeys().then((keys) => keys.add(stored));
      this.learn(asked, reply.result);
      asked.done(reply.result);
      return;
    }
    if (reply.kind === 'error') console.warn('[buildings] a cell worker failed on a cell; put together on the page', reply.message);
    this.onPage(asked);
  }

  /** The worker failed, or none runs: put together here. */
  private failed(asked: Asked, message: string): void {
    if (this.asked.get(asked.id) !== asked) return;
    if (!this.warned) { this.warned = true; console.warn('[buildings] a cell job could not run in a worker; put together on the page', message); }
    this.forget(asked);
    this.onPage(asked);
  }

  private forget(asked: Asked): void {
    this.asked.delete(asked.id);
    if (asked.key !== null && this.byKey.get(asked.key) === asked) this.byKey.delete(asked.key);
  }

  /** The job done here, every height read off the page's own ground. */
  private onPage(asked: Asked): void {
    const { groundAt, naturalAt, pavedAt } = asked.env;
    const reply = runCellJob(this.base(asked), { groundAt, naturalAt, ...(pavedAt ? { pavedAt } : {}) });
    if (reply.kind !== 'done') throw new Error(`[buildings] a cell put together on the page did not finish (${asked.cell})`);
    this.learn(asked, reply.result);
    asked.done(reply.result);
  }

  /** A building's floor measured on the same record, ground and paving; undefined when none was, or on other ground (its digest changed: a stamp under it, a road beside it). */
  floorKnown(entry: CellEntry, paved: boolean): number | null | undefined {
    const known = this.floors.get(entry.key);
    return known && known.sig === entry.sig && known.digest === entry.digest && known.paved === paved ? known.floor : undefined;
  }

  /** DO NOT keep the floor of a building gone from the document (a tile let go as the eye walked on): the floors kept follow what is loaded, not the way walked. `keep`: whether an entry's building is still there, by its key (an id, or a building cut open, `id|...`). */
  forgetFloors(keep: (key: string) => boolean): void {
    this.floors.retain(keep);
  }

  /** Where a job's result is kept between sessions: its kind, its cell and all it is made from (each building's record and the ground under it, `CellEntry`; the paving read or not; the kit's boxes and tones). Null for a kind not kept, or with no fingerprint of the code. */
  private storedKey(kind: JobKind, cell: string, entries: readonly CellEntry[], env: CellEnv): string | null {
    if (!CELL_PREFIX || !CELL_HASH || !KEPT_KINDS.has(kind)) return null;
    if (!forgotOthers) { forgotOthers = true; forgetOtherDerived('cellWork', CELL_HASH); }
    const d = new Digest().add(this.kitDigest()).add(env.pavedAt !== undefined ? 1 : 0).add(env.naturalAt === env.groundAt ? 1 : 0);
    for (const e of entries) d.addText(e.key).add(e.sig).add(e.digest);
    return `${CELL_PREFIX}${kind}|${cell}|${d.value()}`;
  }

  /** The kit's boxes and tones as one number: the boxes hashed once a set (`kitBoxes` gives the same `parts` and `furniture` every call), the
   *  bandings (a few numbers a kind) each call. DO NOT key it by the object `boxes()` returns: it is new every call (its bandings follow the
   *  kit's levels), and the whole kit was hashed again for every cell job of a move (`Digest.addPlain` 333 ms self in the player's probe,
   *  612 ms in `npm run diagnose`, 2026-10-22). */
  private kitDigest(): number {
    const boxes = this.boxes();
    if (this.kitSeen?.parts !== boxes.parts || this.kitSeen.furniture !== boxes.furniture) {
      this.kitSeen = { parts: boxes.parts, furniture: boxes.furniture, digest: new Digest().addPlain(boxes.parts).addPlain(boxes.furniture).addPlain(this.tone ?? null).value() };
    }
    return new Digest().add(this.kitSeen.digest).addPlain(boxes.bandings ?? null).value();
  }
  private kitSeen: { readonly parts: KitBoxes['parts']; readonly furniture: KitBoxes['furniture']; readonly digest: number } | null = null;

  /** The floors a job stood its buildings on, kept for their next jobs. */
  private learn(asked: Asked, result: CellResult): void {
    const floors = result.floors;
    if (!floors) return;
    const paved = asked.env.pavedAt !== undefined;
    asked.entries.forEach((e, i) => {
      const floor = floors[i];
      if (floor !== undefined) this.floors.set(e.key, { sig: e.sig, digest: e.digest, paved, floor });
    });
  }
}

// src/render/buildings/cellPool.ts:200-229 slice of src/render/buildings/cellPool.ts#CellPool.run (shared): the feature's statements; "…" marks code left out
  run(kind: JobKind, cell: string, entries: readonly CellEntry[], env: CellEnv, done: (result: CellResult) => void, key: string | null = null, edited = false, hole = false): number {
    // … 3 lines
    const place = at ? [(at.i0 + at.n / 2) * BATCH_CELL, (at.j0 + at.n / 2) * BATCH_CELL] as const : null;
    // … 4 lines
    const asked: Asked = {
      id: this.nextId++, kind, cell, entries, writers: entries.map((e) => this.text(e)), env, done, key, priority, first, sent: false, place,
    // … 1 lines
    };
  // … 16 lines
  }
