// What the feature uses of src/render/people/heldProps.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/heldProps.ts:10-10 type HeldKind
/** What people hold while they do something (`sim/people` gestures) */
export type HeldKind = 'newspaper' | 'cup' | 'phone' | 'camera' | 'bag' | 'umbrella' | 'box';

// src/render/people/heldProps.ts:13-20 variable HELD
/** Which hand holds what each gesture shows, and what it is. */
export const HELD: Readonly<Partial<Record<string, { kind: HeldKind; left?: boolean }>>> = {
  read: { kind: 'newspaper' }, // a book, held as the phone is (`citizenGait` plays the phone clip)
  drink: { kind: 'cup' },
  phone: { kind: 'phone' },
  photo: { kind: 'camera' },
  bag: { kind: 'bag' },
  umbrella: { kind: 'umbrella' },
};

// src/render/people/heldProps.ts:81-85 variable UPRIGHT
/** Things that hang or stand by their own weight - a cup, a bag, an umbrella */
const UPRIGHT: Partial<Record<HeldKind, readonly [number, number, number]>> = {
  cup: [0.07, 0.0, -0.05],
  bag: [0.08, 0.0, -0.02],
  umbrella: [0.06, 0.0, -0.03],
};

// src/render/people/heldProps.ts:87-87 variable UP
const UP = new Vector3(0, 1, 0);

// src/render/people/heldProps.ts:89-89 variable CAPACITY
const CAPACITY = 300;

// src/render/people/heldProps.ts:91-99 interface HeldProps
export interface HeldProps {
  readonly group: Group;
  begin(): void;
  /** One `kind` in the hand whose matrix (model to world) is `hand`, on a body drawn at `scale` world units per metre. */
  place(kind: HeldKind, hand: Matrix4, scale: number): void;
  /** One `kind` drawn with exactly this matrix (model to world): a box held in both hands. */
  placeMatrix(kind: HeldKind, matrix: Matrix4): void;
  dispose(): void;
}

// src/render/people/heldProps.ts:101-174 function createHeldProps
export function createHeldProps(): HeldProps;
