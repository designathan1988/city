// What the feature uses of src/world/buildings/handles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/handles.ts:22-22 type HandleKind
/** Where the edit handles of a selected building stand, in world 3D. */
type HandleKind = 'storeys' | 'side' | 'move' | 'rotate' | 'scale' | 'relief' | 'vertex' | 'lift';

// src/world/buildings/handles.ts:33-44 interface Handle
export interface Handle {
  readonly kind: HandleKind;
  readonly side?: FaceId;
  readonly vertex?: number;
  readonly x: number;
  readonly y: number;
  /** Absolute height. */
  readonly z: number;
  /** World direction the handle drags along (sides: the outward normal). */
  readonly dx: number;
  readonly dy: number;
}
