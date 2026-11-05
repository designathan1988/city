// What the feature uses of src/world/transit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/transit.ts:9-9 type TransitMode
/** Public transport, as the player lays it out */

export type TransitMode = 'bus' | 'train' | 'metro';

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
