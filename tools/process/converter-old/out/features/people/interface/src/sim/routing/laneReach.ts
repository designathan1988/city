// What the feature uses of src/sim/routing/laneReach.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/routing/laneReach.ts:7-13 interface ReachWorld
/** DO NOT import `SimWorld`: `sim/world.ts` imports this file, and the type import closed an import cycle */
export interface ReachWorld {
  readonly doc: RoadDoc;
  readonly graph: LaneletGraph;
  readonly topologyLog: ChangeLog<GraphChange>;
  lanelet(id: LaneletId): Lanelet | undefined;
  connector(id: string): Connector | undefined;
}

// src/sim/routing/laneReach.ts:340-374 class LaneReaches
/** Each body class's reach, kept with the world (`SimWorld.laneReaches`). */
export class LaneReaches { … }

// src/sim/routing/laneReach.ts:353-358 method LaneReaches.mayReach
/** Whether a link lane can lead to another for a body (false: no route, DO NOT search the graph for one); true when it may, or when it cannot be said. */
  mayReach(w: ReachWorld, start: LaneletId, goal: LaneletId, body: BodyClass): boolean;
