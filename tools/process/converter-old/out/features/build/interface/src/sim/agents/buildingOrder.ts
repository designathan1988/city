// What the feature uses of src/sim/agents/buildingOrder.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/buildingOrder.ts:21-23 constructor StoreOrder.constructor
constructor();

// src/sim/agents/buildingOrder.ts:25-28 method StoreOrder.rank
rank(id: BuildingId): number | undefined;

// src/sim/agents/buildingOrder.ts:30-32 method StoreOrder.has
private has(id: BuildingId): boolean;

// src/sim/agents/buildingOrder.ts:34-38 method StoreOrder.place
private place(id: BuildingId): void;

// src/sim/agents/buildingOrder.ts:41-49 method StoreOrder.follow
/** Brought up to the diary's latest entry. */
  follow(w: BuildingSource): void;

// src/sim/agents/buildingOrder.ts:52-90 method StoreOrder.apply
/** One entry of the diary applied to the ranks; false when its effect on the order is not known. */
  private apply(detail: string | undefined, ids: readonly BuildingId[] | undefined): boolean;

// src/sim/agents/buildingOrder.ts:102-107 interface BuildingDelta
/** What changed among the buildings since a reader last looked. */
export interface BuildingDelta {
  /** The ids the diary names, each once. */
  readonly ids: readonly BuildingId[];
  /** Where they stood and stand. */
  readonly rects: readonly ChangeRect[];
}

// src/sim/agents/buildingOrder.ts:111-111 variable MERGE_PAST
/** Past this many named in one update, the list is walked once and merged (`merge`), not spliced a building at a time. */
const MERGE_PAST = 32;

// src/sim/agents/buildingOrder.ts:114-252 class BuildingsInOrder
/** The buildings that pass `keep`, in the store's order, kept from the diary. */
export class BuildingsInOrder implements JournalReader { … }

// src/sim/agents/buildingOrder.ts:125-125 field BuildingsInOrder.filedAt
/** Each kept building and the rank it was filed in the list at. */
  private readonly filedAt = new Map<BuildingId, number>();

// src/sim/agents/buildingOrder.ts:126-126 field BuildingsInOrder.list
private list: BuildingId[] = [];

// src/sim/agents/buildingOrder.ts:133-135 getter BuildingsInOrder.ids
/** The kept buildings' ids, in the store's order. */
  get ids(): readonly BuildingId[];

// src/sim/agents/buildingOrder.ts:137-139 method BuildingsInOrder.has
has(id: BuildingId): boolean;

// src/sim/agents/buildingOrder.ts:142-144 method BuildingsInOrder.rank
/** A building's place in the store's order as of the last `update` (to sort other buildings by it). */
  rank(id: BuildingId): number;

// src/sim/agents/buildingOrder.ts:147-183 method BuildingsInOrder.update
/** Up to date with the store. */
  update(w: BuildingSource): BuildingDelta | null;

// src/sim/agents/buildingOrder.ts:186-210 method BuildingsInOrder.merge
/** The named buildings taken out of the list in one walk, those kept now filed back by one merge of the two orders. */
  private merge(w: BuildingSource, ids: ReadonlySet<BuildingId>): void;

// src/sim/agents/buildingOrder.ts:213-220 method BuildingsInOrder.slot
/** Where a rank stands, or would stand, in the list (the list is in the order of the ranks it was filed at). */
  private slot(rank: number): number;

// src/sim/agents/buildingOrder.ts:222-225 method BuildingsInOrder.file
private file(id: BuildingId, rank: number): void;

// src/sim/agents/buildingOrder.ts:227-232 method BuildingsInOrder.unfile
private unfile(id: BuildingId): void;

// src/sim/agents/buildingOrder.ts:235-240 method BuildingsInOrder.resort
/** @rebuild The kept buildings in the store's order read again whole: its ranks were (`StoreOrder.reread`). */
  private resort(): void;

// src/sim/agents/buildingOrder.ts:243-251 method BuildingsInOrder.reread
/** @rebuild The kept buildings read whole from the store. */
  private reread(w: BuildingSource): void;
