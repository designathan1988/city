// The code of src/render/signLayer.ts that is the feature's (the rest of the file is shared).

// src/render/signLayer.ts:34-38 slice of src/render/signLayer.ts#SignSource (shared): the feature's statements; "…" marks code left out
export interface SignSource {
  // … 2 lines
  readonly elevation: RoadElevation;
}

// src/render/signLayer.ts:219-232 slice of src/render/signLayer.ts#SignLayerKept (shared): the feature's statements; "…" marks code left out
export interface SignLayerKept {
  // … 2 lines
  sync(net: Network, elevation: RoadElevation): number;
// … 9 lines
}
