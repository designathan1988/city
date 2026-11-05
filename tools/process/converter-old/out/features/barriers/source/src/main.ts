// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:393-430 slice of src/main.ts#gameState (shared): the feature's statements; "…" marks code left out
const gameState = new GameState({
  // … 19 lines
  barrierKind: 'fence' as BarrierKind,
// … 16 lines
}, () => { if (booted) requestDraw(); });

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
  // … 2 lines
  barrierTool.cancel();
// … 8 lines
}

// src/main.ts:1056-1073 slice of src/main.ts#currentGesture (shared): the feature's statements; "…" marks code left out
function currentGesture(): string | null {
  // … 9 lines
  const barrier = barrierTool.gesture();
  if (barrier) return barrier;
// … 5 lines
}

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 34 lines
  switch (game.tool) {
    // … 44 lines
    case 'barrier':
      barrierTool.down(world, e.shiftKey, e.clientX, e.clientY, e.detail);
      break;
  // … 94 lines
  }
// … 1 lines
}

// src/main.ts:1404-1475 slice of src/main.ts#pointerMove (shared): the feature's statements; "…" marks code left out
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void {
  // … 33 lines
  if (game.tool === 'barrier') barrierTool.move(world);
// … 36 lines
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

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 91 lines
  inventory: (options: { text?: boolean; top?: number } = {}) => {
    // … 2 lines
    const counts = {
      doc: {
        // … 1 lines
        landscape: doc.landscape.size, barriers: doc.barriers.size, trees: doc.trees.length, zones: doc.zones.length, people: doc.people.length,
      },
    // … 3 lines
    };
  // … 4 lines
  },
// … 86 lines
};
