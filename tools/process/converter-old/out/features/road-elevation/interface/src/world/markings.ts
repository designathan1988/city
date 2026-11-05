// What the feature uses of src/world/markings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/markings.ts:33-43 interface StrokeSpec
export interface StrokeSpec {
  readonly points: readonly Vec2[];
  readonly width: number;
  readonly color: string;
  /** Dash pattern in WORLD units, or null for a solid line. */
  readonly dash: readonly number[] | null;
  /** Dash phase in world units, so a split does not reflow the pattern. */
  readonly dashOffset: number;
  /** The points close a ring painted whole (`width` and `dash` unused): an area, not a line. */
  readonly fill?: true;
}

// src/world/markings.ts:46-46 variable DASH
/** Dash pattern used for every broken line, in world units. */
const DASH: readonly number[] = [m(3.2), m(3.2)];

// src/world/markings.ts:252-256 interface Bar
export interface Bar {
  readonly a: Vec2;
  readonly b: Vec2;
  readonly width: number;
}

// src/world/markings.ts:264-286 function stopBar
/** Stop bar for one approach, spanning only the lanes entering the junction. */
function stopBar(
  origin: Vec2,
  dir: Vec2,
  nrm: Vec2,
  rt: RoadType,
  trim: number,
  /** The travel way's offset from the centreline in this frame (`travelShift`, sign by end). */
  shift = 0,
): Bar | null;

// src/world/markings.ts:289-299 function crosswalkBars
/** Zebra bars for one approach. */
function crosswalkBars(
  origin: Vec2,
  dir: Vec2,
  nrm: Vec2,
  rt: RoadType,
  trim: number,
): Bar[];

// src/world/markings.ts:346-367 function junctionDetailAt
/** The stop bars and crosswalks of one junction's approaches, onto `stops` and `zebras` (`junctionDetail`). */
export function junctionDetailAt(net: Network, node: NodeId, stops: Bar[], zebras: Bar[]): void;

// src/world/markings.ts:401-451 function transitionMarkings
/** The paint across a taper where one road carries on at another width. */
export function transitionMarkings(net: Network, node: NodeId): StrokeSpec[];
