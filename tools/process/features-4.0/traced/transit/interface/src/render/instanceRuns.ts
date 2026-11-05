// What the feature uses of src/render/instanceRuns.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/instanceRuns.ts:16-16 type PoseFormat
/** The same design as the buildings' gardens (`gardens.ts`, which keeps a run per garden list), on the same cells (`cellInstances.ts`) */

/** DO NOT keep a matrix per thing, on the CPU or the GPU: a pose of `stride` floats, made three's matrix by the vertex shader (`cellInstances.ts` `posedMaterial`). */
export type PoseFormat = SlotFormat;

// src/render/instanceRuns.ts:69-88 interface RunStore
/** One mesh of the layer: a model, its material, how it is drawn. */
export interface RunStore {
  readonly name: string;
  /** Its bounding sphere computed (each slot's sphere is grown from it). */
  readonly geometry: BufferGeometry;
  /** Drawn from the layer's poses (`cellInstances.ts` `posedMaterial`): never shared with a mesh that reads three's own `instanceMatrix`. */
  readonly material: Material;
  /** The shadow pass's material (the wind's sway). */
  readonly depth?: MeshDepthMaterial;
  readonly castShadow: boolean;
  readonly receiveShadow: boolean;
  readonly renderOrder?: number;
  /** Has a colour per instance (white where a thing gives none): one program variant for its material. */
  readonly tinted: boolean;
  /** Leaf cards: left out at the map zoom (`setMap`). */
  readonly cards: boolean;
  /** More meshes drawn from the same instances: a plant's leaf cards, its far picture (`plantImpostors.ts`). */
  readonly with?: readonly KindMesh[];
  /** How its level is chosen by its size on the screen (`cellInstances.ts`); none: drawn at every size. */
  readonly levels?: KindLevels;
}

// src/render/instanceRuns.ts:97-97 type RunPack
/** A thing's instances in one store: poses (the layer's `PoseFormat`), tints (tinted stores) and each one's largest scale. */
type RunPack = Pack;

// src/render/instanceRuns.ts:100-100 type Run
/** A thing's instances, by store index. Never changed once made: a changed thing is a new run. */
export type Run = ReadonlyMap<number, RunPack>;

// src/render/instanceRuns.ts:103-106 interface RunChange
/** Runs to take away and to put in. A run is taken away by the same object that was put in. */
interface RunChange {
  readonly removed: readonly Run[];
  readonly added: readonly Run[];
}

// src/render/instanceRuns.ts:108-108 interface Tint
interface Tint { readonly r: number; readonly g: number; readonly b: number }

// src/render/instanceRuns.ts:111-149 class RunBuilder
/** Builds a run, placement by placement, in the poses of the layer it goes to (`createInstanceRuns`'s `format`). */
export class RunBuilder { … }

// src/render/instanceRuns.ts:132-135 method RunBuilder.turned
/** One instance turned about the up axis (`YAW_POSE`): its place in the scene's frame, its yaw, its scale on three axes, its largest scale and tint; drawn wherever it stands. */
  turned(store: number, x: number, y: number, z: number, yaw: number, sx: number, sy: number, sz: number, scale: number, tint?: Tint): void;

// src/render/instanceRuns.ts:137-148 method RunBuilder.build
build(): Run;

// src/render/instanceRuns.ts:152-162 function sameRun
/** Whether two runs hold the same instances, bit for bit. */
export function sameRun(a: Run, b: Run): boolean;

// src/render/instanceRuns.ts:170-182 interface InstanceRuns
export interface InstanceRuns {
  /** Every mesh of the layer, under `group`. */
  readonly group: Group;
  readonly meshes: readonly InstancedMesh[];
  readonly triangles: number;
  /** The runs that came and went; the meshes made are put under `group`. */
  update(change: RunChange): void;
  /** The map zoom: the leaf cards are left out, neither culled nor drawn; a store with far pictures drawn as them. Only a change of mode does anything. */
  setMap(map: boolean): void;
  /** Keeps the instances of each shown store the camera can see, or see the margin round, at the level each one's size on the screen picks. */
  cull(frustum: Frustum, view: Matrix4): void;
  dispose(): void;
}

// src/render/instanceRuns.ts:201-220 function createInstanceRuns
/** The layer over these stores, its runs built in `format`'s poses (`RunBuilder`). */
export function createInstanceRuns(name: string, specs: readonly RunStore[], margin: number, format: PoseFormat = YAW_POSE): InstanceRuns;

// src/render/instanceRuns.ts:209-212 method createInstanceRuns.update
update(change);

// src/render/instanceRuns.ts:214-214 method createInstanceRuns.cull
cull(frustum, view);

// src/render/instanceRuns.ts:215-218 method createInstanceRuns.dispose
dispose();
