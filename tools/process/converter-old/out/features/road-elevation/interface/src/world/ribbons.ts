// What the feature uses of src/world/ribbons.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/ribbons.ts:18-23 interface SegmentTrims
/** A road's surface between the junctions at its ends (`Network.ribbons`) */

/** Per-level trim distances at both ends of a segment. */
export interface SegmentTrims {
  /** Trim at the `a` endpoint, indexed by surface level. */
  readonly a: Record<number, number>;
  /** Trim at the `b` endpoint, indexed by surface level. */
  readonly b: Record<number, number>;
}

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

// src/world/ribbons.ts:92-94 getter LeanRibbon.centre
get centre(): Record<number, Polyline>;

// src/world/ribbons.ts:199-211 function junctionBounds
/** Every ring a node's junctions put on any level; null for none. */
export function junctionBounds(byLevel: ReadonlyMap<SurfaceLevel, Junction>): ChangeRect | null;
