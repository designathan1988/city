// What the feature uses of src/editor/roads/profile.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roads/profile.ts:8-11 interface ApplyResult
/** Applying a profile to roads already built (: "apply without demolishing") */
export interface ApplyResult {
  readonly changed: boolean;
  readonly problems: readonly ProfileProblem[];
}

// src/editor/roads/profile.ts:13-34 function applyProfileTo
export function applyProfileTo(doc: RoadDoc, ids: readonly SegmentId[], profile: RoadProfileSpec, type?: number): ApplyResult;
