// The code of src/world/cells.ts that is the feature's (the rest of the file is shared).

// src/world/cells.ts:222-228 method CellFiling.takeBack: the feature's own
  takeBack(fork: CellFiling<Id, T>): void {
    if (!(fork.entries instanceof ForkMap)) throw new Error('CellFiling.takeBack: not a working copy');
    fork.entries.applyTo(this.entries);
    (fork.byCell as ForkMap<number, Id[]>).applyTo(this.byCell);
    (fork.versions as ForkMap<number, number>).applyTo(this.versions);
    this.epoch = fork.epoch;
  }
