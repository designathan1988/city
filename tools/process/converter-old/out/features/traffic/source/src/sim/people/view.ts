// The code of src/sim/people/view.ts that is the feature's (the rest of the file is shared).

// src/sim/people/view.ts:113-115 function gaitSpeed: the feature's own
export function gaitSpeed(view: Pick<PedView, 'x' | 'y' | 'prev' | 'v'>): number {
  return Math.max(view.v, Math.hypot(view.x - view.prev.x, view.y - view.prev.y) / DT);
}
