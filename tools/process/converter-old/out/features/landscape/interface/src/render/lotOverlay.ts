// What the feature uses of src/render/lotOverlay.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/lotOverlay.ts:13-18 interface LotOverlayInput
/** DO NOT go back to geometry laid on the ground per lot; DO NOT hand every lot over again when one changes */
export interface LotOverlayInput {
  /** The shapes by group (a lot's, a proposed lot's, the gesture's). */
  readonly groups: ReadonlyMap<string, LotOverlayGroup>;
  /** The groups made, changed or dropped (no longer in `groups`) since `reader` last asked; null: all of them (its first time). */
  changedFor(reader: object): Iterable<string> | null;
}

// src/render/lotOverlay.ts:21-24 interface LotOverlayGroup
/** One group's shapes and its place in the drawing order: in each of the polygons, the lines and the dots, a higher rank over a lower. */
export interface LotOverlayGroup extends LotOverlayShapes {
  /** Under `MAX_RANK` (2^29; one over is drawn as the last under it). */
  readonly rank: number;
}

// src/render/lotOverlay.ts:33-47 interface LotOverlayShapes
/** What is drawn of one group. */
interface LotOverlayShapes {
  readonly polygons: readonly {
    readonly corners: readonly Vec2[];
    readonly fill: number | null;
    readonly fillAlpha: number;
    readonly line: number;
    readonly lineAlpha: number;
    /** Line width, metres on the ground. */
    readonly width: number;
    /** A lot with a building: its yard's paving and paths - the building's own parts, not the ground - are coloured too */
    readonly built?: boolean;
  }[];
  readonly lines: readonly { readonly a: Vec2; readonly b: Vec2; readonly colour: number; readonly dashed: boolean; readonly width: number }[];
  readonly points: readonly { readonly p: Vec2; readonly colour: number; readonly radius: number }[];
}

// src/render/lotOverlay.ts:208-252 function applyLotOverlay
/** Makes a material colour the lots into itself */
export function applyLotOverlay(material: Material, options: { readonly world?: string; readonly building?: boolean } = {}): void;
