// The code of src/render/roadSurfaceReads.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfaceReads.ts:40-61 slice of src/render/roadSurfaceReads.ts#PassDesc (shared): the feature's statements; "…" marks code left out
export interface PassDesc {
  // … 1 lines
  readonly structure: RoadStructure;
// … 18 lines
}

// src/render/roadSurfaceReads.ts:77-91 slice of src/render/roadSurfaceReads.ts#SurfaceReads (shared): the feature's statements; "…" marks code left out
export interface SurfaceReads {
  readonly field: RoadReads;
// … 12 lines
}

// src/render/roadSurfaceReads.ts:108-359 slice of src/render/roadSurfaceReads.ts#passSurfaces (shared): the feature's statements; "…" marks code left out
export function passSurfaces(pass: PassDesc, reads: SurfaceReads): SurfaceRead[] {
  // … 1 lines
  const only: ReadonlySet<RoadStructure> = new Set([pass.structure]);
  // … 18 lines
  const deck: HeightFn = pass.segment !== undefined
    ? (x, y) => field.onSegment(segmentAt(x, y), x, y)
    : (x, y) => field.at(x, y, only, false);
  // … 1 lines
  const depth = roadStructure(pass.segment === undefined ? pass.structure : 'elevated').deck;
  const soffit: HeightFn = raised
    ? (x, y) => deck(x, y) - depth - FOOTWAY_RISE
    : (x, y) => Math.min(deck(x, y) - VERGE_SKIRT, reads.terrainAt(x, y) - 0.2);
  // … 102 lines
  const specs: SurfaceRead[] = [
    {
      // … 2 lines
      options: {
        // … 1 lines
        top: raised ? offset(deck, -VERGE_DROP) : vergeTop,
        bottom: soffit,
      // … 8 lines
      },
    },
    {
      // … 1 lines
      options: {
        // … 3 lines
        bottom: raised ? offset(deck, -VERGE_DROP) : offset(deck, -VERGE_SKIRT),
      // … 6 lines
      },
    },
    {
      // … 2 lines
      options: {
        // … 1 lines
        top: (x, y) => deck(x, y) + FOOTWAY_RISE * reads.footwayShare(x, y) * reads.walkingRise(x, y, pass.structure),
        bottom: raised ? offset(deck, -VERGE_DROP) : offset(deck, -VERGE_SKIRT),
      // … 3 lines
      },
    },
    {
      // … 1 lines
      options: {
        // … 1 lines
        top: offset(deck, KERB_RISE),
        bottom: deck,
      // … 3 lines
      },
    },
    {
      // … 1 lines
      options: {
        // … 1 lines
        top: deck,
        ...(raised ? { bottom: soffit } : {}),
      // … 5 lines
      },
    },
    {
      // … 1 lines
      options: {
        // … 1 lines
        top: offset(deck, MEDIAN_KERB),
        bottom: deck,
      // … 3 lines
      },
    },
    {
      // … 1 lines
      options: {
        // … 1 lines
        top: offset(deck, MEDIAN_PLANTING),
        bottom: offset(deck, MEDIAN_KERB),
      // … 3 lines
      },
    },
    {
      // … 2 lines
      options: {
        // … 1 lines
        top: offset(deck, MEDIAN_PLANTING),
        bottom: offset(deck, MEDIAN_KERB),
      // … 3 lines
      },
    },
  ];
  for (const color of pass.paint) {
    specs.push({
      // … 1 lines
      options: {
        // … 1 lines
        top: offset(deck, pass.paintLift),
      // … 6 lines
      },
    });
  }
// … 1 lines
}

// src/render/roadSurfaceReads.ts:140-147 slice of src/render/roadSurfaceReads.ts#passSurfaces.frameFor (shared): the feature's statements; "…" marks code left out
  const frameFor = (tile: number, panel?: number): UvFrameFn => (x, y, pickX, pickY, out) => {
    // … 1 lines
    const frame = field.surfaceFrameInto
      ? field.surfaceFrameInto(x, y, only, pickX, pickY, pass.segment !== undefined, segment, panel, at)
      : field.surfaceFrameAt(x, y, only, pickX, pickY, pass.segment !== undefined, segment, panel);
    out[0] = frame.across / tile;
    out[1] = frame.along / tile;
  };

// src/render/roadSurfaceReads.ts:155-171 slice of src/render/roadSurfaceReads.ts#passSurfaces.asphaltTint (shared): the feature's statements; "…" marks code left out
  const asphaltTint: TintFn = (x, y, out) => {
    const sample = pass.segment === undefined ? field.roadAt(x, y, only, false) : null;
    const type = sample ? sample.type : pass.rootType;
    // … 1 lines
    const segment = sample ? sample.segment : segmentAt(x, y);
    const look = segment === undefined ? undefined : reads.look(segment);
    let material = look?.carriageway;
    // … 1 lines
    if (material && sample && look) {
      if (sample.along < look.from || sample.along > look.to) material = undefined;
    }
    const paving = material && material !== 'asphalt' ? pass.pavingTint[material] : undefined;
    if (paving) { out[0] = paving[0]; out[1] = paving[1]; out[2] = paving[2]; return; }
    // … 1 lines
    if (type >= 0 && type * 3 < classes.length) { out[0] = classes[type * 3]!; out[1] = classes[type * 3 + 1]!; out[2] = classes[type * 3 + 2]!; return; }
  // … 1 lines
  };

// src/render/roadSurfaceReads.ts:176-183 slice of src/render/roadSurfaceReads.ts#passSurfaces.asphaltSection (shared): the feature's statements; "…" marks code left out
  const asphaltSection: TintFn = (x, y, out) => {
    const sample = field.roadAt(x, y, only, pass.segment !== undefined);
    const look = sample.segment === undefined ? undefined : reads.look(sample.segment);
    if (!look || sample.along < look.from || sample.along > look.to) { out[0] = 0; out[1] = -1; out[2] = 0; return; }
    out[0] = sample.across;
    out[1] = look.width / 2;
    out[2] = look.lane;
  };

// src/render/roadSurfaceReads.ts:185-193 slice of src/render/roadSurfaceReads.ts#passSurfaces.footwaySection (shared): the feature's statements; "…" marks code left out
  const footwaySection: TintFn = (x, y, out) => {
    const sample = field.roadAt(x, y, only, pass.segment !== undefined);
    const look = sample.segment === undefined ? undefined : reads.look(sample.segment);
    if (!look) { out[0] = 0; out[1] = -1; out[2] = 0; return; }
    const side = sample.across >= 0 ? sample.sidewalkLeft : sample.sidewalkRight;
    out[0] = Math.abs(sample.across) - look.width / 2 - CURB_BAND;
    out[1] = Math.max(m(0.5), (side ?? sample.sidewalk ?? m(2)) - CURB_BAND);
  // … 1 lines
  };

// src/render/roadSurfaceReads.ts:196-202 slice of src/render/roadSurfaceReads.ts#passSurfaces.footwayTint (shared): the feature's statements; "…" marks code left out
  const footwayTint: TintFn = (x, y, out) => {
    const sample = field.roadAt(x, y, only, pass.segment !== undefined);
    const look = sample.segment === undefined ? undefined : reads.look(sample.segment);
    const material = look ? (sample.across >= 0 ? look.footwayLeft : look.footwayRight) : undefined;
    const tint = FOOTWAY_TINT[material ?? 'pavers'];
    out[0] = tint[0]; out[1] = tint[1]; out[2] = tint[2];
  };
