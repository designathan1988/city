// The code of src/lab/catalog/layers.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/layers.ts:78-86 slice of src/lab/catalog/layers.ts#signItems.one (shared): the feature's statements; "…" marks code left out
  const one = (name: string, variation: string, extra: (doc: RoadDoc, net: Network) => void, junction = false): void => {
    // … 2 lines
    layer.sync(net, buildRoadElevation(net, flat));
  // … 4 lines
  };
