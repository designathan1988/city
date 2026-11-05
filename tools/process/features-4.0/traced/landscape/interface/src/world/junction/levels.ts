// What the feature uses of src/world/junction/levels.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junction/levels.ts:124-124 type RingRun
/** A ring as its run of numbers (`Ring.run`): what a message's clone keeps of it (a class's methods are not cloned, `structuredClone`). */
export type RingRun = ReturnType<Ring['run']>;

// src/world/junction/levels.ts:128-139 interface JunctionData
/** A junction as runs of numbers, for another thread (`NetworkBake`): its legs and corners packed (`packLegs`, `packCorners`), its rings as runs. */
export interface JunctionData {
  readonly nodeId: NodeId;
  readonly level: SurfaceLevel;
  readonly legs: readonly number[];
  readonly corners: readonly number[];
  readonly trims: readonly number[];
  readonly ring: RingRun;
  readonly usedHullFallback: boolean;
  readonly transition: boolean;
  /** Its tongues and rings when they are not the ones `fill` makes. */
  readonly kept: { readonly tongues: readonly RingRun[]; readonly rings: readonly RingRun[] } | null;
}

// src/world/junction/levels.ts:157-167 interface JunctionLevelsData
/** A node's levels as plain data (`JunctionLevels.data`, `fromData`). */
export interface JunctionLevelsData {
  readonly carriageway: JunctionData | null;
  readonly present: number;
  readonly derived: number;
  readonly frames: readonly number[] | null;
  readonly kept: readonly JunctionData[];
  readonly casingRing: RingRun | null;
  /** The legs' road profiles (`packLegs`). */
  readonly roads: readonly RoadType[];
  readonly bounds: ChangeRect | null;
}

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
