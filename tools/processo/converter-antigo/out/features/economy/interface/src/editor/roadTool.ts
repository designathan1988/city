// What the feature uses of src/editor/roadTool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roadTool.ts:33-33 variable VERDICT_DELAY
/** How long a draft rests before the preview judges it (`RoadTool.verdict`), ms. */
const VERDICT_DELAY = 120;

// src/editor/roadTool.ts:36-36 type RefusalReason
/** A refusal the preview names: the editing rules', or a crossing at the wrong height. */
export type RefusalReason = RoadEditRefusal | 'clearance';

// src/editor/roadTool.ts:44-53 interface RoadDraft
/** A road being drawn: a drag, or a stretch chained from the last road's end. */
export interface RoadDraft {
  readonly start: Anchor;
  readonly startHeightOffset: number;
  readonly chained: boolean;
  readonly pressedAt: Vec2;
  snap: SnapResult;
  readonly samples: RoadPathPoint[];
  heightOffset: number;
  curveControl?: Vec2;
}

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

// src/editor/roadTool.ts:440-514 method RoadTool.layPart
/** One part of a road laid from `start` to `end`, one undo step (joined into `group`'s); `last`: the gesture ends with it. False when refused. */
  private layPart(d: RoadDraft, start: Anchor, end: Anchor, endHeightOffset: number, pieces: RoadPathPiece[], last: boolean, group: number | null): boolean;

// src/editor/roadTool.ts:587-587 type DraftKey
/** A draft's numbers as it is judged (`RoadTool.draftKey`): the same array while none of them moved. */
type DraftKey = readonly (string | number | null)[];
