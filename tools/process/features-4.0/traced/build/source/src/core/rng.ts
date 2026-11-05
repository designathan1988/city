// The code of src/core/rng.ts that is the feature's (the rest of the file is shared).

// src/core/rng.ts:3-3 type RngState: the feature's own
export type RngState = readonly [number, number, number, number];

// src/core/rng.ts:30-32 method Rng.state: the feature's own
  state(): RngState {
    return [this.s0, this.s1, this.s2, this.s3];
  }

// src/core/rng.ts:35-39 method Rng.fromState: the feature's own
  static fromState(state: RngState): Rng {
    const rng = new Rng(0);
    [rng.s0, rng.s1, rng.s2, rng.s3] = state;
    return rng;
  }
