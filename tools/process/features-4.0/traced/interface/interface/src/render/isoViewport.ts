// What the feature uses of src/render/isoViewport.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/isoViewport.ts:17-17 variable MIN_HALF_HEIGHT
/** Close inspection of pedestrian faces, gaits and the people inside vehicles. */
export const MIN_HALF_HEIGHT = m(0.8);

// src/render/isoViewport.ts:19-19 variable MAX_HALF_HEIGHT
/** Out to some 11 km of ground in view (the old map's side and a tenth more): within the far ground's ring round the view (`flatAtlas.ts`, 7 plates). */
export const MAX_HALF_HEIGHT = m(5_632);

// src/render/isoViewport.ts:22-24 function isoZoomBounds
/** The zoom range the iso rig can represent at a given viewport height. */
export function isoZoomBounds(height: number): { min: number; max: number };

// src/render/isoViewport.ts:27-27 variable DEFAULT_AZIMUTH
/** The orbit the camera starts in, and the one "reset view" returns to */
export const DEFAULT_AZIMUTH = Math.PI / 4;

// src/render/isoViewport.ts:28-28 variable DEFAULT_ELEVATION
export const DEFAULT_ELEVATION = (48 * Math.PI) / 180;

// src/render/isoViewport.ts:31-31 variable MAX_ELEVATION
export const MAX_ELEVATION = Math.PI / 2;

// src/render/isoViewport.ts:35-35 variable PERSPECTIVE_FOV
/** The perspective camera's vertical field of view far away, degrees */
export const PERSPECTIVE_FOV = 35;

// src/render/isoViewport.ts:40-42 function clampElevation
function clampElevation(e: number): number;

// src/render/isoViewport.ts:44-46 function wrapAzimuth
function wrapAzimuth(a: number): number;

// src/render/isoViewport.ts:48-63 interface IsoRig
export interface IsoRig {
  /** The camera drawn with: orthographic, or perspective (`setPerspective`). */
  readonly camera: OrthographicCamera | PerspectiveCamera;
  readonly perspective: boolean;
  /** Perspective on or off. */
  setPerspective(on: boolean): void;
  readonly viewport: Viewport;
  readonly target: Vector3;
  resize(width: number, height: number): void;
  /** The height of what is drawn at a map point (x, y) */
  setGround(groundAt: (x: number, y: number) => number): void;
  /** The lowest the perspective camera's eye may stand at a map point, world height: over the roofs near a building */
  setSolids(floorAt: ((x: number, y: number) => number) | null, roofAt?: ((x: number, y: number) => number) | null): void;
  /** One frame of the pull's own time, `dt` seconds, as Cinemachine's Deoccluder times it */
  settle(dt: number): boolean;
}

// src/render/isoViewport.ts:66-66 variable PULL_SAMPLES
/** The pull in front of a building (`apply`): samples along the line of sight, and how deep in a building's margin one weighs whole. */
const PULL_SAMPLES = 48;

// src/render/isoViewport.ts:67-67 variable PULL_SOFT
const PULL_SOFT = m(1.5);

// src/render/isoViewport.ts:78-460 function createIsoRig
export function createIsoRig(
  initial: Vec2,
  initialHalfHeight: number,
  orbit: { azimuth: number; elevation: number } = { azimuth: DEFAULT_AZIMUTH, elevation: DEFAULT_ELEVATION },
): IsoRig;
