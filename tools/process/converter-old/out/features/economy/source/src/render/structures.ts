// The code of src/render/structures.ts that is the feature's (the rest of the file is shared).

// src/render/structures.ts:95-152 slice of src/render/structures.ts#retainingWalls (shared): the feature's statements; "…" marks code left out
function retainingWalls(
// … 2 lines
): void {
  for (const ribbon of walled) {
    // … 5 lines
    for (const side of [1, -1] as const) {
      // … 2 lines
      for (let i = 0; i <= steps; i++) {
        // … 8 lines
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
