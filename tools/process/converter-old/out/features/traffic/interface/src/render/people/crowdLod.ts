// What the feature uses of src/render/people/crowdLod.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/crowdLod.ts:8-8 variable LEVEL_PIXELS
/** The crowd's levels of detail (`proceduralCrowd.ts`), from its own meshes. */
const LEVEL_PIXELS = [40, 20, 8, 3] as const;

// src/render/people/crowdLod.ts:10-10 variable LEVELS
/** Levels drawn: 0..3. */
export const LEVELS = 4;

// src/render/people/crowdLod.ts:12-12 variable HYSTERESIS
/** Hysteresis: a person goes back up a level only this far past its edge. */
const HYSTERESIS = 0.1;

// src/render/people/crowdLod.ts:14-14 variable LEVEL_CAPS
/** The closest levels' safety caps: past them, the people smallest on the screen go one level down */
export const LEVEL_CAPS = [30, 120] as const;

// src/render/people/crowdLod.ts:28-28 type PieceRole
/** What a piece is, for its triangle budget at each level. */
export type PieceRole = 'skin' | 'outfit' | 'garment' | 'shoes' | 'hair' | 'brows' | 'lashes' | 'mouth' | 'accessory';

// src/render/people/crowdLod.ts:31-31 variable SKIN_TRIANGLES_LEVEL0
/** The body's triangles at level 0: no hair costs more. */
const SKIN_TRIANGLES_LEVEL0 = 26756;

// src/render/people/crowdLod.ts:34-46 variable LEVEL_TRIANGLES
/** Triangles a piece keeps at each level (Infinity: all of it; 0: not drawn there). */
export const LEVEL_TRIANGLES: Readonly<Record<PieceRole, readonly [number, number, number, number]>> = {
  skin: [Infinity, 2000, 600, 0],
  outfit: [Infinity, 1200, 400, 0],
  // Trousers or a skirt worn under a separate top.
  garment: [Infinity, 900, 300, 0],
  shoes: [Infinity, 300, 80, 0],
  // Level 3 draws the class's far mesh alone, its hair a colour on it (`FarZone`): no piece of its own there.
  hair: [SKIN_TRIANGLES_LEVEL0, 1000, 300, 0],
  brows: [Infinity, 100, 0, 0],
  lashes: [Infinity, 0, 0, 0],
  mouth: [Infinity, 0, 0, 0],
  accessory: [Infinity, 200, 60, 0],
};

// src/render/people/crowdLod.ts:49-49 variable LEVEL_TOP_PIXELS
/** The tallest a person stands on the screen at each level (pixels, the hysteresis's margin in): what an item's size is held against (`itemShownAt`). */
const LEVEL_TOP_PIXELS = [Infinity, ...LEVEL_PIXELS.slice(0, -1).map((p) => p * (1 + HYSTERESIS))] as const;

// src/render/people/crowdLod.ts:51-51 variable ITEM_LEAST_PIXELS
/** Pixels across under which an item is not drawn at a level (a ring, a stud, a pin: a draw a person for nothing seen). */
const ITEM_LEAST_PIXELS = 1.5;

// src/render/people/crowdLod.ts:57-57 variable EYE_TRIANGLES
/** Of the skin's triangles, the eyes' - simplified apart: as part of the body they were the first to go. */
export const EYE_TRIANGLES = [Infinity, 200, 40, 0] as const;

// src/render/people/crowdLod.ts:59-59 variable FAR_TRIANGLES
/** The far mesh (level 3): a class's body whole, each person's colours on its zones (`FarZone`). */
export const FAR_TRIANGLES = 300;

// src/render/people/crowdLod.ts:63-63 variable FarZone
/** THE ZONES OF A FAR BODY (level 3, 3 to 8 pixels tall): each vertex of a class's far mesh is one, each person has a colour a zone - the
 *  hair's, the skin's or the garment over it (`proceduralCrowd.ts` zoneColours). One mesh draws every far person of a class. */
export const FarZone = { hair: 0, face: 1, torso: 2, pelvis: 3, upperArm: 4, forearm: 5, hand: 6, thigh: 7, calf: 8, foot: 9 } as const;

// src/render/people/crowdLod.ts:64-64 variable FAR_ZONES
export const FAR_ZONES = 10;

// src/render/people/crowdLod.ts:82-82 interface HeadFrame
/** Where a head's hair grows on a body at rest: over the brow, or round the back of the skull (`eye`: the eyes' mean height and depth, and the
 *  head's height over them). The body faces +z. */
export interface HeadFrame { readonly eyeY: number; readonly eyeZ: number; readonly above: number }

// src/render/people/crowdLod.ts:87-90 variable lodReady
export const lodReady: Promise<void> = MeshoptSimplifier.ready.catch((error: unknown) => {
  // Said, not swallowed: every piece is then drawn whole (`simplifyIndex` answers null).
  console.error('[crowd] meshoptimizer did not load; pieces are drawn unsimplified', error);
});

// src/render/people/crowdLod.ts:213-213 variable REGION
/** The region of the far mesh a vertex belongs to (`proceduralCrowd.ts`) */
export const REGION = { skin: 0, outfit: 1, shoes: 2, garment: 3 } as const;

// src/render/people/crowdLod.ts:272-272 variable sortedWeights
const sortedWeights = new WeakSet<object>();
