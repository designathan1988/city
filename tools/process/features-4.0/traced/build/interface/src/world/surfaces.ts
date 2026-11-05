// What the feature uses of src/world/surfaces.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/surfaces.ts:9-9 type SurfaceSegmentFilter
export type SurfaceSegmentFilter = (segment: SegmentId) => boolean;

// src/world/surfaces.ts:12-17 interface SurfaceRibbon
export interface SurfaceRibbon {
  readonly id: SegmentId;
  readonly road: RoadType;
  readonly rings: Record<number, Ring>;
  readonly full: Polyline;
}

// src/world/surfaces.ts:18-20 interface SurfaceJunction
export interface SurfaceJunction {
  readonly rings: readonly Ring[];
}

// src/world/surfaces.ts:21-30 interface SurfaceNet
export interface SurfaceNet {
  readonly doc: {
    node(id: NodeId): { readonly incident: readonly SegmentId[] } | undefined;
    segment(id: SegmentId): { readonly a: NodeId; readonly b: NodeId; readonly curve: CurveShape | null; readonly structure: RoadStructure } | undefined;
    degree(id: NodeId): number;
  };
  readonly trims: ReadonlyMap<SegmentId, { readonly a: Record<number, number>; readonly b: Record<number, number> }>;
  readonly ribbons: ReadonlyMap<SegmentId, SurfaceRibbon>;
  readonly junctions: ReadonlyMap<NodeId, ReadonlyMap<SurfaceLevel, SurfaceJunction>>;
}

// src/world/surfaces.ts:81-126 function levelRingsOf
/** `levelRings` of the given ribbons and junctions only: the rings round one place */
export function levelRingsOf(
  net: SurfaceNet, level: SurfaceLevel, ribbons: Iterable<SurfaceRibbon>,
  junctions: Iterable<readonly [NodeId, ReadonlyMap<SurfaceLevel, SurfaceJunction>]>, include?: SurfaceSegmentFilter,
): MultiPoly;
