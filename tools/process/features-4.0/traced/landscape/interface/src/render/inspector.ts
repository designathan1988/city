// What the feature uses of src/render/inspector.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/inspector.ts:11-28 interface InspectShot
/** THE INSPECTION CAMERA (development builds only). */
interface InspectShot {
  /** Point looked at, world units: (x, y) on the map and `h` its height. */
  readonly x: number;
  readonly y: number;
  readonly h: number;
  /** Camera bearing round the point, radians (0 looks along +x), and height angle. */
  readonly azimuth: number;
  readonly elevation: number;
  /** Camera distance from the point, world units. */
  readonly distance: number;
  /** Vertical field of view, degrees. */
  readonly fov?: number;
  readonly width?: number;
  readonly height?: number;
  /** Cut away a vehicle's roof and near side: its heading */
  readonly interior?: { readonly heading: number; readonly roofCut: number; readonly halfWidth: number;
    readonly cx?: number; readonly cy?: number };
}

// src/render/inspector.ts:30-33 interface Inspector
export interface Inspector {
  shot(spec: InspectShot): string;
  dispose(): void;
}
