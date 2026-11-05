// What the feature uses of src/render/people/personVariants.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/personVariants.ts:4-4 type Kind
/** The kind of piece a procedural person is drawn in (`proceduralCrowd.ts` kindOf): skin, a garment, hair, brows and lashes. */
export type Kind = 'skin' | 'cloth' | 'hair' | 'face';

// src/render/people/personVariants.ts:7-13 interface Variant
/** What a program draws: the kind of piece (or the far mesh), its level, and whether its cards are widened. */
export interface Variant {
  readonly kind: Kind | 'far';
  readonly level: number;
  readonly widen: boolean;
  readonly grown: boolean;
  readonly lash: boolean;
}

// src/render/people/personVariants.ts:34-36 function castsAt
/** Whether a piece casts a real shadow at `level`: close up only, cast by the next level's triangles; not the brows', lashes' or mouth's. */
export function castsAt(kind: Kind, role: PieceRole, level: number): boolean;
