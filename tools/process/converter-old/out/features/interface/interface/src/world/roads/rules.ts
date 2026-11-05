// What the feature uses of src/world/roads/rules.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/rules.ts:8-8 type ApproachRule
/** THE RULES OF A JUNCTION: who goes first, as the source; the signs are derived from it (V6). */
export type ApproachRule = 'priority' | 'yield' | 'stop';

// src/world/roads/rules.ts:11-14 interface ApproachRuleEntry
export interface ApproachRuleEntry {
  readonly segment: SegmentId;
  readonly rule: ApproachRule;
}

// src/world/roads/rules.ts:17-26 interface SignalSettings
/** A signal's settings; absent: adaptive, the game's own timings. */
export interface SignalSettings {
  /** 'fixed': each stage its green, in order, every cycle (needed for a green wave); absent: adaptive. */
  readonly mode?: 'fixed';
  /** Green of each stage, seconds, in a fixed-time plan; missing ones take the game's own. */
  readonly greens?: readonly number[];
  /** Where in its cycle this signal starts, seconds (a green wave's offset). */
  readonly offset?: number;
  /** A bus approaching on red or at the end of its green is served first (green extension, early green). */
  readonly busPriority?: true;
}

// src/world/roads/rules.ts:28-28 variable GREEN_LIMITS
export const GREEN_LIMITS = [6, 90] as const;

// src/world/roads/rules.ts:29-29 variable OFFSET_LIMIT
export const OFFSET_LIMIT = 300;

// src/world/roads/rules.ts:73-103 function mainRoadLegs
/** The road a junction is ON: the pair of legs whose lesser rank is the highest, the straighter pair on a tie. */
export function mainRoadLegs(doc: RoadDoc, nodeId: NodeId): readonly [SegmentId, SegmentId] | null;

// src/world/roads/rules.ts:106-111 function legRule
/** The rule of one leg of a junction with signs ('priority' control) */
export function legRule(doc: RoadDoc, node: RoadNode, segment: SegmentId): ApproachRule;
