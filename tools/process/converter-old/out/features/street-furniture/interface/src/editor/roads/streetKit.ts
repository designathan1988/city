// What the feature uses of src/editor/roads/streetKit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roads/streetKit.ts:14-14 function carried
/** A STREET'S POWER LINE AND FURNITURE CARRIED THROUGH AN EDIT OF ITS ROAD: widened, narrowed or moved, the road takes them along instead of leaving them in its lanes or the grass */

/** The pieces a street is furnished with (`furnitureSets.ts`): what an edit carries; a sign or a name plate stays where it was put. */
const carried = (kind: LandscapeKind): boolean => …;

// src/editor/roads/streetKit.ts:21-25 interface Kit
/** What stood on the streets before the edit: their poles and pieces, and the set to lay again. */
interface Kit {
  readonly poles: PoleId[];
  readonly items: number[];
  readonly line: boolean;
}

// src/editor/roads/streetKit.ts:41-59 function kitOf
/** The poles and pieces standing on `segments`' footways, each street's own (nearer to it than to any other). */
function kitOf(doc: RoadDoc, net: Network, segments: readonly SegmentId[]): Kit;

// src/editor/roads/streetKit.ts:73-90 function carryStreetKit
/** Runs a road edit of `segments` (`edit`, true when it changed the map), then lays their power line and furniture again on the roads it left (`after`: their ids then, the same by default) */
export function carryStreetKit(doc: RoadDoc, net: Network, segments: readonly SegmentId[], edit: () => boolean,
  after: () => readonly SegmentId[] = () => segments.filter((id) => doc.segment(id) !== undefined)): boolean;
