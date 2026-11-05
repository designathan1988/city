// The code of src/editor/roadTool.ts that is the feature's (the rest of the file is shared).

// src/editor/roadTool.ts:65-100 slice of src/editor/roadTool.ts#RoadToolHost (shared): the feature's statements; "…" marks code left out
export interface RoadToolHost {
  // … 23 lines
  offerElevation(solution: RoadElevation, revision: number): void;
  // … 1 lines
  groundSolve?(): RoadElevation | null;
// … 8 lines
}
