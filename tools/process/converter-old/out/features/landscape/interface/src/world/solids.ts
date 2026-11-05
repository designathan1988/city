// What the feature uses of src/world/solids.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/solids.ts:12-14 type Solid
/** DO NOT build the list again for the whole map on a revision */
export type Solid =
  | { readonly kind: 'capsule'; readonly a: Vec2; readonly b: Vec2; readonly r: number; readonly box: Box }
  | { readonly kind: 'disc'; readonly c: Vec2; readonly r: number; readonly box: Box };

// src/world/solids.ts:15-15 type Box
type Box = readonly [number, number, number, number];

// src/world/solids.ts:20-23 variable FURNITURE
/** The street furniture's reach, by kind (none: walked over). */
const FURNITURE: Readonly<Partial<Record<LandscapeKind, number>>> = {
  tree: m(0.3), shrub: m(0.45), bench: m(0.55), bin: m(0.3), lamp: m(0.15), hydrant: m(0.2),
  postbox: m(0.3), phone: m(0.5), sign: m(0.08), streetname: m(0.08),
};

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

// src/world/solids.ts:37-37 function disc
const disc = (c: Vec2, r: number): Solid => …;

// src/world/solids.ts:45-82 function solidsOfItem
/** The solids of one source item now; undefined when it is gone or has none. */
function solidsOfItem(doc: RoadDoc, key: SourceKey): Solid[] | undefined;

// src/world/solids.ts:96-142 function createSolids
/** A new solids product with its grid (made from nothing at its first refresh). */
export function createSolids(): { solids: Solids; derived: Derived<RoadDoc, SourceKey, Solid[]> };
