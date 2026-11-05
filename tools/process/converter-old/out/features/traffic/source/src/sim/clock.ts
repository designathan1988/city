// The code of src/sim/clock.ts that is the feature's (the rest of the file is shared).

// src/sim/clock.ts:21-47 slice of src/sim/clock.ts#SimClock.advance (shared): the feature's statements; "…" marks code left out
  advance(wallDelta: number, step: () => void, budgetMs = Infinity, now: () => number = () => performance.now()): number {
    // … 5 lines
    this.acc += snapToTicks(clamp(wallDelta, 0, MAX_FRAME)) * this.speed;
    // … 4 lines
    while (this.acc >= DT && n < MAX_SUBSTEPS) {
      step();
      this.acc -= DT;
      n++;
      spent = now() - start;
      // Another step only if it would end inside the budget, at the mean cost of the steps so far
      if (spent + spent / n > budgetMs) break;
    }
    // … 1 lines
    this.lastStepMs = n > 0 ? spent / n : 0;
  // … 5 lines
  }
