// The code of src/lab/catalog/layers.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/layers.ts:76-94 slice of src/lab/catalog/layers.ts#signItems (shared): the feature's statements; "…" marks code left out
export function signItems(add: Add, cache: BatchCache): void {
  // … 10 lines
  for (const type of SIGN_TYPES) {
    one(type, SIGN_HAS_TEXT.has(type) ? 'with text "Main St 50"' : 'one sign', (doc, net) => {
      doc.addLandscape('sign', { x: 0, y: footwayOut(net) }, { signType: type, ...(SIGN_HAS_TEXT.has(type) ? { text: 'Main St 50' } : {}) });
    });
  }
// … 2 lines
}

// src/lab/catalog/layers.ts:78-86 slice of src/lab/catalog/layers.ts#signItems.one (shared): the feature's statements; "…" marks code left out
  const one = (name: string, variation: string, extra: (doc: RoadDoc, net: Network) => void, junction = false): void => {
    // … 2 lines
    layer.sync(net, buildRoadElevation(net, flat));
  // … 4 lines
  };

// src/lab/catalog/layers.ts:157-157 function waterItems.basin: the feature's own
  const basin = (x: number, y: number): number => (Math.hypot(x, y) < m(40) ? -m(4) : 0);

// src/lab/catalog/layers.ts:160-160 function waterItems.bed: the feature's own
  const bed = (x: number, y: number): number => (Math.abs(y - Math.sin((x + m(80)) / m(40)) * m(10)) < m(9) ? -m(3) : 0);
