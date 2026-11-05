// The code of src/render/elements.ts that is the feature's (the rest of the file is shared).

// src/render/elements.ts:57-65 slice of src/render/elements.ts#EFFECTS (shared): the feature's statements; "…" marks code left out
const EFFECTS: Readonly<Record<EffectKind, { puff: PuffKind; rate: number; size: number; life: number; spread: number; rise: number }>> = {
  smoke: { puff: 2, rate: 7, size: 0.45, life: 5, spread: 0.3, rise: 0.2 },
  fire: { puff: 5, rate: 16, size: 0.45, life: 0.9, spread: 0.35, rise: 0.05 },
  steam: { puff: 3, rate: 7, size: 0.4, life: 3, spread: 0.3, rise: 0.1 },
  dust: { puff: 1, rate: 5, size: 0.5, life: 4, spread: 0.5, rise: 0.05 },
  soot: { puff: 2, rate: 6, size: 0.6, life: 7, spread: 0.3, rise: 0.4 },
  sparks: { puff: 6, rate: 12, size: 0.12, life: 1.2, spread: 0.25, rise: 0.1 },
  spray: { puff: 8, rate: 14, size: 0.15, life: 1.4, spread: 0.4, rise: 0 },
};

// src/render/elements.ts:235-245 slice of src/render/elements.ts#ElementLayer (shared): the feature's statements; "…" marks code left out
export interface ElementLayer {
  // … 6 lines
  emit(dt: number, eye: Vector3, exhaust: Exhaust): void;
// … 2 lines
}

// src/render/elements.ts:310-319 slice of src/render/elements.ts#createElementLayer.give (shared): the feature's statements; "…" marks code left out
  const give = (e: Emitter, dt: number, eye: Vector3, exhaust: Exhaust): void => {
    const { item } = e;
    if (Math.hypot(item.x - eye.x, e.z - eye.y, -item.y - eye.z) > EFFECT_REACH) return;
    const fx = EFFECTS[item.kind as EffectKind];
    e.owed += fx.rate * (item.intensity ?? 0.5) * Math.max(0.5, item.size / m(2)) * dt;
    const n = Math.floor(e.owed);
    if (n <= 0) return;
    e.owed -= n;
    exhaust.burst(item.x, item.y, e.z + item.size * fx.rise, n, fx.puff, item.size * fx.spread, item.size * fx.size, fx.life);
  };
