// The code of src/render/sharedGround.ts that is the feature's (the rest of the file is shared).

// src/render/sharedGround.ts:16-27 slice of src/render/sharedGround.ts#SharedGroundData (shared): the feature's statements; "…" marks code left out
export interface SharedGroundData {
  // … 5 lines
  readonly relief: ReliefVersion | null;
// … 4 lines
}

// src/render/sharedGround.ts:65-65 slice of src/render/sharedGround.ts#GroundShare.relief (shared): the feature's statements; "…" marks code left out
  private relief: ReliefVersion | null = null;

// src/render/sharedGround.ts:87-91 slice of src/render/sharedGround.ts#GroundShare.setRelief (shared): the feature's statements; "…" marks code left out
  setRelief(relief: ReliefVersion | null): void {
    if (relief === this.relief) return;
    this.relief = relief;
  // … 1 lines
  }

// src/render/sharedGround.ts:126-126 slice of src/render/sharedGround.ts#sharedGroundReaders.bare (shared): the feature's statements; "…" marks code left out
  const bare = (x: number, y: number): number => data.base + (data.relief === null ? 0 : reliefHeight(data.relief, x, y));

// src/render/sharedGround.ts:127-135 slice of src/render/sharedGround.ts#sharedGroundReaders.read (shared): the feature's statements; "…" marks code left out
  const read = (natural: boolean) => (x: number, y: number): number => {
    // … 6 lines
    return natural ? sampleCorners(grid, v.natural, v.naturalFlip, lx, ly) : sampleCorners(grid, v.rendered, v.flip, lx, ly);
  };
