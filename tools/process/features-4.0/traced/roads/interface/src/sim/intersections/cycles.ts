// What the feature uses of src/sim/intersections/cycles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/intersections/cycles.ts:11-16 interface RoadCycle
/** Small closed loops of road: rings of streets, roundabouts drawn as polygons. */

/** A small closed loop of road: its lanelets and the queue it can hold. */
export interface RoadCycle {
  readonly id: number;
  /** Road a queue can stand on: the summed length of the loop's links. */
  readonly storage: number;
  readonly lanelets: ReadonlySet<LaneletId>;
}

// src/sim/intersections/cycles.ts:19-19 variable MAX_CYCLE_STORAGE
/** Loops with more storage than this are a street network, not a ring. */
const MAX_CYCLE_STORAGE = m(560);

// src/sim/intersections/cycles.ts:21-21 variable METER_SHARE
/** Share of a loop's storage past which nobody more is let on. */
const METER_SHARE = 0.7;

// src/sim/intersections/cycles.ts:23-23 variable indexes
const indexes = new WeakMap<LaneletGraph, CycleIndex>();

// src/sim/intersections/cycles.ts:25-30 function indexOf
function indexOf(w: SimWorld): CycleIndex;

// src/sim/intersections/cycles.ts:33-35 function cyclesOf
/** The small loops a lanelet belongs to, kept by what each edit changed (`CycleIndex`). */
export function cyclesOf(w: SimWorld, lanelet: LaneletId): readonly RoadCycle[];

// src/sim/intersections/cycles.ts:38-52 function cycleFull
/** DO NOT go back to walking `w.vehicles` here: that was the whole fleet for every request onto a ring, every tick. */
export function cycleFull(w: SimWorld, from: LaneletId, to: LaneletId, need: number): boolean;

// src/sim/intersections/cycles.ts:55-203 class CycleIndex
/** DO NOT search every link of the map again for an edit: every road drawn did the town's searches again. */
export class CycleIndex { … }

// src/sim/intersections/cycles.ts:56-56 field CycleIndex.byLanelet
readonly byLanelet = new Map<LaneletId, RoadCycle[]>();

// src/sim/intersections/cycles.ts:58-58 field CycleIndex.ofStart
/** Each link searched: the loop it found (its key), and what the search read. */
  private readonly ofStart = new Map<LaneletId, { readonly key: string | null; readonly lanes: readonly LaneletId[]; readonly nodes: readonly NodeId[] }>();

// src/sim/intersections/cycles.ts:60-60 field CycleIndex.laneReaders
/** The links whose search read each lanelet, and each junction. */
  private readonly laneReaders = new Map<LaneletId, Set<LaneletId>>();

// src/sim/intersections/cycles.ts:61-61 field CycleIndex.nodeReaders
private readonly nodeReaders = new Map<NodeId, Set<LaneletId>>();

// src/sim/intersections/cycles.ts:68-84 method CycleIndex.follow
/** Brought up to the lane graph's changes since it was last. */
  follow(graph: LaneletGraph, log: ChangeLog<GraphChange>): void;

// src/sim/intersections/cycles.ts:87-96 method CycleIndex.restart
/** @rebuild Everything forgotten, and every link to search. */
  private restart(graph: LaneletGraph): Set<LaneletId>;

// src/sim/intersections/cycles.ts:99-116 method CycleIndex.forget
/** A link's search taken away: its reads, and its share of the loop it found. */
  private forget(start: LaneletId): void;

// src/sim/intersections/cycles.ts:119-202 method CycleIndex.search
/** One link's shortest way back to itself, and what it read on the way. */
  private search(graph: LaneletGraph, start: LaneletId): void;

// src/sim/intersections/cycles.ts:205-208 function file
function file<K>(lists: Map<K, Set<LaneletId>>, key: K, start: LaneletId): void;

// src/sim/intersections/cycles.ts:210-215 function unfile
function unfile<K>(lists: Map<K, Set<LaneletId>>, key: K, start: LaneletId): void;

// src/sim/intersections/cycles.ts:219-219 field OpenSet.items
private readonly items: { id: LaneletId; cost: number }[] = [];

// src/sim/intersections/cycles.ts:225-235 method OpenSet.push
push(item: { id: LaneletId; cost: number }): void;

// src/sim/intersections/cycles.ts:237-255 method OpenSet.pop
pop(): { id: LaneletId; cost: number };

// src/sim/intersections/cycles.ts:259-260 function before
/** Cheaper first, then the lower id. */
const before = (a: { id: LaneletId; cost: number }, b: { id: LaneletId; cost: number }): boolean => …;
