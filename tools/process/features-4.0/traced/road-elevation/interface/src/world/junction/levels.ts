// What the feature uses of src/world/junction/levels.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/levels.ts:169-169 function bit
const bit = (level: SurfaceLevel): number => …;

// src/world/junction/levels.ts:185-321 class JunctionLevels
/** A node's junction at each surface level, read as the map of them it stands for (in `SURFACE_LEVELS` order). */
export class JunctionLevels implements ReadonlyMap<SurfaceLevel, Junction> { … }

// src/world/junction/levels.ts:186-202 constructor JunctionLevels.constructor
private constructor(
    /** The carriageway's junction (`Level.Asphalt`), when there is one. */
    private readonly carriageway: Junction | null,
    /** The levels there are (`bit`), and those of them made from the carriageway (`frames`). */
    private readonly present: number,
    private readonly derived: number,
    /** The legs of every derived level as numbers, in their own order (`FRAME` a leg). */
    private readonly frames: readonly number[] | null,
    /** The outer levels built on their own, kept whole, in level order. */
    private readonly kept: readonly Junction[],
    /** The casing's outline, kept when it is a derived level (`ringAt`). */
    private readonly casingRing: Ring | null,
    /** The box of every ring of every level, worked out as solved. */
    readonly bounds: ChangeRect | null,
    /** The solver's functions its levels are made again with; none for the empty one (nothing to make). */
    private readonly kit: JunctionKit | null,
  );

// src/world/junction/levels.ts:269-274 method JunctionLevels.legsAt
/** A level's legs, without making the level (`elevation.ts` reads the casing's widths at every junction). */
  legsAt(level: SurfaceLevel): readonly Leg[];
