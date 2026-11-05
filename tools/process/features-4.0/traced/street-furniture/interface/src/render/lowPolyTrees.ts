// What the feature uses of src/render/lowPolyTrees.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/lowPolyTrees.ts:31-31 type LowPolyKind
export type LowPolyKind = 'oak' | 'broadleafTall' | 'cypress' | 'palm' | 'ipeYellow' | 'ipePink' | 'bush' | 'bushFlowering' | 'hedge';

// src/render/lowPolyTrees.ts:34-37 interface LowPolyParts
/** A tree's two parts: its body (trunk, inner crown) and its foliage cards (a palm's: its frond strips). */
export interface LowPolyParts {
  readonly body: BufferGeometry;
  readonly cards: BufferGeometry | null;
}

// src/render/lowPolyTrees.ts:461-473 function lowPolyTreeParts
/** A tree or bush of this kind, grown from `seed` (the same seed, the same model): its body and its foliage cards. */
export function lowPolyTreeParts(kind: LowPolyKind, seed: number): LowPolyParts;
