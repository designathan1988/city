// The code of src/world/turnChoice.ts that is the feature's (the rest of the file is shared).

// src/world/turnChoice.ts:6-12 slice of src/world/turnChoice.ts#chooseTurnOf (shared): the feature's statements; "…" marks code left out
export function chooseTurnOf(ask: TurnAsk, rings: SurfaceRings, surfaces: Map<string, JunctionSurface>): ChosenTurn {
  let surface = surfaces.get(ask.surfaceKey);
  if (!surface) surfaces.set(ask.surfaceKey, (surface = new JunctionSurface(rings)));
  const { choice, body } = chooseTurn(Polyline.fromFlat(ask.inXY), Polyline.fromFlat(ask.outXY), surface,
    ask.waiting.map((xy) => ({ centre: Polyline.fromFlat(xy) })), { fromRoadEnd: ask.fromRoadEnd, toRoadEnd: ask.toRoadEnd });
  return { choice, maxBodyClass: body };
}
