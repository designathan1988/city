// What the feature uses of src/sim/agents/trafficBuildings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/trafficBuildings.ts:692-695 interface BuildingSent
/** A building of the page changed, as it is posted to the worker: where its record is read (`refOf`: its tile's shared table and row, or a table of its own), null when it is gone; DO NOT project it on the page. */
export interface BuildingSent {
  readonly id: BuildingId;
  readonly ref: RecordRef | null;
}

// src/sim/agents/trafficBuildings.ts:698-703 interface TrafficMap
/** What the worker's copy of the map is made of: the roads and the transit, the lots and their keys, the buildings as the document's own buffers (`BuildingSections`: its table, shared, and the records written over it). */
export interface TrafficMap {
  readonly doc: SerializedDoc;
  readonly lots: readonly Lot[];
  readonly lotKeys: readonly string[];
  readonly buildings: BuildingSections;
}
