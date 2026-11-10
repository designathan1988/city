// The code of src/world/buildings/items.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/items.ts:62-67 method BuildingItems.tableRef: the feature's own
  tableRef(key: BuildingId): RecordRef | null {
    const base = this.base;
    if (!base || this.owns(key) || this.gone.has(key)) return null;
    const row = base.rowOf(key);
    return row < 0 ? null : { buffer: base.buffer, row };
  }
