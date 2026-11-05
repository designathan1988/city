// What the feature uses of src/render/roadStations.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadStations.ts:14-17 interface Hint
/** A road's ground as read, with the plates its box read (each as the share held it: a plate made or let go since is read again) and the terrain's revision then (a stamp since: read again). */
interface Hint extends StationHint {
  readonly plates: readonly (readonly [number, SharedArrayBuffer | undefined])[];
  readonly terrain: number;
}

// src/render/roadStations.ts:19-24 function sameXY
const sameXY = (x: Float64Array, y: Float64Array): boolean =>;

// src/render/roadStations.ts:43-72 method RoadStations.waiting
/** Whether the solve must wait for the ground under `roads` (each read off the page unless its hint stands); asked now if not yet. False: solve now, with `hints`. */
  waiting(roads: readonly { readonly id: SegmentId; readonly xy: Float64Array; readonly half: number }[], ground: SharedGroundData | null, terrain: number): boolean;

// src/render/roadStations.ts:75-77 getter RoadStations.current
/** The hints for the solve (`buildRoadElevation` checks each road's line and casing again). */
  get current(): ReadonlyMap<SegmentId, StationHint>;
