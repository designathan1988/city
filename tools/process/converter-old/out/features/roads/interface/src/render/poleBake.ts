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

// src/render/poleBake.ts:75-83 function framePole
/** A pole as it stands: its top, its way to the kerb (kept from before, or undefined to read it, `kerbward`), its arms. */
export function framePole(net: Network, groundAt: (x: number, y: number) => number, source: PoleSource,
  pole: UtilityPole, kept: { x: number; y: number } | null | undefined): { readonly top: number; readonly kerb: { x: number; y: number } | null; readonly frame: PoleArms };

// src/render/poleBake.ts:86-87 function wireEnd
/** A span's end at a pole, as its wires read it: the pole's place and top, and its arm for the span. */
export const wireEnd = (span: UtilitySpan, at: { readonly x: number; readonly y: number; readonly top: number; readonly frame: PoleArms }): Parameters<typeof spanWires>[1] => …;
