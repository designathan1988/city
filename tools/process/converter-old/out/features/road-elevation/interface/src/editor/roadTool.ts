// What the feature uses of src/editor/roadTool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roadTool.ts:33-33 variable VERDICT_DELAY
/** How long a draft rests before the preview judges it (`RoadTool.verdict`), ms. */
const VERDICT_DELAY = 120;

// src/editor/roadTool.ts:36-36 type RefusalReason
/** A refusal the preview names: the editing rules', or a crossing at the wrong height. */
export type RefusalReason = RoadEditRefusal | 'clearance';

// src/editor/roadTool.ts:65-100 interface RoadToolHost
/** What the road tool reads of the game and does to it. */
export interface RoadToolHost {
  readonly doc: RoadDoc;
  readonly net: Network;
  zoom(): number;
  /** World units for `n` screen pixels at the current zoom (`Camera.px`). */
  px(n: number): number;
  /** The road in hand: class, lanes, plan, the height it is drawn at. */
  settings(): {
    readonly typeIndex: number; readonly lanes: number | null; readonly alignment: 'straight' | 'curve' | 'free';
    readonly heightOffset: number; readonly parking: SegmentParking | undefined; readonly width: number | null; readonly grid: boolean;
    /** A profile new roads are laid with, over the class, lanes, width and parking; null for those. */
    readonly profile?: { readonly profile: RoadProfileSpec; readonly type: number } | null;
    /** Retaining walls in the cuttings of new roads instead of a batter. */
    readonly cutWalls?: boolean;
    /** The furniture the new roads are built with (V7, `world/roads/furnitureSets.ts`); absent: none. */
    readonly furniture?: FurnitureSet;
  };
  /** Sets the height the road is drawn at (the game's state, with the cause). */
  setHeight(value: number, cause: string): void;
  /** The world point under screen point (px, py) at `height` above the ground plane. */
  worldAtScreen(px: number, py: number, height: number): Vec2;
  /** The natural ground's height, for the tunnel test of a commit. */
  naturalHeightAt(x: number, y: number): number;
  /** The elevation solved by the commit's tunnel test, offered to the renderer. */
  offerElevation(solution: RoadElevation, revision: number): void;
  /** The roads' heights the game last solved on the natural ground as it stands, or null (the land moved since) */
  groundSolve?(): RoadElevation | null;
  /** The roads just laid blink. */
  flash(ids: readonly SegmentId[]): void;
  /** The undo steps of the edits made from now joined into one (`History.grouped`), until `null`: a long road's parts. */
  undoGroup?(group: number | null): void;
  /** An edit of the document, one undo step. */
  mutate(fn: () => boolean): void;
  hint(key: string): void;
  redraw(): void;
}

// src/editor/roadTool.ts:103-569 class RoadTool
/** THE ROAD TOOL: a road drawn by a drag */
export class RoadTool { … }

// src/editor/roadTool.ts:356-369 method RoadTool.verdict
/** Why the road in hand would be refused (`editRules.ts`, or a crossing at the wrong height), or null */
  verdict(): RefusalReason | null;

// src/editor/roadTool.ts:371-372 field RoadTool.judged
private judged: { key: DraftKey | null; reason: RefusalReason | null; cost: number | null; stations: readonly RoadStation[] | null } =
    { key: null, reason: null, cost: null, stations: null };

// src/editor/roadTool.ts:375-378 method RoadTool.stations
/** The road in hand as it would be built: the dry run's stations */
  stations(): readonly RoadStation[] | null;

// src/editor/roadTool.ts:381-384 method RoadTool.cost
/** What the road in hand would cost (`world/economy.ts`), judged with the verdict on the same dry run, or null until it is */
  cost(): number | null;

// src/editor/roadTool.ts:419-431 method RoadTool.judge
private judge(key: DraftKey): void;

// src/editor/roadTool.ts:587-587 type DraftKey
/** A draft's numbers as it is judged (`RoadTool.draftKey`): the same array while none of them moved. */
type DraftKey = readonly (string | number | null)[];
