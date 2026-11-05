// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:1953-1958 slice of src/world/doc.ts#RoadDoc.nodeToWrite (shared): the feature's statements; "…" marks code left out
  private nodeToWrite(id: NodeId): RoadNode | undefined {
    // … 2 lines
    if (this.recorder) this.note('nodes', id, map.get(id));
  // … 1 lines
  }

// src/world/doc.ts:1961-1966 slice of src/world/doc.ts#RoadDoc.segmentToWrite (shared): the feature's statements; "…" marks code left out
  private segmentToWrite(id: SegmentId): RoadSegment | undefined {
    // … 2 lines
    if (this.recorder) this.note('segments', id, map.get(id));
  // … 1 lines
  }

// src/world/doc.ts:2060-2070 slice of src/world/doc.ts#RoadDoc.replaceWhole (shared): the feature's statements; "…" marks code left out
  replaceWhole(source: RoadDoc): void {
    // … 1 lines
    if (this.recorder && !this.recorder.whole) this.recorder.whole = this.image();
  // … 7 lines
  }

// src/world/doc.ts:2073-2104 slice of src/world/doc.ts#RoadDoc.replaceWith (shared): the feature's statements; "…" marks code left out
  replaceWith(source: RoadDoc): void {
    // … 7 lines
    if (this.recorder && !this.recorder.whole) this.recorder.whole = this.image();
  // … 22 lines
  }

// src/world/doc.ts:2133-2138 method RoadDoc.beginEdit: the feature's own
  beginEdit(): void {
    const recorder: EditRecorder = { records: new Map(), lists: new Map(), values: new Map(), whole: null };
    for (const part of VALUE_PARTS) recorder.values.set(part, this.valueOf(part));
    this.recorder = recorder;
    this.listen(recorder);
  }

// src/world/doc.ts:2141-2181 method RoadDoc.endEdit: the feature's own
  endEdit(): DocPatch | null {
    const rec = this.recorder;
    if (!rec) return null;
    this.recorder = null;
    this.listen(null);
    // The map after is the map as it stands: taken only when the step is undone (`editor/history.ts`), never kept twice.
    if (rec.whole) {
      return { records: [], lists: [], values: [], whole: { before: rec.whole, after: null }, bytes: wholeBytes(rec.whole, null) };
    }
    const records: DocPatch['records'][number][] = [];
    for (const [part, kept] of rec.records) {
      for (const [id, before] of kept) {
        const after = copyRecord(part, this.recordOf(part, id));
        if (before === after || (before !== undefined && after !== undefined && samePlain(before, after))) continue;
        // A building changed, not made or removed: only its members that changed, on both sides (`MemberDiff`) - DO NOT keep its whole record before and after (1,000 edits of one building held 1,000 records).
        if (part === 'buildings' && before !== undefined && after !== undefined) {
          const [was, now] = memberDiffs(before as Record<string, unknown>, after as Record<string, unknown>);
          records.push({ part, id, before: was, after: now });
          continue;
        }
        records.push({ part, id, before, after });
      }
    }
    const lists: DocPatch['lists'][number][] = [];
    for (const [part, was] of rec.lists) {
      const now = this.listOf(part);
      let start = 0;
      while (start < was.length && start < now.length && samePlain(was[start], now[start])) start++;
      let end = 0;
      while (end < was.length - start && end < now.length - start && samePlain(was[was.length - 1 - end], now[now.length - 1 - end])) end++;
      if (start === was.length && start === now.length) continue;
      lists.push({ part, start, before: was.slice(start, was.length - end).map(copyItem), after: now.slice(start, now.length - end).map(copyItem) });
    }
    const values: DocPatch['values'][number][] = [];
    for (const [part, before] of rec.values) {
      const now = this.valueOf(part);
      if (!samePlain(before, now)) values.push({ part, before, after: copyItem(now) });
    }
    if (!records.length && !lists.length && !values.length) return null;
    return { records, lists, values, bytes: patchBytes(records, lists, values) };
  }

// src/world/doc.ts:2184-2191 slice of src/world/doc.ts#RoadDoc.note (shared): the feature's statements; "…" marks code left out
  private note(part: RecordPart, id: number, was: unknown): void {
    const rec = this.recorder!;
    // … 1 lines
    if (rec.whole) return;
    let kept = rec.records.get(part);
    if (!kept) rec.records.set(part, kept = new Map());
    if (!kept.has(id)) kept.set(id, copyRecord(part, was));
  }

// src/world/doc.ts:2194-2197 slice of src/world/doc.ts#RoadDoc.noteList (shared): the feature's statements; "…" marks code left out
  private noteList(part: ListPart): void {
    const rec = this.recorder;
    if (rec && !rec.whole && !rec.lists.has(part)) rec.lists.set(part, this.listOf(part).slice());
  }

// src/world/doc.ts:2215-2226 method RoadDoc.recordOf: the feature's own
  private recordOf(part: RecordPart, id: number): unknown {
    switch (part) {
      case 'nodes': return this.nodes.get(id as NodeId);
      case 'segments': return this.segments.get(id as SegmentId);
      case 'poles': return this.poleMap.get(id as PoleId);
      case 'poleSpans': return this.spanMap.get(id as SpanId);
      case 'landscape': return this.landscapeMap.get(id);
      case 'barriers': return this.barrierMap.get(id);
      case 'buildings': return this.buildings.get(id as BuildingId);
      case 'lots': return this.lotStore.get(id);
    }
  }

// src/world/doc.ts:2581-2583 method RoadDoc.fromBinary: the feature's own
  static fromBinary(bytes: ArrayBufferLike): RoadDoc {
    return RoadDoc.fromImage(unpackImage(bytes, (b) => { const out = allocate(b.byteLength); new Uint8Array(out).set(b); return out; }));
  }

// src/world/doc.ts:2586-2596 method RoadDoc.toText: the feature's own
  toText(): string {
    if (this.buildings.size === 0) return JSON.stringify(this.serialized(false));
    // The keys in `toJSON`'s order: those before the buildings, the buildings, those after.
    const head: Record<string, unknown> = {};
    const tail: Record<string, unknown> = {};
    for (const [key, value] of Object.entries(this.serialized(false))) {
      (AFTER_BUILDINGS.has(key) ? tail : head)[key] = value;
    }
    const after = JSON.stringify(tail);
    return `${JSON.stringify(head).slice(0, -1)},"buildings":${this.buildings.toText()}${after === '{}' ? '' : `,${after.slice(1, -1)}`}}`;
  }

// src/world/doc.ts:2599-2616 method RoadDoc.savedGlobals: the feature's own
  savedGlobals(part: GlobalPart): Partial<SavedGlobals> {
    switch (part) {
      case 'values': return {
        version: 1,
        unit: METERS_PER_UNIT,
        ...(this.terrainRelief !== RELIEF_LEGACY ? { relief: this.terrainRelief } : {}),
        ...(this.markingStyle !== 'classic' ? { markingStyle: this.markingStyle } : {}),
        ...(this.nature ? { nature: { ...this.nature } } : {}),
        ...(!samePlain(this.weather, DEFAULT_WEATHER) ? { weather: { ...this.weather } } : {}),
        fogSettings: { ...this.fogSettings },
        gullyAuto: this.gullyAuto,
        ...(this.economy.balance !== DEFAULT_ECONOMY.balance ? { economy: { balance: this.economy.balance } } : {}),
      };
      case 'lotKeys': return this.lotStore.keys.length > 0 ? { lotKeys: [...this.lotStore.keys] } : {};
      case 'people': return this.people.length > 0 ? { people: this.people.map((p) => structuredClone(p)) } : {};
      case 'transit': return hasTransit(this.transit) ? { transit: structuredClone(this.transit) } : {};
    }
  }

// src/world/doc.ts:3144-3152 interface DocPatch: the feature's own
export interface DocPatch {
  readonly records: readonly { readonly part: RecordPart; readonly id: number; readonly before: unknown; readonly after: unknown }[];
  readonly lists: readonly { readonly part: ListPart; readonly start: number; readonly before: readonly unknown[]; readonly after: readonly unknown[] }[];
  readonly values: readonly { readonly part: ValuePart; readonly before: unknown; readonly after: unknown }[];
  /** The whole map before and after as its image (`RoadDoc.image`), when the edit replaced it whole (a map opened, a city generated). */
  readonly whole?: { readonly before: DocImage; readonly after: DocImage | null };
  /** Roughly what it holds, UTF-16 bytes of its text. */
  readonly bytes: number;
}

// src/world/doc.ts:3154-3154 function textBytes: the feature's own
const textBytes = (value: unknown): number => (value === undefined ? 0 : 2 * JSON.stringify(value).length);

// src/world/doc.ts:3157-3157 function wholeBytes: the feature's own
export const wholeBytes = (before: DocImage, after: DocImage | null): number => imageBytes(before) + (after ? imageBytes(after) : 0);

// src/world/doc.ts:3160-3167 function patchBytes: the feature's own
function patchBytes(records: DocPatch['records'], lists: DocPatch['lists'], values: DocPatch['values']): number {
  let bytes = 0;
  // A building's or a lot's record after is the document's own (by reference); a building's member diff is the step's own.
  for (const r of records) bytes += textBytes(r.before) + ((r.part === 'buildings' && !(r.after instanceof MemberDiff)) || r.part === 'lots' ? 0 : textBytes(r.after));
  for (const l of lists) bytes += textBytes(l.before) + textBytes(l.after);
  for (const v of values) bytes += textBytes(v.before) + textBytes(v.after);
  return bytes;
}

// src/world/doc.ts:3581-3588 method MemberDiff.keyOrder: the feature's own
  static keyOrder(record: Record<string, unknown>): readonly string[] {
    const keys = Object.keys(record);
    const name = keys.join('\u0000');
    const known = MemberDiff.orders.get(name);
    if (known) return known;
    if (MemberDiff.orders.size < MemberDiff.ORDERS_KEPT) MemberDiff.orders.set(name, keys);
    return keys;
  }

// src/world/doc.ts:3605-3614 function memberDiffs: the feature's own
function memberDiffs(before: Record<string, unknown>, after: Record<string, unknown>): [MemberDiff, MemberDiff] {
  const was: Record<string, unknown> = {}, now: Record<string, unknown> = {};
  for (const key of Object.keys(before)) {
    if (Object.hasOwn(after, key) && samePlain(before[key], after[key])) continue;
    was[key] = before[key];
    if (Object.hasOwn(after, key)) now[key] = after[key];
  }
  for (const key of Object.keys(after)) if (!Object.hasOwn(before, key)) now[key] = after[key];
  return [new MemberDiff(MemberDiff.keyOrder(before), was), new MemberDiff(MemberDiff.keyOrder(after), now)];
}

// src/world/doc.ts:3627-3627 function copyItem: the feature's own
const copyItem = <T>(value: T): T => (value === undefined ? value : structuredClone(value));

// src/world/doc.ts:3631-3631 type GlobalPart: the feature's own
export type GlobalPart = (typeof GLOBAL_PARTS)[number];

// src/world/doc.ts:3634-3647 interface SavedGlobals: the feature's own
export interface SavedGlobals {
  readonly version: 1;
  readonly unit?: number;
  readonly relief?: number;
  readonly markingStyle?: string;
  readonly nature?: { readonly region: string; readonly seed: number };
  readonly weather?: unknown;
  readonly fogSettings?: unknown;
  readonly gullyAuto?: number;
  readonly lotKeys?: readonly string[];
  readonly people?: readonly unknown[];
  readonly transit?: unknown;
  readonly economy?: { readonly balance: number };
}
