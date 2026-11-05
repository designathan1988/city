// What the feature uses of src/world/conflictTable.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/conflictTable.ts:15-15 type BodyClass
/** WHERE TWO MOVEMENTS' BODIES MEET, measured from their sampled sweeps alone */

/** Size classes a conflict zone is measured for. */
export type BodyClass = 0 | 1 | 2;

// src/world/conflictTable.ts:18-18 type TurnChoice
/** A turn as `turnPaths.ts` chose it: one cubic's handle, or two cubics through a point inside the junction. */
export type TurnChoice = number | { x: number; y: number; share: number };

// src/world/conflictTable.ts:21-24 interface ChosenTurn
/** What the search for a turn answers (`turnPaths.ts` `chooseTurn`): the choice and the largest body it fits. */
export interface ChosenTurn {
  readonly choice: TurnChoice;
  readonly maxBodyClass: BodyClass | -1;
}

// src/world/conflictTable.ts:27-36 interface TurnAsk
/** What a turn is chosen from, as plain numbers, for a worker (`conflictPool.ts`) */
export interface TurnAsk {
  /** The lanes' own lists (`Polyline.xy`, never changed once made): not copied here, once by the message. */
  readonly inXY: Float64Array;
  readonly outXY: Float64Array;
  readonly waiting: readonly Float64Array[];
  /** The junction's surface, kept once a key in the plan (`TurnPlan.surfaces`). */
  readonly surfaceKey: string;
  readonly fromRoadEnd: boolean;
  readonly toRoadEnd: boolean;
}

// src/world/conflictTable.ts:39-42 interface SurfaceRings
/** A junction's surface as a worker makes it again: its centre and its rings as flat x, y lists. */
export interface SurfaceRings {
  readonly centre: { readonly x: number; readonly y: number };
  readonly rings: readonly Float64Array[];
}

// src/world/conflictTable.ts:45-49 variable BODY_ENVELOPE
export const BODY_ENVELOPE: readonly { readonly length: number; readonly width: number }[] = [
  { length: m(2.2), width: m(0.9) },
  { length: m(5.7), width: m(2.05) },
  { length: m(12.1), width: m(2.6) },
];

// src/world/conflictTable.ts:50-50 variable HEAVY
export const HEAVY: BodyClass = 2;

// src/world/conflictTable.ts:71-77 interface SweepAsk
/** What one movement's sweep is sampled from, as plain numbers */
export interface SweepAsk {
  readonly inXY: Float64Array;
  readonly crossXY: Float64Array;
  readonly outXY: Float64Array;
  readonly c0: number;
  readonly count: number;
}

// src/world/conflictTable.ts:80-88 interface SampledSweep
/** A movement's sweep sampled (`sampleSweep`): its frame, and the digests the page keys it by. */
export interface SampledSweep {
  /** Per sample: centre x, y and unit tangent x, y (`SweepSamples.frame`). */
  readonly frame: Float64Array;
  readonly frameLengthSq: Float64Array;
  /** `lineDigest` of the approach, the movement and the exit. */
  readonly lines: readonly [number, number, number];
  /** `shapeDigest` of the frame and the movement's centreline. */
  readonly shape: number;
}

// src/world/conflictTable.ts:167-171 interface Crossing
/** Where two movements' centrelines first cross, with the arc position on each. */
export interface Crossing {
  readonly sA: number;
  readonly sB: number;
  readonly at: Vec2;
}

// src/world/conflictTable.ts:198-202 interface FinishAsk
/** A pair to finish somewhere else (`PairMeasurer.finishPairs`): its two centrelines' own point lists (`Polyline.xy`) and its table (`pairTable`). */
export interface FinishAsk {
  readonly a: Float64Array;
  readonly b: Float64Array;
  readonly table: Float64Array;
}

// src/world/conflictTable.ts:205-208 interface FinishedPair
/** A pair finished (`finishPair`): where its centrelines first cross, and what the index makes of its zones. */
export interface FinishedPair {
  readonly crossing: Crossing | null;
  readonly facts: PairFacts;
}

// src/world/conflictTable.ts:219-235 interface SweepSamples
/** What `pairTable` reads of a movement's sweep (`conflictPoints.ts` `Sweep`), and the caches it keeps on it. */
export interface SweepSamples {
  /** First sampled centre arc position (negative: on the approach). */
  readonly c0: number;
  readonly count: number;
  /** The movement's own length (its crossing lanelet's): where a body's rear has left the box. */
  readonly length: number;
  /** Per sample: centre x, y and unit tangent x, y. */
  readonly frame: Float64Array;
  /** Squared tangent length per sample, shared by every pair using this sweep. */
  readonly frameLengthSq: Float64Array;
  /** A conservative bound around every sampled heavy-vehicle body. */
  bounds: { minX: number; minY: number; maxX: number; maxY: number } | null;
  /** Broad phase of the HEAVY rectangles (`SweepGrid`), made once a sweep. */
  grid: SweepGrid | null;
  /** Body classes that may stand at each sampled centre, computed once. */
  range: Uint8Array | null;
}

// src/world/conflictTable.ts:253-260 interface SweepGrid
/** A sweep's HEAVY rectangles filed by cell over the sweep's own box, as typed arrays (CSR: each cell's samples at `items[start[c]..start[c + 1])`, in sample order): DO NOT go back to a map of a list a cell, an array per cell and per sweep the collector had to take back (the allocation profile, 2026-10-17: `pairTable` 2.8-4.7 GB in the traffic's workers). */
export interface SweepGrid {
  readonly gx0: number;
  readonly gy0: number;
  readonly w: number;
  readonly h: number;
  readonly start: Uint32Array;
  readonly items: Uint32Array;
}

// src/world/conflictTable.ts:447-447 type TurnKind
/** DO NOT define them in `junctionBuild.ts` */

/** How a movement turns (`junctionBuild.ts` `classifyTurn`). */
export type TurnKind = 'through' | 'left' | 'right' | 'uturn';

// src/world/conflictTable.ts:450-450 type JunctionNode
/** What a junction reads of its node. */
export type JunctionNode = Pick<RoadNode, 'id' | 'x' | 'y' | 'incident' | 'control' | 'crossing' | 'blockedMovements' | 'laneLinks' | 'uturn' | 'end'>;

// src/world/conflictTable.ts:453-457 interface JunctionLeg
/** A road meeting the junction: what the junction reads of it, and its two ends (its centreline, `segmentLine`). */
export interface JunctionLeg {
  readonly segment: Pick<RoadSegment, 'id' | 'a' | 'b' | 'curve' | 'type' | 'lanes' | 'direction' | 'section' | 'parking'>;
  readonly from: Vec2;
  readonly to: Vec2;
}

// src/world/conflictTable.ts:460-473 interface LaneAsk
/** A link lane arriving at or leaving a junction (`LaneletGraph.buildSegmentLinks`), as a worker is sent it: its centre as its own point list (`Polyline.xy`). */
export interface LaneAsk {
  readonly id: string;
  /** The key it was built under: the junction's own key is made of its lanes' (`junctionBuild.ts` `junctionKey`). */
  readonly key: string;
  readonly segment: SegmentId;
  readonly from: NodeId;
  readonly to: NodeId;
  readonly laneIndex: number;
  readonly speedLimit: number;
  readonly centre: Float64Array;
  /** Whether its `from`, and its `to`, is a road's end (`RoadDoc.mapEdge`), where traffic comes into the map or leaves it. */
  readonly fromEdge: boolean;
  readonly toEdge: boolean;
}

// src/world/conflictTable.ts:476-483 interface JunctionAsk
/** One junction of a whole build, as a worker builds it (`buildJunctionOf`) */
export interface JunctionAsk {
  readonly node: JunctionNode;
  readonly legs: readonly JunctionLeg[];
  readonly inbound: readonly LaneAsk[];
  readonly outbound: readonly LaneAsk[];
  readonly surface: { readonly centre: Vec2; readonly parts: readonly (Float64Array | { readonly full: Float64Array; readonly half: number; readonly mouth: number })[] };
  readonly before: readonly (readonly [string, ChosenTurn])[] | null;
}

// src/world/conflictTable.ts:486-495 interface JunctionAnswer
/** A junction a worker built (`buildJunctionOf`): its key, groups and control, each movement with its path's points, the turns it kept. */
export interface JunctionAnswer {
  readonly key: string;
  readonly groups: readonly ApproachGroup[];
  readonly signalised: boolean;
  readonly connectors: readonly {
    readonly from: number; readonly to: number; readonly turn: TurnKind; readonly carried: boolean; readonly group: number;
    readonly speedLimit: number; readonly maxBodyClass: BodyClass | -1; readonly path: Float64Array;
  }[];
  readonly turns: readonly (readonly [string, ChosenTurn])[];
}

// src/world/conflictTable.ts:498-500 interface JunctionMeasurer
/** Builds a whole build's junctions somewhere else - the browser's workers (`conflictPool.ts`) */
export interface JunctionMeasurer {
  buildJunctions?(asks: readonly JunctionAsk[]): Promise<readonly JunctionAnswer[]>;
}

// src/world/conflictTable.ts:503-514 interface PairMeasurer
/** Measures many pairs' tables somewhere else - the browser's workers (`conflictPool.ts`) */
export interface PairMeasurer extends CornerMeasurer, JunctionMeasurer {
  /** `key`: the pair's key, under which a measurer that `keeps` stores its table for the next session. */
  measure(pairs: readonly { readonly a: SweepSamples; readonly b: SweepSamples; readonly key?: string }[]): Promise<readonly (Float64Array | null)[]>;
  /** DO NOT keep them from the page: the structured clone of a city's tables in one write held a frame 0.9 s (2026-10-15). */
  readonly keeps?: boolean;
  /** Chooses many junction turns somewhere else, each as `turnPaths.ts` `chooseTurnOf` would here */
  chooseTurns?(asks: readonly TurnAsk[], surfaces: ReadonlyMap<string, SurfaceRings>): Promise<readonly ChosenTurn[]>;
  /** Samples many movements' sweeps somewhere else, each as `sampleSweep` would here, answered in the order asked */
  sampleSweeps?(asks: readonly SweepAsk[]): Promise<readonly SampledSweep[]>;
  /** Finishes many pairs somewhere else, each as `finishPair` would here - where the two movements' centrelines first cross */
  finishPairs?(pairs: readonly FinishAsk[]): Promise<readonly FinishedPair[]>;
}

// src/world/conflictTable.ts:573-581 interface PairFacts
/** DO NOT work them out again for each build */
export interface PairFacts {
  readonly sA: number;
  readonly sB: number;
  /** Per class, the centre past which the body on `a` (`exitA`) or `b` is clear of any other body. Never changed. */
  readonly exitA: readonly number[];
  readonly exitB: readonly number[];
  /** The intrusions in the order `recordIntrusions` finds them: whose (`onB`) and the two classes. */
  readonly intrusions: readonly { readonly onB: boolean; readonly mine: BodyClass; readonly theirs: BodyClass }[];
}
