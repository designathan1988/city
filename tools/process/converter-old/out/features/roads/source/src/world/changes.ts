// The code of src/world/changes.ts that is the feature's (the rest of the file is shared).

// src/world/changes.ts:355-364 method ChangeJournal.idsSince: the feature's own
  idsSince(since: number, kinds: readonly ChangeKind[]): Set<number> | null {
    const entries = this.since(since, kinds);
    if (!entries) return null;
    const ids = new Set<number>();
    for (const entry of entries) {
      if (!entry.ids) return null;
      for (const id of entry.ids) ids.add(id);
    }
    return ids;
  }
