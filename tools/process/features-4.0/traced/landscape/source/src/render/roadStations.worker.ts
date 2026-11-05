// The code of src/render/roadStations.worker.ts that is the feature's (the rest of the file is shared).

// src/render/roadStations.worker.ts:15-27 slice of src/render/roadStations.worker.ts#serve (shared): the feature's statements; "…" marks code left out
export function serve(scope: JobScope): void {
  scope.onmessage = (event: MessageEvent<RoadStationsJob>) => {
    // … 3 lines
    for (const road of roads) {
      const { ceil, base } = roadStations(road.xy, road.half, naturalAt);
      out.push({ id: road.id, ceil: Float64Array.from(ceil), base: Float64Array.from(base) });
    }
  // … 2 lines
  };
}
