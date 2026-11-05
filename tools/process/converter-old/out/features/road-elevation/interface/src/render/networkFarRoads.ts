// What the feature uses of src/render/networkFarRoads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/networkFarRoads.ts:25-29 interface FarRoadHeights
/**
 * EVERY ROAD OF A LARGE CITY AT ITS FAR LEVEL FROM THE FIRST FRAME OF ITS OPENING (`renderer.ts` openingBuild): a large city's opening
 * builds the detailed surfaces of the roads round its view only, the rest filled block by block after the curtain (tens of seconds in
 * which the map showed no street past the opening's view: step-25k 08-map, 2026-10-21). This level is made from the network itself:
 * each road's and junction's real outline (`Network.ribbons`, `Network.junctions`: the carriageway, and the casing its footways and
 * kerbs fill) at its class's colour over the street textures' means (as the blocks' far level paints them), at the solved heights where
 * the opening's solve took the road and on the ground elsewhere, merged a cell at a time in a job worker: O(the outlines' vertices).
 * The area the opening built in detail is cut out of it; once the fill's blocks are in it is made again whole and kept, hidden, as the
 * roads' top level (`mesh/roadResidency.ts`): shown in a cell whose blocks' far levels the far budget let go.
 */

/** What the outlines' heights are read from: the opening's solve (`RoadElevation`), the roads it left for later, the ground. */
export interface FarRoadHeights {
  onSegment(segment: SegmentId, x: number, y: number): number;
  at(x: number, y: number): number;
  pendingRoads?(): SegmentId[];
}

// src/render/networkFarRoads.ts:32-32 variable ASPHALT_REFERENCE_COLOUR
/** The class's tint over the reference the asphalt texture is authored against (`roadSurfaces.ts` CLASS_TINTS), times the texture's mean. */
const ASPHALT_REFERENCE_COLOUR = new Color(ASPHALT_REFERENCE);

// src/render/networkFarRoads.ts:33-36 variable ASPHALT_BY_CLASS
const ASPHALT_BY_CLASS: readonly (readonly [number, number, number])[] = ROAD_TYPES.map((type) => {
  const c = new Color(type.color);
  return [STREET_MEANS.asphalt[0] * (c.r / ASPHALT_REFERENCE_COLOUR.r), STREET_MEANS.asphalt[1] * (c.g / ASPHALT_REFERENCE_COLOUR.g), STREET_MEANS.asphalt[2] * (c.b / ASPHALT_REFERENCE_COLOUR.b)];
});

// src/render/networkFarRoads.ts:37-37 variable CASING
const CASING: readonly [number, number, number] = [STREET_MEANS.footway[0] * FOOTWAY_TINT.pavers[0], STREET_MEANS.footway[1] * FOOTWAY_TINT.pavers[1], STREET_MEANS.footway[2] * FOOTWAY_TINT.pavers[2]];

// src/render/networkFarRoads.ts:48-83 function farRoadInput
/** @rebuild The outlines of the network, their heights read on the page (`heights`, `groundAt` for the roads the solve left): what the worker fills. Once a scoped opening, the whole map by design: no other level stands for the roads its view left. */
export function farRoadInput(net: Network, heights: FarRoadHeights, groundAt: (x: number, y: number) => number, cell: number, origin: number,
  exclude: readonly [number, number, number, number] | null): { input: FarRoadInput; transfer: ArrayBuffer[] };
