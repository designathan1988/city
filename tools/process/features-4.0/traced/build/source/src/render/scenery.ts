// The code of src/render/scenery.ts that is the feature's (the rest of the file is shared).

// src/render/scenery.ts:346-360 interface Scenery: the feature's own
export interface Scenery {
  readonly meshes: readonly InstancedMesh[];
  /** Grass tufts and wildflowers, shown only at close zoom. */
  readonly grass: Group;
  readonly triangles: number;
  /** Swaps every plant between its close-up model and its map-zoom model. */
  setNear(near: boolean): void;
  /** The map zoom (`PLANT_MAP_ZOOM`): the leaf cards are not drawn at all, a shrub or a length of hedge being a pixel or two */
  setMap(map: boolean): void;
  /** Keeps only the instances the camera can see, or whose shadow it can. */
  cull(frustum: Frustum, view: Matrix4): void;
  /** Drops every tree and shrub whose trunk stands where `covered` says - under a building (section 5) */
  exclude(covered: ((x: number, y: number) => boolean) | null): void;
  dispose(): void;
}
