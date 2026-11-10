// What the feature uses of src/sim/agents/kerbBays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/kerbBays.ts:21-27 interface KerbWorld
/** THE BAYS ALONG THE KERBS, KEPT BY STREET (`parking.ts` `collectBays` hands them out after the lots' stalls). */

/** DO NOT import `SimWorld`: the world reaches this file, and the type import closed an import cycle */
export interface KerbWorld {
  readonly doc: RoadDoc;
  readonly net: Network;
  readonly topologyLog: ChangeLog<GraphChange>;
  /** The buildings' entrances (`TrafficBuilding.entrances`). */
  readonly buildings: TrafficBuildings;
}
