// What the feature uses of src/render/roadStandIn.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadStandIn.ts:12-17 variable STREET_MEANS
/** A ROAD JUST BUILT, DRAWN ON THE PAGE IN THE NEXT FRAME UNTIL THE WORLD'S BUILD OF ITS COMMIT LANDS (`renderer.ts` worldSteps): its casing and its carriageway, and the junctions at its ends, from the network the commit solved
 *  - DO NOT leave a road built invisible until the world's build lands: the coarse in place until the fine comes, as every level does. */

/** THE MEAN LINEAR ALBEDO OF EACH BAKED STREET TEXTURE (`bake/roadRecipes.ts`), every texel as stored (sRGB bytes) and read (linear): constants, worked out once from the recipes. */
export const STREET_MEANS: Readonly<Record<'asphalt' | 'footway' | 'kerb' | 'verge', readonly [number, number, number]>> = {
  asphalt: [0.040666, 0.041456, 0.044698],
  footway: [0.210044, 0.208119, 0.195658],
  kerb: [0.338651, 0.331236, 0.316671],
  verge: [0.029413, 0.070144, 0.010057],
};

// src/render/roadStandIn.ts:20-23 interface StandInHeights
/** What the stand-in reads of the roads' heights (`RoadElevation`). */
export interface StandInHeights {
  onSegment(segment: SegmentId, x: number, y: number): number;
  at(x: number, y: number): number;
}

// src/render/roadStandIn.ts:34-43 function fill
/** A closed outline (x, y pairs) triangulated and laid at `height`: positions, normals up, indices appended. */
function fill(ring: readonly { readonly x: number; readonly y: number }[], height: (x: number, y: number) => number,
  out: { positions: number[]; indices: number[] }): void;

// src/render/roadStandIn.ts:77-108 method RoadStandIn.show
/** The roads `segments` drawn now (the commit's network solved, of network revision `revision`): their outlines, and the junctions at their ends whose every road is one of them.
   *  - DO NOT draw a road or a junction that was there before (a street the new one meets, split at the new junction): it is drawn already, and the stand-in's flat tone covered it until the build came (via2 37/40). */
  show(net: Network, heights: StandInHeights, segments: Iterable<SegmentId>, revision: number): void;
