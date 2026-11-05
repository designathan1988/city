// The code of src/peopleLab/reshape.ts that is the feature's (the rest of the file is shared).

// src/peopleLab/reshape.ts:56-65 slice of src/peopleLab/reshape.ts#Reshaper.constructor (shared): the feature's statements; "…" marks code left out
  constructor(private readonly mesh: SkinnedMesh, private readonly reshape: (shape: Float32Array) => Reshaped) {
  // … 8 lines
  }

// src/peopleLab/reshape.ts:109-114 slice of src/peopleLab/reshape.ts#Reshaper.flat (shared): the feature's statements; "…" marks code left out
  private flat(shape: Float32Array): { moves: Float32Array; heads: Float32Array } {
    // … 2 lines
    r.heads.forEach((h, i) => h.toArray(heads, i * 3));
    return { moves: r.moves, heads };
  }
