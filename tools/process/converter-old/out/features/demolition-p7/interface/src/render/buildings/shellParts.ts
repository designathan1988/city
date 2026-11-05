// What the feature uses of src/render/buildings/shellParts.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/shellParts.ts:11-11 type ShellPartKind
/** THE SHELL'S PARTS: THE LOTS' AND ELEMENTS' BOXES, DRAWN INSTANCED WITH THE SHELL'S OWN LOOK */
// A gate's bars, a fence's posts and rails, a bin, a crate: a box each, once laid in the shell face by face (24 vertices,
// ~1 KB), now one instance of a unit model (88 bytes) drawn by the shell's material (`finishes.ts`), which lays on it what
// the shell laid on each vertex (`buildingMesh.ts` `Shell.face`): the planar uv of its finish, the tone and the grime.

/** The kinds drawn by the shell's material: a box (sides and top), a fence's rail (sides, top and bottom), a paving (a box whose sides stand in),
 *  a box whose top is covered (its four sides: a wall's body under its coping, a post under its head, a bar between a gate's rails). */
export type ShellPartKind = 'lotBox' | 'lotRail' | 'lotSlab' | 'lotSides';

// src/render/buildings/shellParts.ts:12-12 variable SHELL_PART_KINDS
export const SHELL_PART_KINDS: readonly ShellPartKind[] = Object.freeze(['lotBox', 'lotRail', 'lotSlab', 'lotSides']);

// src/render/buildings/shellParts.ts:16-16 variable LOOK_FLOATS
/** Numbers an instance of a shell part carries besides its matrix and colour: its finish id, its wear and the ground its walls darken towards. */
export const LOOK_FLOATS = 3;

// src/render/buildings/shellParts.ts:26-26 variable NO_GROUND
/** A ground so far down no wall darkens: what a part of a building with no ground floor takes (`Shell.ground` NaN). */
export const NO_GROUND = -1e30;

// src/render/buildings/shellParts.ts:33-33 variable COVER_INSET
/** How far a paving's sides stand in from its top's edge (`Emitter.slab`): a wall or a gate on that edge keeps the face there. */
export const COVER_INSET = m(0.006);

// src/render/buildings/shellParts.ts:87-87 type Unit
/** A point of a shell part's model in its own plan: along (u, the building's x), across (v, its y) and up (w), each -0.5..0.5. */
type Unit = readonly [number, number, number];

// src/render/buildings/shellParts.ts:90-94 interface ShellPartFace
/** A face of a shell part's model, as `Emitter.box` (or `emitFence`) laid it: its corners in order and its normal; `inset`: a paving's side (`COVER_INSET`). */
export interface ShellPartFace {
  readonly points: readonly Unit[];
  readonly n: Unit;
  readonly inset: boolean;
}

// src/render/buildings/shellParts.ts:96-96 variable H
const H = 0.5;

// src/render/buildings/shellParts.ts:98-104 variable BOX
/** `Emitter.box`'s faces from (-0.5, -0.5, -0.5) to (0.5, 0.5, 0.5), in its order: the four sides, then the top (the bottom is never seen). */
const BOX: readonly Omit<ShellPartFace, 'inset'>[] = [
  { points: [[-H, -H, -H], [H, -H, -H], [H, -H, H], [-H, -H, H]], n: [0, -1, 0] },
  { points: [[H, H, -H], [-H, H, -H], [-H, H, H], [H, H, H]], n: [0, 1, 0] },
  { points: [[H, -H, -H], [H, H, -H], [H, H, H], [H, -H, H]], n: [1, 0, 0] },
  { points: [[-H, H, -H], [-H, -H, -H], [-H, -H, H], [-H, H, H]], n: [-1, 0, 0] },
  { points: [[-H, -H, H], [H, -H, H], [H, H, H], [-H, H, H]], n: [0, 0, 1] },
];

// src/render/buildings/shellParts.ts:107-118 variable SHELL_PART_FACES
/** Each shell part's model, face by face: the box; the rail of `emitFence` (its two sides, top and bottom, along u, no ends); the paving of `Emitter.slab`. */
export const SHELL_PART_FACES: Readonly<Record<ShellPartKind, readonly ShellPartFace[]>> = Object.freeze({
  lotBox: BOX.map((f) => ({ ...f, inset: false })),
  lotRail: [
    { points: [[-H, -H, -H], [H, -H, -H], [H, -H, H], [-H, -H, H]], n: [0, -1, 0], inset: false },
    { points: [[H, H, -H], [-H, H, -H], [-H, H, H], [H, H, H]], n: [0, 1, 0], inset: false },
    { points: [[-H, -H, H], [H, -H, H], [H, H, H], [-H, H, H]], n: [0, 0, 1], inset: false },
    { points: [[-H, H, -H], [H, H, -H], [H, -H, -H], [-H, -H, -H]], n: [0, 0, -1], inset: false },
  ],
  lotSlab: BOX.map((f) => ({ ...f, inset: f.n[2] === 0 })),
  // DO NOT draw a top another part lies on: two triangles an instance no one sees, coplanar with the part over it.
  lotSides: BOX.filter((f) => f.n[2] === 0).map((f) => ({ ...f, inset: false })),
});
