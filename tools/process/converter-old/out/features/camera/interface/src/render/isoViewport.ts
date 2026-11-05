// What the feature uses of src/render/isoViewport.ts (shared with the program): declarations as the source writes them, bodies left out.

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

// src/render/isoViewport.ts:78-460 function createIsoRig
export function createIsoRig(
  initial: Vec2,
  initialHalfHeight: number,
  orbit: { azimuth: number; elevation: number } = { azimuth: DEFAULT_AZIMUTH, elevation: DEFAULT_ELEVATION },
): IsoRig;
