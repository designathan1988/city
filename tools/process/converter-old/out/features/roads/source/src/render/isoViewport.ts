// The code of src/render/isoViewport.ts that is the feature's (the rest of the file is shared).

// src/render/isoViewport.ts:112-245 slice of src/render/isoViewport.ts#createIsoRig.apply (shared): the feature's statements; "…" marks code left out
  const apply = (): void => {
    // … 3 lines
    const edge = MAP_HALF;
    target.x = Math.max(-edge, Math.min(edge, target.x));
    target.z = Math.max(-edge, Math.min(edge, target.z));
  // … 126 lines
  };
