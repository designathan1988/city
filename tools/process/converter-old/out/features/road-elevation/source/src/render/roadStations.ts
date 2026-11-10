// The code of src/render/roadStations.ts that is the feature's (the rest of the file is shared).

// src/render/roadStations.ts:14-17 slice of src/render/roadStations.ts#Hint (shared): the feature's statements; "…" marks code left out
interface Hint extends StationHint {
// … 2 lines
}

// src/render/roadStations.ts:43-72 slice of src/render/roadStations.ts#RoadStations.waiting (shared): the feature's statements; "…" marks code left out
  waiting(roads: readonly { readonly id: SegmentId; readonly xy: Float64Array; readonly half: number }[], ground: SharedGroundData | null, terrain: number): boolean {
    // … 2 lines
    const missing = roads.filter((r) => { const h = this.hints.get(r.id); return !(h && h.half === r.half && sameXY(h.xy, r.xy) && stands(h)); });
  // … 25 lines
  }

// src/render/roadStations.ts:75-77 slice of src/render/roadStations.ts#RoadStations.current (shared): the feature's statements; "…" marks code left out
  get current(): ReadonlyMap<SegmentId, StationHint> {
  // … 1 lines
  }
