// The code of src/render/propCells.ts that is the feature's (the rest of the file is shared).

// src/render/propCells.ts:515-518 method createPropLayer.forgetGrowing: the feature's own
    forgetGrowing() {
      near.forget([...growingNear]);
      far.forEach((f, cls) => f.forget([...growingFar[cls]!]));
    },
