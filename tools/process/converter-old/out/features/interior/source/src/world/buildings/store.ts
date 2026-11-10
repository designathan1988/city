// The code of src/world/buildings/store.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/store.ts:69-71 getter BuildingStore.wearRevision: the feature's own
  get wearRevision(): number {
    return this.journal.serialOf('buildingWear');
  }

// src/world/buildings/store.ts:104-107 method BuildingStore.tableRef: the feature's own
  tableRef(id: BuildingId): RecordRef | null {
    if (this.items instanceof BuildingItems) return this.items.tableRef(id);
    return null;
  }
