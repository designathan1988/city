// What the feature uses of src/render/clearingCells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/clearingCells.ts:8-8 field ClearingCells.cells
private readonly cells = new Map<number, TreeClearing[]>();

// src/render/clearingCells.ts:10-10 field ClearingCells.filed
/** The clearings filed, in the document's order, from `head`. */
  private filed: TreeClearing[] = [];

// src/render/clearingCells.ts:19-39 method ClearingCells.follow
/** The filing brought up to `list`, the clearings of `owner` (a document), oldest first. */
  follow(owner: object, list: readonly TreeClearing[]): void;

// src/render/clearingCells.ts:42-47 method ClearingCells.has
/** Whether a point lies in a clearing. */
  has(x: number, y: number): boolean;

// src/render/clearingCells.ts:54-59 method ClearingCells.refile
/** @rebuild Every clearing of a list filed: a document opened, or its clearings replaced. */
  private refile(list: readonly TreeClearing[]): void;

// src/render/clearingCells.ts:61-68 method ClearingCells.file
private file(c: TreeClearing): void;

// src/render/clearingCells.ts:70-79 method ClearingCells.unfile
private unfile(c: TreeClearing): void;

// src/render/clearingCells.ts:82-86 method ClearingCells.over
/** Each cell a clearing's square reaches. */
  private over(c: TreeClearing, visit: (key: number) => void): void;
