// The code of src/editor/blueprintLibrary.ts that is the feature's (the rest of the file is shared).

// src/editor/blueprintLibrary.ts:21-49 class BlueprintLibrary: the feature's own
export class BlueprintLibrary {
  private readonly kept: KeptList;

  constructor(storageKey = KEY) {
    this.kept = new KeptList(storageKey, 'blueprint');
  }

  list(): Blueprint[] {
    return this.kept.read(readEntry);
  }

  /** Saves a body under a name (the one of that name replaced); returns the stored blueprint, or null when it was not saved. */
  save(name: string, body: BlueprintBody): Blueprint | null {
    const title = name.trim().slice(0, 40) || 'Blueprint';
    let key = '';
    const written = this.kept.update((entries) => {
      const existing = entries.find((e): e is StoredBlueprint => isStored(e) && e.name.toLocaleLowerCase() === title.toLocaleLowerCase());
      key = existing?.key ?? `user-${Date.now().toString(36)}-${entries.length}`;
      const entry: StoredBlueprint = { key, name: title, body, unit: METERS_PER_UNIT };
      return [entry, ...entries.filter((e) => !(isStored(e) && e.key === key))];
    });
    return written ? { key, name: title, body } : null;
  }

  /** Removes one blueprint, the rest as they were; false when nothing could be written. */
  remove(key: string): boolean {
    return this.kept.update((entries) => entries.filter((e) => !(isStored(e) && e.key === key)));
  }
}
