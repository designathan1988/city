// What the feature uses of src/conflictPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/conflictPool.ts:165-181 method ConflictPool.buildJunctions
/** A whole build's junctions built across the workers (`conflictTable.worker.ts`, `buildJunctionOf`) */
  buildJunctions(asks: readonly JunctionAsk[]): Promise<readonly JunctionAnswer[]>;
