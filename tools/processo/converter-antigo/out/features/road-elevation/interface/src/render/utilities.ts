// What the feature uses of src/render/utilities.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/utilities.ts:73-77 interface Utilities
/** The overhead utility network on screen: poles, cross-arms, wires and lamps. */

export interface Utilities {
  readonly group: Object3D;
  readonly triangles: number;
  dispose(): void;
}

// src/render/utilities.ts:524-537 function poleGroundAt
/** Ground a pole stands on: the footway where there is one, the terrain where there is not. */
export function poleGroundAt(
  elevation: RoadElevation,
  terrainAt: (x: number, y: number) => number,
  /** The network, to stand a pole level with a footway laid flush (`footwayRiseAt`). */
  net?: Network,
): (x: number, y: number) => number;
