// What the feature uses of src/render/transit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/transit.ts:31-41 interface TransitMeshes
/** Public transport on screen (`world/transit.ts`, `sim/transit/transit.ts`) */
export interface TransitMeshes {
  readonly group: Group;
  readonly triangles: number;
  /** Whether trains are drawn: the transport's only casters that move (its stops and tracks stand still). */
  readonly moving: boolean;
  /** The metro under the ground, seen as a cut (the transit tool) */
  setXray(on: boolean): void;
  /** The trains where the simulation has them now. */
  update(trains: readonly TrainState[], groundAt: (x: number, y: number) => number): void;
  dispose(): void;
}

// src/render/transit.ts:323-467 function createTransit
/** DO NOT build the whole transport again on its revision or a change of the ground under any piece of it. */
export function createTransit(): TransitMeshes & {
  /** Brings the tracks and stops up to the diary; the items made again. */
  sync(doc: RoadDoc, groundAt: (x: number, y: number) => number, pavedAt: (x: number, y: number) => number, onRoad: (p: { x: number; y: number }) => boolean): number;
  /** The parts the camera can see, every frame. */
  cull(frustum: Frustum, view: Matrix4): void;
};
