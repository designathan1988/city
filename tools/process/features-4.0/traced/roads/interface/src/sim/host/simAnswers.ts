// What the feature uses of src/sim/host/simAnswers.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/simAnswers.ts:14-21 interface TrafficCount
/** THE PANELS' QUESTIONS ANSWERED IN THE SIMULATION'S WORKER (`simHost.ts`): plain numbers, worded on the page */

/** The traffic on a road's lanes now, or on a junction's approaches. */
export interface TrafficCount {
  readonly vehicles: number;
  readonly queued: number;
  readonly speedSum: number;
  readonly speedCount: number;
  readonly capacity: number;
  readonly volume: number;
}

// src/sim/host/simAnswers.ts:24-31 interface JunctionAnswer
/** What the junction panel and the inspector show of a junction. */
export interface JunctionAnswer extends TrafficCount {
  readonly known: boolean;
  readonly signalised: boolean;
  readonly choice: string | null;
  readonly flows: readonly (readonly [SegmentId, number])[];
  readonly connectors: readonly { readonly inSegment: SegmentId; readonly outSegment: SegmentId; readonly turn: string | null }[];
  readonly plan: { readonly cycle: number; readonly stageIndex: number; readonly sub: string; readonly greens: readonly number[] } | null;
}

// src/sim/host/simAnswers.ts:34-34 variable QUEUED
/** Below this speed a vehicle counts as queued (u/s), as the panels have always counted it. */
const QUEUED = 0.6;

// src/sim/host/simAnswers.ts:36-54 function count
function count(sim: SimWorld, lanes: Iterable<string>, volume: number): TrafficCount;

// src/sim/host/simAnswers.ts:57-67 function roadLanes
/** A road's lanes, each way, by their ids (DO NOT walk `sim.graph.lanelets`: every lane of the city). */
function* roadLanes(sim: SimWorld, doc: RoadDoc, id: SegmentId): Generator<string>;

// src/sim/host/simAnswers.ts:69-77 function answer
export function answer(sim: SimWorld, doc: RoadDoc, _net: Network, ask: SimAsk): unknown;

// src/sim/host/simAnswers.ts:79-102 function junctionAnswer
function junctionAnswer(sim: SimWorld, doc: RoadDoc, id: NodeId): JunctionAnswer | null;
