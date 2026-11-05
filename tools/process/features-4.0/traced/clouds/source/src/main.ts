// The code of src/main.ts that is the feature's (the rest of the file is shared).

// src/main.ts:1056-1073 slice of src/main.ts#currentGesture (shared): the feature's statements; "…" marks code left out
function currentGesture(): string | null {
  // … 13 lines
  const cloud = cloudTool.gesture();
  if (cloud) return cloud;
// … 1 lines
}

// src/main.ts:1137-1150 slice of src/main.ts#cloudTool (shared): the feature's statements; "…" marks code left out
const cloudTool = new CloudTool({
  doc,
  mode: () => cloudMode(),
  brush: () => cloudBrush(),
  drift: () => scene.cloudDrift(),
  rayAt: (px, py, height) => view.toWorldAt(px, py, height, surface.cssW, surface.cssH),
  record: () => { history.begin(doc); cloudStepOpen = true; },
  changed: () => {
    updateHistoryButtons();
    persistence.saveSessionSoon(doc, sessionSettings);
    requestDraw();
  },
  hint: (key) => flashHint(key),
});

// src/main.ts:1217-1396 slice of src/main.ts#pointerDown (shared): the feature's statements; "…" marks code left out
function pointerDown(e: PointerEvent, r: DOMRect): void {
  // … 34 lines
  switch (game.tool) {
    // … 24 lines
    case 'terrain':
      if (game.terrainMode === 'cloud') cloudTool.down(e.pointerId, e.clientX - r.left, e.clientY - r.top);
      // The weather tool: a click calls a lightning bolt down there.
      else if (game.terrainMode === 'weather') scene.strikeAt(world.x, world.y);
      else terrainBrush.begin(e.pointerId, world);
      break;
  // … 111 lines
  }
// … 1 lines
}

// src/main.ts:1404-1475 slice of src/main.ts#pointerMove (shared): the feature's statements; "…" marks code left out
function pointerMove(e: PointerEvent, r: DOMRect, path: PointerSample[] | undefined): void {
  // … 53 lines
  if (cloudTool.pointer === e.pointerId) {
    const rect = canvas.getBoundingClientRect();
    cloudTool.move(e.clientX - rect.left, e.clientY - rect.top);
    return;
  }
// … 12 lines
}

// src/main.ts:1484-1513 slice of src/main.ts#endPointer (shared): the feature's statements; "…" marks code left out
function endPointer(e: PointerEvent): void {
  // … 8 lines
  cloudTool.up(e.pointerId);
// … 19 lines
}

// src/main.ts:1703-1711 slice of src/main.ts#<IfStatement>@1703 (shared): the feature's statements; "…" marks code left out
if (atmosphere.atmoClouds > 0) {
  if (doc.clouds.length === 0) {
    doc.addClouds(scatterClouds(Math.round((atmosphere.atmoClouds / 100) * 12), {
      size: Math.max(40, atmosphere.atmoCloudThickness * UNITS_PER_METER), height: atmosphere.atmoCloudBase * UNITS_PER_METER, density: 0.8,
    }, 0.3, CLOUD_SPAN / 2, [], editRandom(doc, 0, 0)));
  // … 1 lines
  }
// … 1 lines
}

// src/main.ts:1798-1817 slice of src/main.ts#MAP_ACTIONS (shared): the feature's statements; "…" marks code left out
const MAP_ACTIONS: Readonly<Record<string, () => void>> = {
  // … 7 lines
  clearClouds: () => {
    if (doc.clouds.length > 0) wholeMapEdit('confirm.clearClouds', () => { for (const cloud of [...doc.clouds]) doc.removeCloud(cloud.id); });
  },
  // … 1 lines
  scatterClouds: () => wholeMapEdit(null, () => {
    // … 1 lines
    const laid = scatterClouds(cloudScatter.count, {
      size: brush.size * UNITS_PER_METER, height: brush.height * UNITS_PER_METER, density: brush.density / 100,
    }, cloudScatter.variation / 100, CLOUD_SPAN / 2, doc.clouds, editRandom(doc, 0, 0));
    if (doc.addClouds(laid) < cloudScatter.count) flashHint('hint.cloud.full');
  }),
};
