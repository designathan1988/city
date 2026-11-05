// What the feature uses of src/world/ribbons.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/ribbons.ts:25-39 interface SegmentRibbon
export interface SegmentRibbon {
  readonly id: SegmentId;
  readonly typeIndex: number;
  readonly direction: SegmentDirection;
  /** Resolved width and marking profile, including lane overrides. */
  readonly road: ReturnType<typeof roadProfile>;
  /** Centreline trimmed for this level, in world space. */
  readonly centre: Record<number, Polyline>;
  /** Closed carriageway outline for this level. */
  readonly rings: Record<number, Ring>;
  /** Arc-length offset for dash phase continuity across splits. */
  readonly dashOrigin: number;
  /** Full untrimmed centreline, shared by every level. */
  readonly full: Polyline;
}
