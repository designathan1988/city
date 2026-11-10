// The code of src/sim/people/engine.ts that is the feature's (the rest of the file is shared).

// src/sim/people/engine.ts:68-79 slice of src/sim/people/engine.ts#PeopleBridge (shared): the feature's statements; "…" marks code left out
export interface PeopleBridge {
  // … 1 lines
  hailable(w: SimWorld, lanelet: LaneletId, s0: number, s1: number,
    exclude?: ReadonlySet<number>): { readonly id: number; readonly s: number } | null;
// … 7 lines
}
