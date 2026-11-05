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
