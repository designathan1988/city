// What the feature uses of src/world/solids.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/solids.ts:12-14 type Solid
/** DO NOT build the list again for the whole map on a revision */
export type Solid =
  | { readonly kind: 'capsule'; readonly a: Vec2; readonly b: Vec2; readonly r: number; readonly box: Box }
  | { readonly kind: 'disc'; readonly c: Vec2; readonly r: number; readonly box: Box };

// src/world/solids.ts:15-15 type Box
type Box = readonly [number, number, number, number];

// src/world/solids.ts:27-32 interface Solids
export interface Solids {
  /** Every solid whose box comes within `reach` of (x, y). */
  near(x: number, y: number, reach: number): Solid[];
  /** What keeping it up has cost (the spine's ledger). */
  readonly work: DerivedWork;
}

// src/world/solids.ts:35-35 type SourceKey
/** A source item's key: its store and its id. */
export type SourceKey = `b${number}` | `r${number}` | `p${number}` | `l${number}`;

// src/world/solids.ts:96-142 function createSolids
/** A new solids product with its grid (made from nothing at its first refresh). */
export function createSolids(): { solids: Solids; derived: Derived<RoadDoc, SourceKey, Solid[]> };

// src/world/solids.ts:144-144 variable known
const known = new WeakMap<RoadDoc, ReturnType<typeof createSolids>>();
