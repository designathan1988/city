// What the feature uses of src/world/surfaceParts.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/surfaceParts.ts:9-13 interface MouthStrip
/** A strip a curved road's drawn surface lays across one of its mouths (`junction/mouthOverlap.ts`), still to be laid: its full centreline, its half width, where the mouth is cut. */
interface MouthStrip {
  readonly full: Polyline;
  readonly half: number;
  readonly mouth: number;
}

// src/world/surfaceParts.ts:16-19 interface SurfaceSource
/** What the drivable surface round a node is made of, in the order `turnPaths.ts` `JunctionSurface` adds them */
export interface SurfaceSource {
  readonly centre: Vec2;
  readonly parts: readonly (Ring | readonly Vec2[] | MouthStrip)[];
}

// src/world/surfaceParts.ts:22-58 function surfaceParts
/** The pieces of the drivable surface round `node` - its junction plate and its legs - read off the network, nothing made */
export function surfaceParts(net: Network, node: NodeId): SurfaceSource;
