// What the feature uses of src/editor/history.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/history.ts:10-156 class History
/** Undo and redo by the records an edit changed. */
export class History { … }

// src/editor/history.ts:159-161 function followNetwork
/** The network after an undo or a redo: rebuilt only when the roads moved */
export function followNetwork(doc: RoadDoc, net: Network): void;

// src/editor/history.ts:176-185 function restoreInto
export function restoreInto(target: RoadDoc, data: SerializedDoc, net: Network, table: ArrayBufferLike | null = null): void;

// src/editor/history.ts:188-192 function restoreSnapshot
/** Restores a document saved by the model itself exactly (a failed load put back, a gesture cancelled). */
export function restoreSnapshot(target: RoadDoc, data: SerializedDoc, net: Network): void;
