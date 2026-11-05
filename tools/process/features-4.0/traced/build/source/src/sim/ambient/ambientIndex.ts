// The code of src/sim/ambient/ambientIndex.ts that is the feature's (the rest of the file is shared).

// src/sim/ambient/ambientIndex.ts:71-79 slice of src/sim/ambient/ambientIndex.ts#SightWalls.sync (shared): the feature's statements; "…" marks code left out
  sync(store: TrafficBuildings, journal: ChangeJournal): void {
    // … 3 lines
    for (const id of changed) {
      // … 1 lines
      this.file(id, store.footprints(id as BuildingId));
    }
  }
