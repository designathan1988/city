// What the feature uses of src/editor/lotTool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/lotTool.ts:21-21 variable REFUSED_REACH
/** How far apart a zone stroke is sampled between two pointer reports: under any lot's front. */
/** How far from a road edit a refused lot is tried again: its frontage and setbacks read the roads that near, u. */
const REFUSED_REACH = m(40);

// src/editor/lotTool.ts:27-27 type ZoneMode
/** THE ZONING TOOL: the lots the player draws, edits and zones (`world/lots.ts`), and the gestures doing it */

/** Brush paints the cells under the pointer; the other modes edit the lots. */
export type ZoneMode = 'brush' | 'fill' | 'edit' | 'front' | 'split' | 'join' | 'add' | 'polygon' | 'curve' | 'delete';

// src/editor/lotTool.ts:29-29 type LotSplitKind
/** How the split tool cuts (`LotCut`): across the front, parallel to it, or along a drawn line. */
export type LotSplitKind = 'vertical' | 'horizontal' | 'line';

// src/editor/lotTool.ts:32-46 interface LotToolHost
/** What the tool reads of the game and does to it. */
export interface LotToolHost {
  readonly doc: RoadDoc;
  readonly net: Network;
  zoom(): number;
  readonly settings: () => {
    readonly mode: ZoneMode; readonly use: ZoneUse; readonly density: ZoneDensity; readonly eraser: boolean;
    readonly splitKind: LotSplitKind; readonly splitParts: number;
  };
  /** An edit of the document, one undo step. */
  mutate(fn: () => boolean): void;
  hint(key: string): void;
  redraw(): void;
  /** The proposed lots for the land as it is now, planned off the page (the simulation's worker, `SimClient.lotPlan`); null: not now, asked again. */
  plan(): Promise<LotPlan | null>;
}

// src/editor/lotTool.ts:70-79 interface LotDecor
/** What a lot's group is drawn with besides the lot itself (`LotTool.drawLot`): -1 for no lot. */
interface LotDecor {
  readonly editing: boolean;
  readonly mode: ZoneMode;
  readonly use: ZoneUse;
  readonly eraser: boolean;
  /** The lot under the pointer. */
  readonly hover: number;
  /** The first lot of a join. */
  readonly joinFirst: number;
}

// src/editor/lotTool.ts:116-857 class LotTool
export class LotTool { … }

// src/editor/lotTool.ts:171-171 getter LotTool.net
private get net(): Network;

// src/editor/lotTool.ts:174-184 method LotTool.refusedNow
/** DO NOT let every refused lot of the town go for an edit */
  refusedNow(): Set<number>;

// src/editor/lotTool.ts:226-233 method LotTool.proposalKey
/** What the proposal was worked out from, as what its planner reads (`LotPlanner.update`): moves when the roads or the lots changed. */
  private proposalKey(): number;

// src/editor/lotTool.ts:334-334 method LotTool.snap
private snap(p: Vec2): Vec2;

// src/editor/lotTool.ts:370-373 method LotTool.snapExcept
/** A dragged corner's snap: onto the streets and other corners, never onto itself. */
  private snapExcept(p: Vec2, from: Vec2): Vec2;

// src/editor/lotTool.ts:376-385 method LotTool.frontSideOf
/** Which side of a lot is its front: the longest of those against a street */
  private frontSideOf(points: readonly Vec2[]): number;

// src/editor/lotTool.ts:388-391 method LotTool.landLot
/** A lot as drawn, cut back to the footways (`onLand`), with its front; null when nothing is left on land. */
  private landLot(points: readonly Vec2[]): { corners: Vec2[]; front: number } | null;

// src/editor/lotTool.ts:394-400 method LotTool.streetAngleNear
/** The direction of the street nearest a point, for a lot drawn there. */
  private streetAngleNear(p: Vec2): number;

// src/editor/lotTool.ts:791-855 method LotTool.refreshGesture
/** The gesture's own group (lines being drawn, a cut, a curve, the snap) and the labels: made again when what it reads changed. */
  private refreshGesture(d: LotDecor, hoverLot: Lot | undefined, lotsSeen: number): void;
