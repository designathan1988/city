// What the feature uses of src/world/cells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cells.ts:9-9 variable CELL
/** THE WORLD'S CELLS: one fixed grid over the plane the map is laid on, the partition an edit's cost is measured in */

/** The side of a cell, world units (Unreal's default runtime grid cell). */
export const CELL = m(256);

// src/world/cells.ts:17-20 function cellRect
/** The rectangle a cell covers. */
export function cellRect(key: number): ChangeRect;

// src/world/cells.ts:23-23 function cellAt
/** The key of the cell holding a point. */
export const cellAt = (x: number, y: number): number => …;

// src/world/cells.ts:33-33 function grow
/** A rectangle grown by `pad` on every side. */
export const grow = (r: ChangeRect, pad: number): ChangeRect => …;

// src/world/cells.ts:43-47 interface Entry
/** What a filing keeps of a thing: the object it was filed for and one run of numbers, its rectangle then the keys of its cells (`RECT`). */
interface Entry<T> {
  readonly item: T;
  /** DO NOT keep the rectangle and the cells as two lists (and a set a cell: ~440 B a thing filed, 2026-10-21): [x0, y0, x1, y1, cell...], laid at its size. */
  readonly data: readonly number[];
}

// src/world/cells.ts:60-255 class CellFiling
/** Things (by id) filed in the cells their bounds reach, each with the object it was filed for */
export class CellFiling<Id extends number, T extends object> { … }

// src/world/cells.ts:64-64 field CellFiling.entries
private entries: Map<Id, Entry<T>> = new Map();

// src/world/cells.ts:66-66 field CellFiling.byCell
/** Each cell's ids in the order they were filed there (a list, not a set: a few ids a cell, a set's table ~100 B). */
  private byCell: Map<number, Id[]> = new Map();

// src/world/cells.ts:67-67 field CellFiling.versions
private versions: Map<number, number> = new Map();

// src/world/cells.ts:69-69 field CellFiling.epoch
/** Moves whenever any cell's version does: the clock's reading at the latest change. */
  epoch = 0;

// src/world/cells.ts:76-104 method CellFiling.file
/** Files `id` for `item` over `rect`; nothing happens when it is already filed for that same object. */
  file(id: Id, item: T, rect: ChangeRect): void;

// src/world/cells.ts:107-112 method CellFiling.remove
/** Takes `id` out (nothing when it is not filed). */
  remove(id: Id): void;

// src/world/cells.ts:120-123 method CellFiling.rectOf
/** The rectangle `id` is filed over, or undefined. */
  rectOf(id: Id): ChangeRect | undefined;

// src/world/cells.ts:126-128 method CellFiling.ids
/** The ids filed (any order). */
  ids(): IterableIterator<Id>;

// src/world/cells.ts:131-154 method CellFiling.idsMeeting
/** The ids whose bounds meet `rect`, each once, in increasing order */
  idsMeeting(rect: ChangeRect): Id[];

// src/world/cells.ts:157-166 method CellFiling.someAt
/** Whether `test` holds for some id whose bounds hold the point (x, y): its one cell looked at, nothing made (no rectangle, set nor list a point, as `idsMeeting` makes: asked for every vertex of a road surface). */
  someAt(x: number, y: number, test: (id: Id, item: T) => boolean): boolean;

// src/world/cells.ts:169-190 method CellFiling.idsMeetingAny
/** DO NOT ask `idsMeeting` rectangle by rectangle for a change of many */
  idsMeetingAny(rects: readonly ChangeRect[]): Id[];

// src/world/cells.ts:193-197 method CellFiling.stamp
/** The versions of the cells `rect` reaches, as one string: equal strings, nothing filed there moved. */
  stamp(rect: ChangeRect): string;

// src/world/cells.ts:200-209 method CellFiling.copyFrom
/** A copy filing the same things (`Network.adopt`). */
  copyFrom(other: CellFiling<Id, T>): void;

// src/world/cells.ts:212-219 method CellFiling.over
/** DO NOT copy every cell for an edit (`copyFrom` twice per road drawn): a working copy over `base` that writes only what it files */
  static over<Id extends number, T extends object>(base: CellFiling<Id, T>): CellFiling<Id, T>;
