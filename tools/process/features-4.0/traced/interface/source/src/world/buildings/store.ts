// The code of src/world/buildings/store.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/store.ts:270-272 method BuildingStore.toText: the feature's own
  toText(): string {
    return JSON.stringify([...this.items.values()]);
  }
