// What the feature uses of src/sim/agents/buildingOrder.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/buildingOrder.ts:102-107 interface BuildingDelta
/** What changed among the buildings since a reader last looked. */
export interface BuildingDelta {
  /** The ids the diary names, each once. */
  readonly ids: readonly BuildingId[];
  /** Where they stood and stand. */
  readonly rects: readonly ChangeRect[];
}

// src/sim/agents/buildingOrder.ts:114-252 class BuildingsInOrder
/** The buildings that pass `keep`, in the store's order, kept from the diary. */
export class BuildingsInOrder implements JournalReader { … }
