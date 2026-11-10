// The code of src/editor/preparedCity.ts that is the feature's (the rest of the file is shared).

// src/editor/preparedCity.ts:21-31 slice of src/editor/preparedCity.ts#prepareCity (shared): the feature's statements; "…" marks code left out
export function prepareCity(data: SerializedDoc): PreparedCity {
  const shared = new SharedMembers();
  const writer = new PreparedCityWriter(shared);
  const all = (data.buildings ?? []) as readonly Building[];
  for (let i = 0; i < all.length; i += PREPARE_BATCH) {
    writer.add(all.slice(i, i + PREPARE_BATCH));
    shared.end();
    writer.endBatch();
  }
  return { data: { ...data, buildings: [] }, buildings: writer.finish() };
}

// src/editor/preparedCity.ts:39-39 slice of src/editor/preparedCity.ts#PreparedCityWriter.table (shared): the feature's statements; "…" marks code left out
  private readonly table = new TableWriter(true);

// src/editor/preparedCity.ts:43-43 slice of src/editor/preparedCity.ts#PreparedCityWriter.constructor (shared): the feature's statements; "…" marks code left out
  constructor(private readonly shared: SharedMembers) {}

// src/editor/preparedCity.ts:46-48 slice of src/editor/preparedCity.ts#PreparedCityWriter.endBatch (shared): the feature's statements; "…" marks code left out
  endBatch(): void {
    this.table.keepOnly((node) => this.shared.keeps(node));
  }

// src/editor/preparedCity.ts:51-62 slice of src/editor/preparedCity.ts#PreparedCityWriter.add (shared): the feature's statements; "…" marks code left out
  add(batch: readonly Building[]): void {
    const settled = new WeakSet<object>(), rows: Building[] = [];
    for (const raw of batch) {
      const b = migrateBuilding(raw);
      if (!b || this.ids.has(b.id)) continue;
      this.ids.add(b.id);
      rows.push(settleBuilding(b, settled, undefined, false));
    }
    // … 1 lines
    this.shared.share(rows);
    this.table.add(rows);
  }

// src/editor/preparedCity.ts:65-67 slice of src/editor/preparedCity.ts#PreparedCityWriter.finish (shared): the feature's statements; "…" marks code left out
  finish(): ArrayBufferLike {
    return this.table.finish().buffer;
  }

// src/editor/preparedCity.ts:82-84 slice of src/editor/preparedCity.ts#openedBuildings (shared): the feature's statements; "…" marks code left out
export function openedBuildings(city: OpenedCity): number {
  return (city.data.buildings?.length ?? 0) + (city.table ? new RecordTable(city.table).rows : 0);
}
