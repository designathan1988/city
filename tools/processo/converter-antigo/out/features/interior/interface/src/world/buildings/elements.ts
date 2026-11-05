// What the feature uses of src/world/buildings/elements.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/elements.ts:31-31 variable RAMP_RUN_PER_RISE
/** Free parts of a building. */

/** A ramp's slope: one in twelve, the usual limit for a wheelchair ramp. */
const RAMP_RUN_PER_RISE = 12;

// src/world/buildings/elements.ts:32-32 variable MIN_ELEMENT
export const MIN_ELEMENT = m(0.1);

// src/world/buildings/elements.ts:33-33 variable MAX_ELEMENT
export const MAX_ELEMENT = m(40);

// src/world/buildings/elements.ts:36-62 variable ELEMENT_DEFAULTS
/** Sizes an element starts with, world units: [w, d, h]. */
export const ELEMENT_DEFAULTS: Readonly<Record<ElementKind, readonly [number, number, number]>> = {
  stair: [m(1.2), m(3), m(1)],
  ramp: [m(1.5), m(6), m(0.5)],
  pillar: [m(0.4), m(0.4), m(3)],
  canopy: [m(2.4), m(1.2), m(0.15)],
  wall: [m(4), m(0.25), m(1.8)],
  slab: [m(4), m(4), m(0.25)],
  pavement: [m(4), m(3), m(0.12)],
  fence: [m(2), m(0.12), m(1.1)],
  tree: [m(3), m(3), m(5)],
  bench: [m(1.6), m(0.5), m(0.45)],
  ac: [m(0.8), m(0.35), m(0.6)],
  planter: [m(1), m(1), m(0.5)],
  railing: [m(2), m(0.12), m(1.05)],
  awning: [m(3), m(1.2), m(0.12)],
  flowers: [m(1.4), m(1.4), m(0.4)],
  rocks: [m(1.6), m(1.6), m(0.7)],
  parking: [m(6), m(5), m(0.12)],
  clock: [m(3), m(0.25), m(3)],
  hedge: [m(3), m(0.7), m(1.2)],
  shrub: [m(1.4), m(1.4), m(1.2)],
  gate: [m(3), m(0.12), m(1.8)],
  bin: [m(1.4), m(0.7), m(1.1)],
  lamp: [m(0.3), m(0.3), m(4.5)],
  bollard: [m(0.2), m(0.2), m(0.9)],
  drain: [m(0.5), m(0.5), m(0.1)],
};

// src/world/buildings/elements.ts:65-65 variable ON_FACADE
/** Parts hung on a facade rather than standing on the ground. */
export const ON_FACADE: ReadonlySet<ElementKind> = new Set<ElementKind>(['canopy', 'ac', 'awning', 'clock']);

// src/world/buildings/elements.ts:85-85 function onGround
/** Whether an element's foot stands on the ground (rather than on a floor, or hung on a wall). */
export const onGround = (e: BuildingElement): boolean => …;

// src/world/buildings/elements.ts:119-127 function elementRing
/** The world ring of an element's plan. */
export function elementRing(b: Building, e: BuildingElement, grow = 0): Vec2[];

// src/world/buildings/elements.ts:130-133 function groundElements
/** World rings of the elements that stand on the ground: footprint, for the road and neighbour tests. */
export const groundElements = (b: Building, grow = 0): Vec2[][] => …;

// src/world/buildings/elements.ts:158-181 function elementClash
/** The first volume an element would stand inside of, or null. Touching is allowed. `spans`: the building's `massSpans`, when the building is unchanged since. */
export function elementClash(b: Building, e: BuildingElement, spans?: Float64Array): Volume | null;

// src/world/buildings/elements.ts:295-295 variable FACING_NORMAL
const FACING_NORMAL: Readonly<Record<Side, Vec2>> = { 0: { x: 0, y: -1 }, 1: { x: 1, y: 0 }, 2: { x: 0, y: 1 }, 3: { x: -1, y: 0 } };
