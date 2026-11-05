// What the feature uses of src/render/utilities.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/utilities.ts:73-77 interface Utilities
/** The overhead utility network on screen: poles, cross-arms, wires and lamps. */

export interface Utilities {
  readonly group: Object3D;
  readonly triangles: number;
  dispose(): void;
}

// src/render/utilities.ts:79-90 interface Placement
export interface Placement {
  x: number;
  y: number;
  /** Height above the ground, in world units. */
  z: number;
  yaw: number;
  /** Tilt of the part's local X axis up from level, radians (a brace, a lamp bracket). */
  roll?: number;
  sx: number;
  sy: number;
  sz: number;
}

// src/render/utilities.ts:155-237 function placePole
/** One pole's parts, pushed onto `out` by `POLE_PART` */
export function placePole(
  pole: UtilityPole,
  top: number,
  frame: PoleArms,
  kerb: { x: number; y: number } | null,
  groundAt: (x: number, y: number) => number,
  out: Placement[][],
): void;

// src/render/utilities.ts:255-276 function spanWires
/** DO NOT build the wires' vertices on the CPU again. */
export function spanWires(
  span: UtilitySpan,
  a: { readonly x: number; readonly y: number; readonly top: number; readonly arm: { x: number; y: number } },
  b: { readonly x: number; readonly y: number; readonly top: number; readonly arm: { x: number; y: number } },
  out: Float32Array,
  at: number,
): void;

// src/render/utilities.ts:524-537 function poleGroundAt
/** Ground a pole stands on: the footway where there is one, the terrain where there is not. */
export function poleGroundAt(
  elevation: RoadElevation,
  terrainAt: (x: number, y: number) => number,
  /** The network, to stand a pole level with a footway laid flush (`footwayRiseAt`). */
  net?: Network,
): (x: number, y: number) => number;
