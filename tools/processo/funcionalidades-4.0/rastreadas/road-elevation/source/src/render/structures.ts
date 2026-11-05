// The code of src/render/structures.ts that is the feature's (the rest of the file is shared).

// src/render/structures.ts:51-85 slice of src/render/structures.ts#structureRibbons (shared): the feature's statements; "…" marks code left out
function structureRibbons(
  // … 1 lines
  elevation: RoadElevation,
// … 31 lines
}

// src/render/structures.ts:95-152 slice of src/render/structures.ts#retainingWalls (shared): the feature's statements; "…" marks code left out
function retainingWalls(
  net: Network, walled: readonly SegmentRibbon[], elevation: RoadElevation, terrainAt: (x: number, y: number) => number,
// … 1 lines
): void {
  for (const ribbon of walled) {
    // … 5 lines
    for (const side of [1, -1] as const) {
      // … 2 lines
      for (let i = 0; i <= steps; i++) {
        // … 3 lines
        const deck = elevation.onSegment(ribbon.id, f.p.x, f.p.y);
        // … 1 lines
        const out = edge + CUT_WALL_REACH - m(0.2);
        const top = terrainAt(f.p.x + ox * out, f.p.y + oy * out);
        // … 1 lines
        const here: Section | null = buildModeAt(structure, deck - top) === 'cutting'
          ? { x: f.p.x + ox * edge, y: f.p.y + oy * edge, ox, oy, deck, top, s }
          : null;
        // … 1 lines
        const end = here && !prev ? here : prev && !here ? prev : null;
        if (end) {
          const tx = f.t.x, ty = f.t.y, sign = here ? -1 : 1;
          const at = (out: number, h: number): [number, number, number] => [end.x + end.ox * out, end.y + end.oy * out, h];
          faces.quad(at(0, end.deck - WALL_FOOT), at(CUT_WALL_REACH - m(0.2), end.deck - WALL_FOOT), at(CUT_WALL_REACH - m(0.2), end.top + m(0.016)), at(0, end.top + WALL_COPING),
            [tx * sign, ty * sign, 0], [0, (end.deck - WALL_FOOT) / faceTile, CUT_WALL_REACH / faceTile, (end.top + WALL_COPING) / faceTile]);
        }
        if (prev && here) {
          const a = prev, b = here;
          const at = (q: Section, out: number, h: number): [number, number, number] => [q.x + q.ox * out, q.y + q.oy * out, h];
          const u0 = a.s / faceTile, u1 = b.s / faceTile;
          // The face towards the road.
          faces.quad(at(a, 0, a.deck - WALL_FOOT), at(b, 0, b.deck - WALL_FOOT), at(b, 0, b.top + WALL_COPING), at(a, 0, a.top + WALL_COPING),
            [-(a.ox + b.ox) / 2, -(a.oy + b.oy) / 2, 0], [u0, (a.deck - WALL_FOOT) / faceTile, u1, (a.top + WALL_COPING) / faceTile]);
          // The coping.
          faces.quad(at(a, 0, a.top + WALL_COPING), at(b, 0, b.top + WALL_COPING), at(b, WALL_THICK, b.top + WALL_COPING), at(a, WALL_THICK, a.top + WALL_COPING),
            [0, 0, 1], [u0, 0, u1, WALL_THICK / faceTile]);
          // The back of the coping, down to the backfill.
          faces.quad(at(a, WALL_THICK, a.top + WALL_COPING), at(b, WALL_THICK, b.top + WALL_COPING), at(b, WALL_THICK, b.top), at(a, WALL_THICK, a.top),
            [(a.ox + b.ox) / 2, (a.oy + b.oy) / 2, 0], [u0, 0, u1, WALL_COPING / faceTile]);
          // The backfill over the band the cutting was dug back.
          const back = CUT_WALL_REACH - m(0.2);
          // Flat from the coping to where it meets the ground.
          const pa = at(a, WALL_THICK, a.top + m(0.016)), pb = at(b, WALL_THICK, b.top + m(0.016));
          const qa = at(a, back, a.top + m(0.016)), qb = at(b, back, b.top + m(0.016));
          lids.quadWorld(pa, pb, qb, qa, [0, 0, 1], lidTile);
        }
        prev = here;
      }
    }
  }
}

// src/render/structures.ts:452-465 slice of src/render/structures.ts#buildStructureDetails (shared): the feature's statements; "…" marks code left out
export function buildStructureDetails(
  // … 1 lines
  elevation: RoadElevation,
// … 8 lines
): StructureDetails {
  return structureMeshes(structureParts(net, elevation, terrainAt, materials.scale.deck, only), materials, groundMaterial, shapes);
}

// src/render/structures.ts:468-672 slice of src/render/structures.ts#structureParts (shared): the feature's statements; "…" marks code left out
export function structureParts(net: Network, elevation: RoadElevation, terrainAt: (x: number, y: number) => number, deckTile: number, only?: readonly SegmentRibbon[]): StructureParts {
  const { raised, tunnels, walled } = structureRibbons(net, elevation, terrainAt, undefined, only);
  const spans = [...raised, ...tunnels, ...walled].map((ribbon) => {
    const bb = ribbon.full.bbox;
    return [bb.minX - SPAN_REACH, bb.minY - SPAN_REACH, bb.maxX + SPAN_REACH, bb.maxY + SPAN_REACH] as const;
  });
  // … 6 lines
  for (const ribbon of raised) {
    const segment = net.doc.requireSegment(ribbon.id);
    const structure = segment.structure as RoadStructure;
    const deckAt = (x: number, y: number): number => elevation.onSegment(segment.id, x, y);
    const spec = roadStructure(isRaised(structure) ? structure : 'elevated');
    const spacing = structure === 'bridge' ? ROAD_TUNING.piers.spacingBridge : ROAD_TUNING.piers.spacingElevated;
    const radius = structure === 'bridge' ? m(1.28) : m(1.04);
    const length = ribbon.full.length;
    const casing = casingHalf(ribbon.road);

    // ------------------------------------------------------------ abutments Where a ramp leaves the ground
    const liftAt = (s: number): number => {
      const f = ribbon.full.sampleAt(s);
      return deckAt(f.p.x, f.p.y) - spec.deck - m(0.144) - terrainAt(f.p.x, f.p.y);
    };
    const abutmentAt: number[] = [];
    const scan = 2;
    let before = liftAt(0);
    for (let s = scan; s <= length; s += scan) {
      const here = liftAt(s);
      if ((before < ABUTMENT_LIFT) !== (here < ABUTMENT_LIFT)) {
        const at = s - scan / 2;
        const f = ribbon.full.sampleAt(at);
        const soffit = deckAt(f.p.x, f.p.y) - spec.deck - m(0.144);
        const foot = Math.min(terrainAt(f.p.x, f.p.y), soffit) - 1;
        abutments.push(boxAlong(f, ABUTMENT_DEPTH, casing * 2, soffit - foot, (soffit + foot) / 2));
        abutmentAt.push(at);
      }
      before = here;
    }

    // ---------------------------------------------------------------- piers Columns stand out at the deck edges rather than
    const halfDeck = sidewalkHalf(ribbon.road);
    const spread = Math.max(0, Math.min(halfDeck * BENT_SPREAD, halfDeck - radius - 1));
    const sides: readonly number[] = spread > 0 ? [-1, 1] : [0];

    /** One bent: a crossbeam under the soffit, on a column at each deck edge. */
    const placeBent = (frame: Frame): void => {
      const soffit = deckAt(frame.p.x, frame.p.y) - spec.deck - m(0.144) - BEARING;
      const beam = soffit - CAP_HEIGHT;

      const standing: { readonly x: number; readonly y: number; readonly ground: number }[] = [];
      for (const side of sides) {
        const x = frame.p.x + frame.n.x * spread * side;
        const y = frame.p.y + frame.n.y * spread * side;
        const ground = terrainAt(x, y);
        if (beam - ground < MIN_SUPPORT) continue;
        standing.push({ x, y, ground });
      }
      if (standing.length === 0) return;

      // One width for the whole bent, from the TALLEST of its feet
      const tallest = standing.reduce((mx, foot) => Math.max(mx, beam - foot.ground), 0);
      const width = Math.min(radius * PIER_MAX_WIDTH_FACTOR, Math.max(radius, tallest / PIER_SLENDERNESS));
      // Not on another road: the column's whole foot clear of its casing, not only its axis.
      const feet = standing.filter((foot) => !onAnotherCasing(net, segment.id, foot.x, foot.y, width * PIER_FOOT_FLARE));
      if (feet.length === 0) return;

      for (const foot of feet) {
        const height = beam - foot.ground + 0.5;
        piers.push({ x: foot.x, y: foot.y, yaw: angleOf(frame.t), sx: width, sy: height, sz: width, cy: foot.ground - 0.5 + height / 2 });
      }
      // The crossbeam follows the columns it rests on, or a tall bent grows a beam narrower than the legs under it.
      caps.push(boxAlong(frame, width * CAP_DEPTH, spread * 2 + width + CAP_OVERHANG * 2, CAP_HEIGHT, beam + CAP_HEIGHT / 2));
    };

    let placed = 0;
    for (let s = spacing * 0.5; s < length - spacing * 0.35; s += spacing) {
      if (abutmentAt.some((at) => Math.abs(at - s) < PIER_ABUTMENT_GAP)) continue;
      placeBent(ribbon.full.sampleAt(s));
      placed++;
    }
    if (placed === 0 && length > 0) {
      const mid = length / 2;
      if (!abutmentAt.some((at) => Math.abs(at - mid) < PIER_ABUTMENT_GAP)) placeBent(ribbon.full.sampleAt(mid));
    }

    // ------------------------------------------------------------- parapets Along the casing's own centreline
    const centre = ribbon.centre[Level.Casing] ?? ribbon.full;
    const edge = casing - m(0.02);
    for (const side of [1, -1] as const) {
      parapetRun(parapets, centre, side, edge, deckAt, terrainAt, deckTile);
    }
  }
  // … 3 lines
  if (walled.length > 0) {
    const faces = new Extrusion();
    const lids = new Extrusion();
    retainingWalls(net, walled, elevation, terrainAt, faces, lids, deckTile, TERRAIN_UV);
    walls = faces.sheet();
    backfill = lids.sheet();
  }
  // … 6 lines
  if (TUNNELS_DRAWN && tunnels.length > 0) {
    for (const ribbon of tunnels) {
      const segment = net.doc.requireSegment(ribbon.id);
      const length = ribbon.full.length;
      const step = Math.min(m(2.4), Math.max(m(0.8), length / 200));
      const half = sidewalkHalf(ribbon.road);
      const jambWidth = m(0.88);

      const coverAt = (s: number): number => {
        const frame = ribbon.full.sampleAt(s);
        return terrainAt(frame.p.x, frame.p.y) - elevation.onSegment(segment.id, frame.p.x, frame.p.y);
      };

      let previous = coverAt(0);
      const buriedJoin = (nodeId: typeof segment.a): boolean => {
        const node = net.doc.node(nodeId);
        return !!node && node.incident.length > 1 && node.heightOffset < -TUNNEL_HEADROOM;
      };
      for (let s = step; s <= length; s += step) {
        const cover = coverAt(s);
        const crossed =
          (previous < TUNNEL_PORTAL_COVER && cover >= TUNNEL_PORTAL_COVER) ||
          (previous >= TUNNEL_PORTAL_COVER && cover < TUNNEL_PORTAL_COVER);
        previous = cover;
        if (!crossed) continue;
        // A portal belongs to the alignment that actually reaches daylight.
        if ((s < m(16) && buriedJoin(segment.a)) ||
          (length - s < m(16) && buriedJoin(segment.b))) continue;

        // Linear interpolation is enough: the cover changes by at most a few tenths of a unit over one step.
        const frame = ribbon.full.sampleAt(s - step / 2);
        const road = elevation.onSegment(segment.id, frame.p.x, frame.p.y);
        const yaw = angleOf(frame.t);
        const opening = TUNNEL_HEADROOM;
        const crown = road + opening + TUNNEL_ARCH;
        // The face has to be at least as wide as the band of ground the cutting held down
        const faceHalf = half + jambWidth + PORTAL_WING;
        // Tall enough to carry the arch and stand proud of the ground at the foot of the cutting
        const top = Math.max(crown + m(0.6), terrainAt(frame.p.x, frame.p.y) + m(0.8));

        // Either side of the opening, the face as WING WALLS stepping down with the hillside
        for (const side of [-1, 1] as const) {
          const inner = half;
          const width = faceHalf - inner;
          const slices = Math.max(2, Math.ceil(width / PORTAL_SLICE));
          const w = width / slices;
          for (let k = 0; k < slices; k++) {
            const centre = inner + w * (k + 0.5);
            const x = frame.p.x + frame.n.x * centre * side;
            const y = frame.p.y + frame.n.y * centre * side;
            // The higher ground either side of the face
            const reach = PORTAL_THICKNESS / 2 + 2;
            const ground = Math.max(terrainAt(x, y), terrainAt(x + frame.t.x * reach, y + frame.t.y * reach), terrainAt(x - frame.t.x * reach, y - frame.t.y * reach));
            const low = Math.min(terrainAt(x + frame.t.x * reach, y + frame.t.y * reach), terrainAt(x - frame.t.x * reach, y - frame.t.y * reach));
            const jamb = centre - inner < jambWidth;
            // A wing over ground lower than the road holds nothing back: none there.
            if (!jamb && ground < road + 1) continue;
            const sliceTop = jamb ? Math.max(crown + m(0.6), Math.min(top, ground + PORTAL_COPING)) : Math.min(top, ground + PORTAL_COPING);
            const foot = Math.min(road, low) - 1;
            if (sliceTop - foot < 0.5) continue;
            jambs.push({ x, y, yaw, sx: PORTAL_THICKNESS, sy: sliceTop - foot, sz: w + m(0.008), cy: foot + (sliceTop - foot) / 2 });
          }
        }
        lintels.push({
          x: frame.p.x,
          y: frame.p.y,
          yaw,
          sx: PORTAL_THICKNESS,
          sy: Math.max(m(0.32), top - crown),
          sz: (half + jambWidth) * 2,
          cy: crown + Math.max(m(0.32), top - crown) / 2,
        });
        // A coping course along the top of the opening
        headwalls.push({
          x: frame.p.x,
          y: frame.p.y,
          yaw,
          sx: PORTAL_THICKNESS + m(0.64),
          sy: m(0.36),
          sz: (half + jambWidth) * 2 + m(0.64),
          cy: top + m(0.18),
        });
      }
    }

  }
  // … 1 lines
  return {
    // … 2 lines
    parapets: parapets.sheet(), walls, backfill, spans,
  };
}
