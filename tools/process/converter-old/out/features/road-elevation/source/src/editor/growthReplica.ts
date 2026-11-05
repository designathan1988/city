// The code of src/editor/growthReplica.ts that is the feature's (the rest of the file is shared).

// src/editor/growthReplica.ts:23-32 slice of src/editor/growthReplica.ts#RoadPiece (shared): the feature's statements; "…" marks code left out
export interface RoadPiece {
  // … 5 lines
  readonly structure: RoadStructure | null;
// … 2 lines
}

// src/editor/growthReplica.ts:163-163 slice of src/editor/growthReplica.ts#Structured (shared): the feature's statements; "…" marks code left out
interface Structured { readonly structure: RoadStructure }
