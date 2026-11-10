// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:64-70 slice of src/world/doc.ts#fitRoadCurve (shared): the feature's statements; "…" marks code left out
export function fitRoadCurve(
  // … 2 lines
  parking?: SegmentParking,
): CurveShape | null {
  return fitShapeToRadius(a, b, shape, casingHalf(roadProfile(type, lanes, direction, section, parking)));
}

// src/world/doc.ts:124-144 slice of src/world/doc.ts#RoadSegment (shared): the feature's statements; "…" marks code left out
export interface RoadSegment {
  // … 14 lines
  parking?: SegmentParking;
// … 4 lines
}

// src/world/doc.ts:732-765 slice of src/world/doc.ts#RoadDoc.addSegment (shared): the feature's statements; "…" marks code left out
  addSegment(
    // … 9 lines
    parking?: SegmentParking,
  ): RoadSegment | null {
    // … 3 lines
    const authoredParking = normalizeParking(parking);
    // … 2 lines
    const s: RoadSegment = {
      // … 4 lines
      ...(authoredParking ? { parking: authoredParking } : {}),
    };
  // … 8 lines
  }

// src/world/doc.ts:964-974 slice of src/world/doc.ts#RoadDoc.setSegmentParking (shared): the feature's statements; "…" marks code left out
  setSegmentParking(id: SegmentId, parking?: SegmentParking): void {
    // … 1 lines
    const next = normalizeParking(parking);
    if (!was || sameParking(was.parking, next)) return;
    // … 2 lines
    if (next) segment.parking = next;
    else delete segment.parking;
  // … 2 lines
  }

// src/world/doc.ts:3029-3047 slice of src/world/doc.ts#segmentRecord (shared): the feature's statements; "…" marks code left out
function segmentRecord(s: SerializedDoc['segments'][number], a: NodeId, b: NodeId): RoadSegment {
  // … 1 lines
  const parking = normalizeParking(s.parking);
  return {
    // … 9 lines
    ...(parking ? { parking } : {}),
  // … 3 lines
  };
}

// src/world/doc.ts:3181-3255 slice of src/world/doc.ts#SerializedDoc (shared): the feature's statements; "…" marks code left out
export interface SerializedDoc {
  // … 18 lines
  readonly segments: readonly {
    id: number;
    a: number;
    b: number;
    type: number;
    curve: CurveShape | null;
    dashOrigin?: number;
    direction?: SegmentDirection;
    lanes?: number | null;
    section?: RoadSection;
    parking?: SegmentParking;
    /** A current structure id, or a legacy one `migrateStructure` maps. */
    structure?: RoadStructure | 'viaduct';
    /** Retaining walls in its cutting; absent on every older map. */
    cutWalls?: boolean;
  }[];
// … 39 lines
}

// src/world/doc.ts:3280-3285 slice of src/world/doc.ts#sameSegment (shared): the feature's statements; "…" marks code left out
function sameSegment(p: RoadSegment, q: RoadSegment): boolean {
  return p.a === q.a && p.b === q.b && p.type === q.type && p.dashOrigin === q.dashOrigin &&
    p.direction === q.direction && p.lanes === q.lanes && p.structure === q.structure && p.cutWalls === q.cutWalls &&
    sameRoadSection(p.section, q.section) && sameParking(p.parking, q.parking) &&
    (p.curve?.t ?? null) === (q.curve?.t ?? null) && (p.curve?.h ?? null) === (q.curve?.h ?? null);
}
