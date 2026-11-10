// What the feature uses of src/view/viewport.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/view/viewport.ts:5-5 type Facing
/** Which of the four quarter turns the view is at. Zero for a top-down view. */
export type Facing = 0 | 1 | 2 | 3;

// src/view/viewport.ts:8-49 interface Viewport
/** What the input layer needs from a view, and nothing else. */
export interface Viewport {
  readonly kind: '2d' | 'iso';
  /** Ground point under a pointer, in world units, on the `y = 0` plane. */
  toWorld(px: number, py: number, cssW: number, cssH: number): Vec2;
  /** The same, but on a horizontal plane at `height`. */
  toWorldAt(px: number, py: number, height: number, cssW: number, cssH: number): Vec2;
  /** Where a ground point lands on screen, in CSS pixels. */
  toScreen(p: Vec2, cssW: number, cssH: number, height?: number): Vec2;
  /** Whether the ground under a pointer, on the plane at `height`, can be held by a pan */
  holds(px: number, py: number, height: number, cssW: number, cssH: number): boolean;
  /** Moves the view so `grabbed` (a point at `height`, what was under the pointer) sits under the pointer again. */
  panTo(grabbed: Vec2, px: number, py: number, cssW: number, cssH: number, height?: number): void;
  /** Zooms about a pointer, keeping the ground under it (at `height`, the ground's own height there). */
  zoomAt(px: number, py: number, factor: number, cssW: number, cssH: number, height?: number, grabbed?: Vec2): void;
  /** The point a zoom about (px, py) holds, and the height of the plane it is held on */
  grab?(px: number, py: number, height: number): { readonly world: Vec2; readonly height: number } | null;
  /** Moves the view over the ground, world units: `right` across the screen, `forward` the way the camera faces. */
  slide(right: number, forward: number): void;
  /** Where the camera's eye is (x, y on the map, z its height), or null for a flat view. */
  readonly eye: { readonly x: number; readonly y: number; readonly z: number } | null;
  /** The pointer's line of sight (x, y on the map, z up; a unit direction), from the camera into the scene */
  rayAt?(px: number, py: number, cssW: number, cssH: number): { readonly ox: number; readonly oy: number; readonly oz: number; readonly dx: number; readonly dy: number; readonly dz: number };
  /** Turns the view. A no-op where there is nothing to turn. */
  rotate(quarterTurns: number, px: number, py: number, cssW: number, cssH: number): void;
  /** Swings the camera round (`dAzimuth`, rad) and over (`dElevation`, rad) the `pivot` */
  orbit(dAzimuth: number, dElevation: number, pivot?: { readonly px: number; readonly py: number; readonly height: number }): void;
  /** Puts the camera at an exact bearing and tilt about the centre of the view. */
  setOrbit(azimuth: number, elevation: number): void;
  /** Bearing the camera looks from, rad, measured as the rig measures it. */
  readonly azimuth: number;
  /** Angle the camera looks down at, rad: PI/2 is straight down. */
  readonly elevation: number;
  /** World point at the centre of the view, and the only way to move it there. */
  readonly centre: Vec2;
  moveTo(p: Vec2): void;
  readonly zoom: number;
  readonly facing: Facing;
  /** Inclusive zoom range this viewport can actually represent. */
  readonly zoomBounds: { readonly min: number; readonly max: number };
  /** Screen pixels per world unit at the point looked at, as drawn. */
  readonly scaleAtCentre?: number;
}

// src/view/viewport.ts:52-104 function flatViewport
/** The existing flat camera, behind the same seam. */
export function flatViewport(camera: Camera): Viewport;

// src/view/viewport.ts:57-57 method flatViewport.toWorldAt
toWorldAt: (px, py, _height, cssW, cssH) => …;

// src/view/viewport.ts:60-64 method flatViewport.panTo
panTo(grabbed, px, py, cssW, cssH);

// src/view/viewport.ts:65-67 method flatViewport.zoomAt
zoomAt(px, py, factor, cssW, cssH);

// src/view/viewport.ts:79-79 method flatViewport.orbit
orbit();

// src/view/viewport.ts:90-93 method flatViewport.moveTo
moveTo(p);
