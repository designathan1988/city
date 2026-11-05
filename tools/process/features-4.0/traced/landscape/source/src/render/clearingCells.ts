// The code of src/render/clearingCells.ts that is the feature's (the rest of the file is shared).

// src/render/clearingCells.ts:8-8 slice of src/render/clearingCells.ts#ClearingCells.cells (shared): the feature's statements; "…" marks code left out
  private readonly cells = new Map<number, TreeClearing[]>();

// src/render/clearingCells.ts:10-10 slice of src/render/clearingCells.ts#ClearingCells.filed (shared): the feature's statements; "…" marks code left out
  private filed: TreeClearing[] = [];

// src/render/clearingCells.ts:19-39 slice of src/render/clearingCells.ts#ClearingCells.follow (shared): the feature's statements; "…" marks code left out
  follow(owner: object, list: readonly TreeClearing[]): void {
    if (owner !== this.owner) {
      // … 1 lines
      this.refile(list);
    // … 1 lines
    }
    // … 1 lines
    while (this.head < this.filed.length && this.filed[this.head] !== list[0]) this.unfile(this.filed[this.head++]!);
    const kept = this.filed.length - this.head;
    // … 1 lines
    if (kept > list.length || (kept > 0 && list[kept - 1] !== this.filed[this.filed.length - 1])) {
      this.refile(list);
      return;
    }
    for (let k = kept; k < list.length; k++) this.file(list[k]!);
    // … 1 lines
    if (this.head > 64 && this.head * 2 > this.filed.length) {
      this.filed = this.filed.slice(this.head);
      this.head = 0;
    }
  }

// src/render/clearingCells.ts:42-47 slice of src/render/clearingCells.ts#ClearingCells.has (shared): the feature's statements; "…" marks code left out
  has(x: number, y: number): boolean {
    for (const c of this.cells.get(this.keyOf(Math.floor(x / this.cell), Math.floor(y / this.cell))) ?? []) {
      if (Math.abs(c.x - x) < c.radius && Math.abs(c.y - y) < c.radius && Math.hypot(c.x - x, c.y - y) < c.radius) return true;
    }
  // … 1 lines
  }

// src/render/clearingCells.ts:54-59 slice of src/render/clearingCells.ts#ClearingCells.refile (shared): the feature's statements; "…" marks code left out
  private refile(list: readonly TreeClearing[]): void {
    // … 1 lines
    this.filed = [];
    // … 1 lines
    for (const c of list) this.file(c);
  }

// src/render/clearingCells.ts:61-68 slice of src/render/clearingCells.ts#ClearingCells.file (shared): the feature's statements; "…" marks code left out
  private file(c: TreeClearing): void {
    this.filed.push(c);
    // … 1 lines
    this.over(c, (k) => {
      const list = this.cells.get(k);
      if (list) list.push(c); else this.cells.set(k, [c]);
    });
  }

// src/render/clearingCells.ts:70-79 slice of src/render/clearingCells.ts#ClearingCells.unfile (shared): the feature's statements; "…" marks code left out
  private unfile(c: TreeClearing): void {
    // … 1 lines
    this.over(c, (k) => {
      const list = this.cells.get(k);
      if (!list) return;
      const at = list.indexOf(c);
      if (at >= 0) list.splice(at, 1);
      if (!list.length) this.cells.delete(k);
    });
  }

// src/render/clearingCells.ts:82-86 slice of src/render/clearingCells.ts#ClearingCells.over (shared): the feature's statements; "…" marks code left out
  private over(c: TreeClearing, visit: (key: number) => void): void {
    for (let j = Math.floor((c.y - c.radius) / this.cell); j <= Math.floor((c.y + c.radius) / this.cell); j++) {
      for (let i = Math.floor((c.x - c.radius) / this.cell); i <= Math.floor((c.x + c.radius) / this.cell); i++) visit(this.keyOf(i, j));
    }
  }
