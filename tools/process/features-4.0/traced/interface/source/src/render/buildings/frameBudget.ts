// The code of src/render/buildings/frameBudget.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/frameBudget.ts:61-63 slice of src/render/buildings/frameBudget.ts#FrameScale.fit (shared): the feature's statements; "…" marks code left out
  fit(count: (scale: number) => number): void {
    this.scale = fitErrorScale(this.scale, 1, this.budget, count);
  }
