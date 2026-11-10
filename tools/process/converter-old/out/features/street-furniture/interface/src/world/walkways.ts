// What the feature uses of src/world/walkways.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/walkways.ts:24-24 type WalkwayKind
/** WHERE PEOPLE WALK, as lanes: the pedestrian network of the city. */

type WalkwayKind = 'footway' | 'corner' | 'crossing';

// src/world/walkways.ts:32-54 interface Walkway
export interface Walkway {
  readonly id: number;
  readonly kind: WalkwayKind;
  /** Centreline, from node `a` to node `b`. People walk it either way. */
  readonly path: Polyline;
  /** Usable width, world units: the lateral room a walker keeps within (`hi - lo`). */
  readonly width: number;
  /** How far a walker's centreline position may lie either side of the path, u, measured along the path's LEFT normal */
  readonly lo: number;
  readonly hi: number;
  /** For a footway or corner, the side the kerb is on (-1 at `lo`, +1 at `hi`); 0 for a crossing. */
  readonly kerb: -1 | 0 | 1;
  readonly a: number;
  readonly b: number;
  /** The road a footway or crossing belongs to. */
  readonly segment?: SegmentId;
  /** The road node a corner or crossing belongs to. */
  readonly node?: NodeId;
  /** Which deck it is on. */
  readonly structure: RoadStructure;
  /** A crossing with no zebra painted: across the end of a road that leads nowhere. */
  readonly unmarked?: true;
}

// src/world/walkways.ts:60-66 function deckOf
/** The deck a road's footways belong to. */
export function deckOf(doc: RoadDoc, id: SegmentId): RoadStructure;
