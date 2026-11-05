// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:393-430 slice of src/main.ts#gameState (shared): the feature's statements; "…" marks code left out
const gameState = new GameState({
  // … 19 lines
  barrierKind: 'fence' as BarrierKind,
// … 16 lines
}, () => { if (booted) requestDraw(); });

// src/main.ts:458-486 slice of src/main.ts#roadTool (shared): the feature's statements; "…" marks code left out
const roadTool = new RoadTool({
  // … 4 lines
  settings: () => ({
    // … 5 lines
    furniture: furnitureChosen(),
  }),
// … 15 lines
});

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

// src/main.ts:515-528 slice of src/main.ts#barrierTool (shared): the feature's statements; "…" marks code left out
const barrierTool = new BarrierTool({
  net: () => net,
  kind: () => game.barrierKind,
  removeAt: (at) => {
    // The runs round the point only (`RoadDoc.barrierNear`), not every wall of the map.
    const hit = doc.barrierNear(at, BARRIER_PICK_PIXELS / view.zoom);
    if (!hit) return false;
    mutate(() => doc.removeBarrier(hit.id));
    return true;
  },
  build: (kind, path) => mutate(() => doc.addBarrier(kind, path) !== null),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:1041-1053 slice of src/main.ts#cancelGestures (shared): the feature's statements; "…" marks code left out
function cancelGestures(): void {
  // … 1 lines
  poleTool.cancel();
  barrierTool.cancel();
// … 8 lines
}

// src/main.ts:1056-1073 slice of src/main.ts#currentGesture (shared): the feature's statements; "…" marks code left out
function currentGesture(): string | null {
  // … 7 lines
  const pole = poleTool.gesture();
  if (pole) return pole;
  const barrier = barrierTool.gesture();
  if (barrier) return barrier;
// … 5 lines
}

// src/main.ts:1117-1119 slice of src/main.ts#gestureInProgress (shared): the feature's statements; "…" marks code left out
function gestureInProgress(): boolean {
  return roadTool.inProgress() || poleTool.inProgress() || terrainBrush.stroking || mover.dragging || lotTool.gesture() !== null || bulldozer.inProgress();
}

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 34 lines
  switch (game.tool) {
    // … 44 lines
    case 'barrier':
      barrierTool.down(world, e.shiftKey, e.clientX, e.clientY, e.detail);
      break;
    // … 5 lines
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
  // … 33 lines
  if (game.tool === 'barrier') barrierTool.move(world);
  // … 5 lines
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

// src/main.ts:1859-1946 slice of src/main.ts#mountInterface (shared): the feature's statements; "…" marks code left out
function mountInterface(): void {
  // … 1 lines
  shell = mountShell({
    // … 34 lines
    setBarrierKind: (kind) => { if ((BARRIER_KINDS as readonly string[]).includes(kind)) gameState.set('barrierKind', kind as BarrierKind, 'fence type'); },
  // … 32 lines
  });
// … 16 lines
}

// src/main.ts:2479-2555 slice of src/main.ts#gameKeys (shared): the feature's statements; "…" marks code left out
const gameKeys = createGameKeys({
  // … 4 lines
  barrierKey: (key) => barrierTool.key(key),
// … 70 lines
});

// src/main.ts:2644-2745 slice of src/main.ts#frame (shared): the feature's statements; "…" marks code left out
function frame(now: number): void {
  // … 55 lines
  framePolePlan = poleTool.plan();
  scene.setPolePreview(net, framePolePlan && !framePolePlan.refused && framePolePlan.poles.length >= 2
    ? { poles: framePolePlan.poles.map((pole) => ({ x: pole.at.x, y: pole.at.y, lamp: pole.lamp, standing: pole.existing !== null })) }
    : null);
// … 41 lines
}

// src/main.ts:2786-2805 slice of src/main.ts#drawBarrierPlan (shared): the feature's statements; "…" marks code left out
function drawBarrierPlan(ctx: CanvasRenderingContext2D, at: (p: Vec2) => Vec2): void {
  const { line: points, bad, dots } = barrierTool.plan();
  // … 1 lines
  if (points.length >= 2) {
    ctx.strokeStyle = bad ? '#ff6f63' : SELECTION;
    ctx.lineWidth = game.barrierKind === 'hedge' ? 5 : game.barrierKind === 'wall' ? 4 : 2.5;
    ctx.lineJoin = 'round';
    ctx.beginPath();
    points.forEach((p, i) => { const s = at(p); if (i === 0) ctx.moveTo(s.x, s.y); else ctx.lineTo(s.x, s.y); });
    ctx.stroke();
  }
  // … 1 lines
  for (const p of dots) {
    const s = at(p);
    ctx.beginPath();
    ctx.arc(s.x, s.y, 3.5, 0, Math.PI * 2);
    ctx.fill();
  }
// … 1 lines
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

// src/main.ts:3573-3818 slice of src/main.ts#showInspector (shared): the feature's statements; "…" marks code left out
function showInspector(): void {
  openInspector(
    // … 4 lines
    {
      // … 104 lines
      onAddCrossing: (id, kind) => {
        // … 4 lines
        mutate(() => {
          const result = commitPedestrianCrossing(doc, net, id, kind, chosen);
          if (!result.committed) {
            flashHint(`hint.crossing.${result.reason}`);
            return false;
          }
          placed = result.node;
        // … 1 lines
        });
        if (placed !== null) {
          select(null, null, placed, 'point inserted on the road');
          showInspector();
        }
      },
      onAddUturn: (id, toward) => {
        // … 3 lines
        mutateRoads((doc, net) => {
          const result = commitUturn(doc, net, id, toward, chosen);
          if (!result.committed) {
            flashHint(`hint.uturn.${result.reason}`);
            return false;
          }
          placed = result.node;
        // … 1 lines
        });
        if (placed !== null) flashHint('hint.uturn.placed');
      },
    // … 98 lines
    },
  );
}

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 91 lines
  inventory: (options: { text?: boolean; top?: number } = {}) => {
    // … 2 lines
    const counts = {
      doc: {
        nodes: doc.nodes.size, segments: doc.segments.size, buildings: doc.buildings.size, lots: doc.lots.length, poles: doc.poles.size,
        landscape: doc.landscape.size, barriers: doc.barriers.size, trees: doc.trees.length, zones: doc.zones.length, people: doc.people.length,
      },
    // … 3 lines
    };
  // … 4 lines
  },
  // … 71 lines
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
