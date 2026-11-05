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

// src/world/junction/levels.ts:185-321 class JunctionLevels
/** A node's junction at each surface level, read as the map of them it stands for (in `SURFACE_LEVELS` order). */
export class JunctionLevels implements ReadonlyMap<SurfaceLevel, Junction> { … }

// src/world/junction/levels.ts:256-261 method JunctionLevels.get
get(level: SurfaceLevel): Junction | undefined;
