// What the feature uses of src/main.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/main.ts:553-559 variable cameraHand
/** The camera moved by hand: the pointers, a pan, an orbit, a pinch (`view/cameraGestures.ts`). */
const cameraHand = new CameraGestures({
  view: () => view,
  size: () => ({ w: surface.cssW, h: surface.cssH }),
  orbited: () => persistence.saveSettingsSoon(sessionSettings),
  redraw: () => requestDraw(),
  heightUnder: (at) => groundHeightUnder(at),
});

// src/main.ts:563-567 variable cameraMotion
/** The keys held and the wheel's notches, spent as a glide frame by frame (`view/cameraMotion.ts`). */
const cameraMotion = new CameraMotion({
  view: () => view,
  size: () => ({ w: surface.cssW, h: surface.cssH }),
  heightUnder: (at) => groundHeightUnder(at),
});
