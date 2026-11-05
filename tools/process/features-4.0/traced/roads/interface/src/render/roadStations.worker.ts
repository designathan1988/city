// What the feature uses of src/render/roadStations.worker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadStations.worker.ts:7-10 interface RoadStationsJob
/** THE GROUND UNDER THE ROADS A SOLVE WILL READ, OFF THE PAGE (`renderer.ts` buildWorld, `world/elevation.ts` roadStations): each road's stations read off the shared plates' natural ground, point for point as the page reads it (`sharedGround.ts`) */

export interface RoadStationsJob {
  readonly ground: SharedGroundData;
  readonly roads: readonly { readonly id: number; readonly xy: Float64Array; readonly half: number }[];
}

// src/render/roadStations.worker.ts:11-13 interface RoadStationsReply
export interface RoadStationsReply {
  readonly roads: readonly { readonly id: number; readonly ceil: Float64Array; readonly base: Float64Array }[];
}

// src/render/roadStations.worker.ts:15-27 function serve
export function serve(scope: JobScope): void;
