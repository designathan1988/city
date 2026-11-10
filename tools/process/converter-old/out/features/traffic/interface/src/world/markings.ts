// What the feature uses of src/world/markings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/markings.ts:33-43 interface StrokeSpec
export interface StrokeSpec {
  readonly points: readonly Vec2[];
  readonly width: number;
  readonly color: string;
  /** Dash pattern in WORLD units, or null for a solid line. */
  readonly dash: readonly number[] | null;
  /** Dash phase in world units, so a split does not reflow the pattern. */
  readonly dashOffset: number;
  /** The points close a ring painted whole (`width` and `dash` unused): an area, not a line. */
  readonly fill?: true;
}

// src/world/markings.ts:147-163 function paintInLane
/** A word or symbol painted in one lane (`strokes` in a box `width` wide and one long), read by the driver coming along it */
function paintInLane(
  out: StrokeSpec[], ribbon: SegmentRibbon, centre: Polyline, forward: boolean, lane: number, along: number,
  word: { readonly strokes: readonly (readonly (readonly [number, number])[])[]; readonly width: number },
  letter: number, height: number, width: number,
): void;

// src/world/markings.ts:166-166 variable LEGEND_GAP
/** Clear floor between a stop line and the legend before it. */
const LEGEND_GAP = m(1.5);

// src/world/markings.ts:176-205 function approachLegendsAt
/** `approachLegends` of one node, onto `out`: what the surfaces paint of it again when an edit reaches it. */
export function approachLegendsAt(net: Network, nodeId: NodeId, styleId: MarkingStyleId, out: StrokeSpec[]): void;
