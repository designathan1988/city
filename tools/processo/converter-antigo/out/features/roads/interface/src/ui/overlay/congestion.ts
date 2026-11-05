// What the feature uses of src/ui/overlay/congestion.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/overlay/congestion.ts:10-10 interface Moving
/** THE CONGESTION MAP (Layers > Congestion): each road in view coloured by how full its lanes are, green free, red jammed */

/** A vehicle as the map reads it: the road its lane is on (undefined on a junction's connector or off the road). */
interface Moving { readonly segment: SegmentId | undefined }

// src/ui/overlay/congestion.ts:13-13 variable CAR_SPACE
/** Road a queued car takes up, its length and the gap to the next, m (HCM jam density: some 130 cars a lane-km). */
const CAR_SPACE = m(7.5);

// src/ui/overlay/congestion.ts:18-22 function congestionColour
/** The colour of a road whose lanes are `occupancy` full. */
export function congestionColour(occupancy: number): string;

// src/ui/overlay/congestion.ts:25-36 function roadOccupancy
/** Each road's occupancy: the length its vehicles take over the length of its travel lanes (a few cars waiting at a light leave a long street free). */
export function roadOccupancy(net: Network, vehicles: Iterable<Moving>): Map<SegmentId, number>;

// src/ui/overlay/congestion.ts:39-39 variable sampled
/** A road's line as the map draws it: the middle, the middle moved across by the road's width, then each point of the band (x, y). */
const sampled = new WeakMap<SegmentRibbon, Float64Array>();

// src/ui/overlay/congestion.ts:42-58 function bandOf
/** The points of a road's band, sampled once: a road changed is a new ribbon (`networkBuild.ts`), so DO NOT sample it again each frame. */
function bandOf(ribbon: SegmentRibbon): Float64Array;

// src/ui/overlay/congestion.ts:61-84 function drawCongestion
/** The congestion map drawn on the editor's overlay: the roads meeting `view` (a world box), as bands as wide as their carriageway. */
export function drawCongestion(ctx: CanvasRenderingContext2D, net: Network, vehicles: Iterable<Moving>,
  view: readonly [number, number, number, number], at: (p: Vec2) => Vec2): void;
