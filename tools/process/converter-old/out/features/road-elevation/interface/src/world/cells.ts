// What the feature uses of src/world/cells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cells.ts:60-255 class CellFiling
/** Things (by id) filed in the cells their bounds reach, each with the object it was filed for */
export class CellFiling<Id extends number, T extends object> { … }

// src/world/cells.ts:76-104 method CellFiling.file
/** Files `id` for `item` over `rect`; nothing happens when it is already filed for that same object. */
  file(id: Id, item: T, rect: ChangeRect): void;

// src/world/cells.ts:107-112 method CellFiling.remove
/** Takes `id` out (nothing when it is not filed). */
  remove(id: Id): void;

// src/world/cells.ts:120-123 method CellFiling.rectOf
/** The rectangle `id` is filed over, or undefined. */
  rectOf(id: Id): ChangeRect | undefined;

// src/world/cells.ts:131-154 method CellFiling.idsMeeting
/** The ids whose bounds meet `rect`, each once, in increasing order */
  idsMeeting(rect: ChangeRect): Id[];

// src/world/cells.ts:169-190 method CellFiling.idsMeetingAny
/** DO NOT ask `idsMeeting` rectangle by rectangle for a change of many */
  idsMeetingAny(rects: readonly ChangeRect[]): Id[];
