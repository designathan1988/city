// What the feature uses of src/main.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/main.ts:854-860 variable transitEditor
/** The public transport tool (`editor/transitTools.ts`): each edit one undo step. */
const transitEditor = new TransitTool({
  doc: () => doc,
  net: () => net,
  edit: (next) => mutate(() => { doc.setTransit(next); return true; }),
  hint: (key) => flashHint(key),
  redraw: () => requestDraw(),
});

// src/main.ts:2960-2960 function drawOverlayScreen.at
const at = (p: Vec2): Vec2 => …;
