// What the feature uses of src/render/elements.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/elements.ts:57-65 variable EFFECTS
/** Effects: the particle each gives off, how many a second at full intensity, a puff's size (times the element's) and life. */
const EFFECTS: Readonly<Record<EffectKind, { puff: PuffKind; rate: number; size: number; life: number; spread: number; rise: number }>> = {
  smoke: { puff: 2, rate: 7, size: 0.45, life: 5, spread: 0.3, rise: 0.2 },
  fire: { puff: 5, rate: 16, size: 0.45, life: 0.9, spread: 0.35, rise: 0.05 },
  steam: { puff: 3, rate: 7, size: 0.4, life: 3, spread: 0.3, rise: 0.1 },
  dust: { puff: 1, rate: 5, size: 0.5, life: 4, spread: 0.5, rise: 0.05 },
  soot: { puff: 2, rate: 6, size: 0.6, life: 7, spread: 0.3, rise: 0.4 },
  sparks: { puff: 6, rate: 12, size: 0.12, life: 1.2, spread: 0.25, rise: 0.1 },
  spray: { puff: 8, rate: 14, size: 0.15, life: 1.4, spread: 0.4, rise: 0 },
};

// src/render/elements.ts:67-67 variable EFFECT_REACH
/** Effects farther than this from the camera give off nothing (a budget for the one particle pool). */
const EFFECT_REACH = m(720);

// src/render/elements.ts:230-233 interface ElementSource
/** What the layer reads of the document: every element (a whole build only) and the elements by place. */
export interface ElementSource {
  readonly elements: readonly ElementItem[];
  elementsIn(rect: ChangeRect): ElementItem[];
}

// src/render/elements.ts:235-245 interface ElementLayer
export interface ElementLayer {
  readonly group: Group;
  /** The cells (`cellKeeps.ts` keep cells) made again from the elements standing in them (null: every cell); whether a prop came or went. */
  sync(cells: ReadonlySet<number> | null, source: ElementSource, groundAt: (x: number, y: number) => number): boolean;
  /** Keeps the instances the camera can see (`InstanceRuns.cull`). */
  cull(frustum: Frustum, view: Matrix4): void;
  /** Gives off the effects' particles near the camera (three's x, y, z), `dt` seconds on. */
  emit(dt: number, eye: Vector3, exhaust: Exhaust): void;
  readonly hasEffects: boolean;
  dispose(): void;
}

// src/render/elements.ts:270-270 interface Emitter
/** An effect giving off its particles: where it stands, and the particles owed since the last frame. */
interface Emitter { readonly item: ElementItem; readonly z: number; owed: number }
