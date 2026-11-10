// The code of src/sim/world.ts that is the feature's (the rest of the file is shared).

// src/sim/world.ts:742-747 method SimWorld.inFocusReach: the feature's own
  inFocusReach(x: number, y: number, pad = 0): boolean {
    const focus = this.focus;
    if (!this.microWindow || !focus) return true;
    const r = Math.min(FOCUS_WINDOW, Math.max(focus.r * 1.35, m(200)) * 1.15 + m(100)) + pad;
    return Math.abs(x - focus.x) <= r && Math.abs(y - focus.y) <= r;
  }
