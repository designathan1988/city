// The code of src/view/viewport.ts that is the feature's (the rest of the file is shared).

// src/view/viewport.ts:52-104 slice of src/view/viewport.ts#flatViewport (shared): the feature's statements; "…" marks code left out
export function flatViewport(camera: Camera): Viewport {
  return {
    // … 33 lines
    get centre() {
      return { x: camera.x, y: camera.y };
    },
    // … 4 lines
    get zoom() {
      return camera.zoom;
    },
    // … 3 lines
    get zoomBounds() {
      return { min: MIN_ZOOM, max: MAX_ZOOM };
    },
  };
}

// src/view/viewport.ts:55-55 slice of src/view/viewport.ts#flatViewport.toWorld (shared): the feature's statements; "…" marks code left out
    toWorld: (px, py, cssW, cssH) => camera.screenToWorld(px, py, cssW, cssH),

// src/view/viewport.ts:57-57 slice of src/view/viewport.ts#flatViewport.toWorldAt (shared): the feature's statements; "…" marks code left out
    toWorldAt: (px, py, _height, cssW, cssH) => camera.screenToWorld(px, py, cssW, cssH),

// src/view/viewport.ts:58-58 slice of src/view/viewport.ts#flatViewport.toScreen (shared): the feature's statements; "…" marks code left out
    toScreen: (p, cssW, cssH) => camera.worldToScreen(p, cssW, cssH),

// src/view/viewport.ts:59-59 method flatViewport.holds: the feature's own
    holds: () => true,

// src/view/viewport.ts:60-64 slice of src/view/viewport.ts#flatViewport.panTo (shared): the feature's statements; "…" marks code left out
    panTo(grabbed, px, py, cssW, cssH) {
      const now = camera.screenToWorld(px, py, cssW, cssH);
      camera.x += grabbed.x - now.x;
      camera.y += grabbed.y - now.y;
    },

// src/view/viewport.ts:65-67 slice of src/view/viewport.ts#flatViewport.zoomAt (shared): the feature's statements; "…" marks code left out
    zoomAt(px, py, factor, cssW, cssH) {
      camera.zoomAt(px, py, factor, cssW, cssH);
    },

// src/view/viewport.ts:68-72 method flatViewport.slide: the feature's own
    slide(right, forward) {
      // Screen up is -y on the flat canvas.
      camera.x += right;
      camera.y -= forward;
    },

// src/view/viewport.ts:90-93 slice of src/view/viewport.ts#flatViewport.moveTo (shared): the feature's statements; "…" marks code left out
    moveTo(p) {
      camera.x = p.x;
      camera.y = p.y;
    },
