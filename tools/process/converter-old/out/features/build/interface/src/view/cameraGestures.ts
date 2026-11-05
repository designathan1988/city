// What the feature uses of src/view/cameraGestures.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/view/cameraGestures.ts:39-43 interface PointerSample
/** A pointer position at a time: CSS px of the canvas, and the event's time stamp (ms, `performance.now()`'s clock). */
export interface PointerSample {
  readonly x: number;
  readonly y: number;
  readonly t: number;
}
