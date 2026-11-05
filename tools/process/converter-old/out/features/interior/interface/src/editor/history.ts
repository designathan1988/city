// What the feature uses of src/editor/history.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/history.ts:10-156 class History
/** Undo and redo by the records an edit changed. */
export class History { … }

// src/editor/history.ts:30-32 getter History.canUndo
get canUndo(): boolean;

// src/editor/history.ts:34-36 getter History.canRedo
get canRedo(): boolean;

// src/editor/history.ts:39-43 method History.begin
/** An edit begins: the one before it is closed (or what changed on its own taken in). */
  begin(doc: RoadDoc): void;

// src/editor/history.ts:46-49 method History.end
/** The edit ends: its patch becomes a step; whether it changed anything. */
  end(doc: RoadDoc): boolean;
