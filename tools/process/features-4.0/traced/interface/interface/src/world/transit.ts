// What the feature uses of src/world/transit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/transit.ts:9-9 type TransitMode
/** Public transport, as the player lays it out */

export type TransitMode = 'bus' | 'train' | 'metro';

// src/world/transit.ts:11-26 interface TransitStop
export interface TransitStop {
  readonly id: number;
  readonly mode: TransitMode;
  /** Where it stands: a bus stop on the footway's edge, a station's platform on its track. */
  readonly x: number;
  readonly y: number;
  /** A bus stop: the road it is on. */
  readonly segment?: SegmentId;
  /** A station: the track it is on. */
  readonly track?: number;
  /** A bus terminal: lines turn here and their buses wait their time. */
  readonly terminal?: boolean;
  /** A metro station: its way down, on the footway nearest it (the station may lie under a street). */
  readonly entrance?: { readonly x: number; readonly y: number };
  readonly name?: string;
}

// src/world/transit.ts:28-33 interface RailTrack
interface RailTrack {
  readonly id: number;
  /** A train's track at grade, or the metro's under the ground. */
  readonly mode: 'train' | 'metro';
  readonly points: readonly Vec2[];
}

// src/world/transit.ts:35-44 interface TransitLine
export interface TransitLine {
  readonly id: number;
  readonly mode: TransitMode;
  /** Its stops or stations, in order: out along them and back. */
  readonly stops: readonly number[];
  readonly colour: string;
  /** Vehicles running on it. */
  readonly vehicles: number;
  readonly name: string;
}

// src/world/transit.ts:46-51 interface TransitData
export interface TransitData {
  readonly stops: readonly TransitStop[];
  readonly tracks: readonly RailTrack[];
  readonly lines: readonly TransitLine[];
  readonly nextId: number;
}

// src/world/transit.ts:53-53 function emptyTransit
export const emptyTransit = (): TransitData => …;

// src/world/transit.ts:103-103 function hasTransit
export const hasTransit = (t: TransitData): boolean => …;
