// What the feature uses of src/render/poleBake.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/poleBake.ts:10-17 interface PoleSource
/** WHAT THE POLE LAYER'S PARTS SHARE (`poleLayer.ts`): a pole's frame, a wire's ends, a cell's bounds */
/** What a pole layer is built from: the document's poles and spans, or a planned run (`buildPolePreview`). */
export interface PoleSource {
  readonly poles: ReadonlyMap<PoleId, UtilityPole>;
  readonly spans: ReadonlyMap<SpanId, UtilitySpan>;
  /** The spans reaching a pole, in the order they were strung (`RoadDoc.spansAt`). */
  spansAt(id: PoleId): Iterable<UtilitySpan>;
  /** Which poles get a mast, arms and lamp here; a pole a preview only ties into is standing already. */
  readonly draws?: (pole: UtilityPole) => boolean;
}
