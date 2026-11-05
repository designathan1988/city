// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:488-498 slice of src/main.ts#poleTool (shared): the feature's statements; "…" marks code left out
const poleTool = new PoleTool({
  doc,
  net,
  zoom: () => view.zoom,
  mode: () => poleToolMode(),
  lamps: () => poleLampMode(),
  inHand: () => game.tool === 'pole',
  mutate: (fn) => mutateBuilt(fn),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:500-500 slice of src/main.ts#framePolePlan (shared): the feature's statements; "…" marks code left out
let framePolePlan: PoleRunPlan | null = null;

// src/main.ts:1041-1053 slice of src/main.ts#cancelGestures (shared): the feature's statements; "…" marks code left out
function cancelGestures(): void {
  // … 1 lines
  poleTool.cancel();
// … 9 lines
}

// src/main.ts:1056-1073 slice of src/main.ts#currentGesture (shared): the feature's statements; "…" marks code left out
function currentGesture(): string | null {
  // … 7 lines
  const pole = poleTool.gesture();
  if (pole) return pole;
// … 7 lines
}

// src/main.ts:1117-1119 slice of src/main.ts#gestureInProgress (shared): the feature's statements; "…" marks code left out
function gestureInProgress(): boolean {
  return roadTool.inProgress() || poleTool.inProgress() || terrainBrush.stroking || mover.dragging || lotTool.gesture() !== null || bulldozer.inProgress();
}

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 34 lines
  switch (game.tool) {
    // … 52 lines
    case 'pole':
      // … 1 lines
      poleTool.down(world, e.shiftKey);
      break;
  // … 85 lines
  }
// … 1 lines
}

// src/main.ts:1404-1475 slice of src/main.ts#pointerMove (shared): the feature's statements; "…" marks code left out
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void {
  // … 39 lines
  if (poleTool.move(world)) return;
// … 30 lines
}

// src/main.ts:1480-1482 slice of src/main.ts#poleReach (shared): the feature's statements; "…" marks code left out
function poleReach(): number {
  return poleTool.reach();
}

// src/main.ts:1484-1513 slice of src/main.ts#endPointer (shared): the feature's statements; "…" marks code left out
function endPointer(e: PointerEvent): void {
  // … 17 lines
  poleTool.up(!cancelled && !wasPinching);
// … 10 lines
}

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 55 lines
  framePolePlan = poleTool.plan();
  scene.setPolePreview(net, framePolePlan && !framePolePlan.refused && framePolePlan.poles.length >= 2
    ? { poles: framePolePlan.poles.map((pole) => ({ x: pole.at.x, y: pole.at.y, lamp: pole.lamp, standing: pole.existing !== null })) }
    : null);
// … 41 lines
}

// src/main.ts:2849-2898 slice of src/main.ts#drawPolePlan (shared): the feature's statements; "…" marks code left out
function drawPolePlan(
  plan: PoleRunPlan | null,
// … 2 lines
): void {
  // … 21 lines
  if (game.tool !== 'pole' && !plan) return;
  const marks = poleTool.marks();
  // … 1 lines
  if (game.tool === 'pole' && poleToolMode() === 'remove') {
    if (marks.removing) ring(at(marks.removing), '#e5534b', 10);
  // … 1 lines
  }
  if (plan?.refused) {
    // An end off the footways: a red cross where it would have gone.
    cross(at(plan.from.kind === 'free' ? plan.from.at : plan.to.at));
    return;
  }
  if (plan && plan.poles.length) {
    // The run itself is drawn in 3D as it will stand (`setPolePreview`)
    for (const pole of plan.poles) if (pole.existing !== null) ring(at(pole.at), HOVER, 9);
    return;
  }
  // … 1 lines
  if (marks.first) {
    const snap = marks.first;
    if (snap.kind === 'free') cross(at(snap.at));
    else ring(at(snap.at), snap.kind === 'pole' ? HOVER : SELECTION);
  }
}

// src/main.ts:2942-3410 slice of src/main.ts#drawOverlayScreen (shared): the feature's statements; "…" marks code left out
function drawOverlayScreen(): void {
  // … 45 lines
  drawPolePlan(framePolePlan, ctx, at);
// … 421 lines
}

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 179 lines
  utilities: {
    plan: (from: Vec2, to: Vec2, reach = POLE_PICK_PIXELS / view.zoom) =>
      planPoleRun(doc, net, from, to, reach),
    run: (from: Vec2, to: Vec2, reach = POLE_PICK_PIXELS / view.zoom) => {
      const plan = planPoleRun(doc, net, from, to, reach);
      return mutateBuilt(() => commitPoleRun(doc, plan));
    },
  },
// … 7 lines
};
