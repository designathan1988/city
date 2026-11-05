// What the feature uses of src/world/terrain.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/terrain.ts:4-4 type TerrainMode
export type TerrainMode = 'raise' | 'lower' | 'flatten' | 'river';

// src/world/terrain.ts:6-26 interface TerrainStamp
export interface TerrainStamp {
  readonly id: number;
  readonly x: number;
  readonly y: number;
  readonly radius: number;
  readonly strength: number;
  readonly mode: TerrainMode;
  /** Height a `flatten` stamp levels towards. */
  readonly level?: number;
  /** The brush stroke this dab belongs to. */
  readonly stroke?: number;
  /** Where a rough dab's noise is measured from (`rough`) */
  readonly ox?: number;
  readonly oy?: number;
  /** A natural brush (raise and lower dabs the player paints) */
  readonly rough?: boolean;
  /** How hard the brush's edge is, 0..1, for raise and lower: 0 (absent) the smooth dome */
  readonly hardness?: number;
  /** A raise's SHAPE other than the dome of its falloff. 'dome': a granite sugarloaf */
  readonly profile?: TerrainProfile;
}

// src/world/terrain.ts:29-29 type TerrainProfile
/** The raise shapes besides the falloff's dome (`TerrainStamp.profile`). */
export type TerrainProfile = 'dome';

// src/world/terrain.ts:36-36 variable MAX_TERRAIN_STAMPS
/** How many brush dabs one map may hold. */
export const MAX_TERRAIN_STAMPS = 4_096;

// src/world/terrain.ts:72-84 function valueNoise
/** Smooth deterministic value noise, -1..1 (the land's and the ecosystem's). */
export function valueNoise(x: number, y: number): number;

// src/world/terrain.ts:87-87 type ReliefVersion
/** Which land a map starts from (`RoadDoc.terrainRelief`) */
export type ReliefVersion = 1 | 2 | 3;

// src/world/terrain.ts:88-88 variable RELIEF_LEGACY
export const RELIEF_LEGACY: ReliefVersion = 1;

// src/world/terrain.ts:90-90 variable RELIEF_FLAT
export const RELIEF_FLAT: ReliefVersion = 3;

// src/world/terrain.ts:100-106 function reliefHeight
/** The map's own land at a point, before any stamp: its relief's field. */
export function reliefHeight(relief: ReliefVersion, x: number, y: number): number;

// src/world/terrain.ts:161-208 class TerrainIndex
/** A uniform grid over the stamps, so a height query touches only nearby ones. */
export class TerrainIndex { … }

// src/world/terrain.ts:163-163 field TerrainIndex.stamps
readonly stamps: readonly TerrainStamp[];

// src/world/terrain.ts:167-167 field TerrainIndex.relief
/** The land under the stamps (`ReliefVersion`). */
  readonly relief: ReliefVersion;

// src/world/terrain.ts:218-292 function sampleTerrainHeight
/** The height of the land at a point: base relief plus every stamp over it. */
export function sampleTerrainHeight(
  stamps: readonly TerrainStamp[] | TerrainIndex,
  x: number,
  y: number,
): number;
