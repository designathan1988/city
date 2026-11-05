// What the feature uses of src/editor/roads/streetKit.ts (shared with the program): declarations as the source writes them, bodies left out.

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
