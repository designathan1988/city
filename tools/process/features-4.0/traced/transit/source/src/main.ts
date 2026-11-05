// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:854-860 slice of src/main.ts#transitEditor (shared): the feature's statements; "…" marks code left out
const transitEditor = new TransitTool({
  doc: () => doc,
  net: () => net,
  edit: (next) => mutate(() => { doc.setTransit(next); return true; }),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:861-861 slice of src/main.ts#<ExpressionStatement>@861 (shared): the feature's statements; "…" marks code left out
setTransitTool(transitEditor);

// src/main.ts:1207-1212 slice of src/main.ts#doubleClick (shared): the feature's statements; "…" marks code left out
function doubleClick(e: MouseEvent, r: DOMRect): void {
  // … 1 lines
  if (game.tool === 'transit') { transitEditor.key('Enter'); return; }
// … 2 lines
}

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 34 lines
  switch (game.tool) {
    // … 39 lines
    case 'transit':
      // … 1 lines
      transitEditor.click(world, e.shiftKey, e.detail >= 2);
      break;
  // … 98 lines
  }
// … 1 lines
}

// src/main.ts:1404-1475 slice of src/main.ts#pointerMove (shared): the feature's statements; "…" marks code left out
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void {
  // … 34 lines
  if (game.tool === 'transit') {
    transitEditor.move(world);
  // … 1 lines
  }
// … 32 lines
}

// src/main.ts:1859-1946 slice of src/main.ts#mountInterface (shared): the feature's statements; "…" marks code left out
function mountInterface(): void {
  // … 1 lines
  shell = mountShell({
    // … 50 lines
    transitTool,
  // … 16 lines
  });
// … 16 lines
}

// src/main.ts:2479-2555 slice of src/main.ts#gameKeys (shared): the feature's statements; "…" marks code left out
const gameKeys = createGameKeys({
  // … 3 lines
  transitKey: (key) => transitEditor.key(key),
// … 71 lines
});

// src/main.ts:2748-2750 slice of src/main.ts#simShowsMotion (shared): the feature's statements; "…" marks code left out
function simShowsMotion(): boolean {
  return sim.vehicles.size > 0 || sim.pedViews.length > 0 || (sim.stats?.lotsMoving ?? 0) > 0 || (sim.stats?.controllers ?? 0) > 0 || hasTransit(doc.transit);
}

// src/main.ts:2942-3410 slice of src/main.ts#drawOverlayScreen (shared): the feature's statements; "…" marks code left out
function drawOverlayScreen(): void {
  // … 66 lines
  if (game.tool === 'transit') transitEditor.draw(ctx, at, true);
  // … 2 lines
  scene.setTransitPreview(game.tool === 'transit' ? transitEditor.preview() : null);
// … 397 lines
}

// src/main.ts:4036-4231 slice of src/main.ts#<IfStatement>@4036 (shared): the feature's statements; "…" marks code left out
if (PROBES) (window as unknown as { __roadcraft: unknown }).__roadcraft = {
  // … 26 lines
  transit: transitEditor,
  // … 34 lines
  transitTool: () => transitEditor,
// … 132 lines
};
