// What the feature uses of src/world/poleIndex.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/poleIndex.ts:25-120 class PoleIndex
export class PoleIndex { … }

// src/world/poleIndex.ts:52-61 method PoleIndex.putPole
/** A pole put (new, or its record replaced where it may have moved). */
  putPole(pole: UtilityPole): void;

// src/world/poleIndex.ts:63-68 method PoleIndex.deletePole
deletePole(id: PoleId): void;

// src/world/poleIndex.ts:77-83 method PoleIndex.putSpan
putSpan(span: UtilitySpan): void;

// src/world/poleIndex.ts:85-92 method PoleIndex.deleteSpan
deleteSpan(span: UtilitySpan): void;

// src/world/poleIndex.ts:95-97 method PoleIndex.spansAt
/** The spans reaching a pole, in the order they were strung. */
  spansAt(id: PoleId): Iterable<SpanId>;

// src/world/poleIndex.ts:100-119 method PoleIndex.near
/** The ids of the poles in the cells the rectangle reaches: a superset, the caller keeps its own exact test. */
  near(rect: ChangeRect): PoleId[];
