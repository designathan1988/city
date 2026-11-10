// What the feature uses of src/render/cellInstances.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/cellInstances.ts:36-53 interface KindMesh
/**
 * INSTANCES KEPT CELL BY CELL, DRAWN AT THE LEVEL THEIR SIZE ON THE SCREEN CALLS FOR: the gardens'
 * (`gardens.ts`) and every layer kept thing by thing (`instanceRuns.ts`). A kind's placements are kept
 * once for all its meshes (a plant's body, its cards, its far picture); a cull walks only the 64 m
 * cells the view takes in, and writes each instance into the buffers of the level its size on the
 * screen picks, with hysteresis, so a plant never flickers between two. DO NOT go back to drawing
 * every plant in view with its whole model: a flowering bush two pixels wide cost 112 triangles.
 * The GPU gets each instance's pose, not its matrix: the vertex shader makes the matrix (`posedMaterial`).
 */

/** One mesh a kind's instances are drawn with. */
export interface KindMesh {
  readonly name: string;
  /** Its bounding sphere computed: the kind's instances are culled by the sphere round all its meshes'. */
  readonly geometry: BufferGeometry;
  /** Drawn from poses (`posedMaterial`): never shared with a mesh that reads three's own `instanceMatrix`. */
  readonly material: Material;
  /** The shadow pass's material (the wind's sway); none: three's own depth material, drawn from poses (`poseDepthOf`). */
  readonly depth?: MeshDepthMaterial;
  readonly castShadow: boolean;
  readonly receiveShadow: boolean;
  readonly renderOrder?: number;
  /** Leaf cards: left out at the map zoom (a kind with no far pictures). */
  readonly cards: boolean;
  /** Draws the instances small enough to be far pictures (`KindLevels.far`); else the near ones. */
  readonly far: boolean;
  /** Called on the mesh once it is made (the pictures' hooks, `plantImpostors.ts`). */
  readonly made?: (mesh: InstancedMesh) => void;
}

// src/render/cellInstances.ts:61-61 type KeptFrustum
/**
 * The frustum a layer culls by, with the box the sun's cascades cover (`SunCascades.coverBox`): what casts there is kept whether the
 * camera frames it or not. DO NOT let the shadows' casters follow the frame alone: as the view moved, the posts, props and plants coming in
 * and going out at its edges were changes all round the near cascade, whose store was thrown away and drawn whole in half the frames of a
 * flight (318 of 606, `npm run diagnose`, 2026-10-22). Kept under the cascades, they come and go past them, where no cascade draws.
 */
export type KeptFrustum = Frustum & { keep?: Box3 | null; /** The near cascade's texel, world units: under it a thing casts only if it spans its texels there (`castBySize`). */ keepTexel?: number };

// src/render/cellInstances.ts:70-86 interface KindLevels
/** How a kind's level is chosen by its size on the screen: its bounding diameter, device pixels. */
export interface KindLevels {
  /** The radius a size is measured by, model units: the diameter times the instance's largest scale. */
  readonly reach: number;
  /** Pixels across under which the far meshes draw it, thinned under `THIN_PX` (`plantImpostors.ts`). */
  readonly far?: number;
  /** Pixels across under which it is not drawn at all (a kind with no far pictures). */
  readonly least?: number;
  /** A small thing (a bin, a bench): cast into a cascade only where it spans `SMALL_CAST_TEXELS` (`makeMesh`). */
  readonly small?: boolean;
  /** Its far model is the colour's alone (a street light's light column, `propGeometry.ts` lampFarGeometry): every one drawn casts by its
   *  near model (`COVER`), the swap between the two never a change of the shadow. */
  readonly shadowNear?: boolean;
  /** Whether the far pictures can be drawn (taken, `plantImpostors.ts`): until then every size is drawn near. */
  readonly ready?: () => boolean;
  /** Told each view culled for and half the screen's height, device pixels (the far pictures' uniforms). */
  readonly view?: (view: Matrix4, halfHeight: number) => void;
}

// src/render/cellInstances.ts:88-93 interface KindSpec
export interface KindSpec {
  readonly meshes: readonly KindMesh[];
  /** A colour per instance (white where a thing gives none): one program variant for its materials. */
  readonly tinted: boolean;
  readonly levels?: KindLevels;
}

// src/render/cellInstances.ts:96-108 interface SlotFormat
/** How a layer keeps a placement: a pose of `stride` floats (5 to 8), made three's matrix by the GPU (`glsl`) and by the CPU (`matrixInto`) with the same math. */
export interface SlotFormat {
  readonly stride: number;
  /** Names its programs (`posedMaterial`): one program variant per format. */
  readonly key: string;
  /** The body of GLSL `mat4 poseMatrixOf(vec4 a, vec4 b)`: `matrixInto` written in GLSL, the pose's floats being a.xyzw then b (b.w is 1 for a 7-float pose). */
  readonly glsl: string;
  /** GLSL declared before `poseMatrixOf` (the uniforms it reads), and those uniforms, given to every program drawn from these poses. */
  readonly pars?: string;
  readonly uniforms?: Readonly<Record<string, { value: unknown }>>;
  /** GLSL run first in `poseMatrixOf`: sets `poseHidden = true` for a pose not to be drawn (every vertex at one point). */
  readonly guard?: string;
  matrixInto(data: Float32Array, at: number, out: Float32Array, outAt: number): void;
}

// src/render/cellInstances.ts:111-116 interface Pack
/** A thing's instances of one kind: placements (`stride` floats each), tints, each one's largest scale. */
export interface Pack {
  readonly data: Float32Array;
  readonly colours: Float32Array | null;
  readonly scales: Float32Array;
  readonly count: number;
}

// src/render/cellInstances.ts:118-122 interface CellChange
export interface CellChange {
  readonly removed: readonly object[];
  /** Each thing's key (taken away by the same object later) and its packs, by kind. */
  readonly added: readonly { readonly key: object; readonly packs: ReadonlyMap<number, Pack> }[];
}

// src/render/cellInstances.ts:124-137 interface CellInstances
export interface CellInstances {
  readonly meshes: readonly InstancedMesh[];
  /** Triangles of the near meshes, every live instance counted. */
  readonly triangles: number;
  /** Bytes its arrays hold on the CPU: the kinds' placements and the levels' buffers (`propCells.ts` counts them). */
  readonly cpuBytes: number;
  /** The things that came and went; the meshes made for kinds that had none. */
  update(change: CellChange): InstancedMesh[];
  /** The map zoom: every plant with far pictures drawn as one, the other kinds' leaf cards left out. Only a change does anything. */
  setMap(map: boolean): void;
  /** Writes each kind with a mesh shown the instances the view (or its margin) takes in, each at its level. */
  cull(frustum: Frustum, view: Matrix4): void;
  dispose(): void;
}

// src/render/cellInstances.ts:221-233 function posedGeometry
/** The model with a level's poses as its instanced attributes: its own attributes and index shared (three r186 `WebGLAttributes` keeps one buffer per attribute). */
function posedGeometry(model: BufferGeometry, poses: InstancedInterleavedBuffer): BufferGeometry;

// src/render/cellInstances.ts:236-240 function letGo
/** A posed geometry let go: the model's attributes and index taken off first, so three frees the poses' buffer and the vertex arrays only (r186 `WebGLGeometries` frees every attribute a disposed geometry holds). */
function letGo(g: BufferGeometry): void;

// src/render/cellInstances.ts:243-259 function dressedMesh
/** A new mesh of `spec` on `geometry`, as the cells draw it: its shadow's material, no sphere of its own (culled instance by instance). */
function dressedMesh(spec: KindMesh, geometry: BufferGeometry, format: SlotFormat): InstancedMesh;

// src/render/cellInstances.ts:281-281 variable HYSTERESIS
/** The band round a level's threshold, as a share of it, an instance keeps the level it had in. */
export const HYSTERESIS = 0.15;

// src/render/cellInstances.ts:291-291 variable screen
/** Half the drawing buffer's height, device pixels, as the meshes last drew (`onBeforeRender`): the screen sizes are in it. */
const screen = { half: 540 };

// src/render/cellInstances.ts:296-296 variable shadowSun
/** Towards the sun the shadows are cast by (three's space, unit), as the cascades hold it (`setShadowSun`); none known: the margin round every way. */
const shadowSun = { x: 0, y: 0, z: 0, known: false };

// src/render/cellInstances.ts:304-304 variable bufferSize
const bufferSize = new Vector2();

// src/render/cellInstances.ts:307-307 function screenHalf
/** Half the drawing buffer's height, device pixels, as the meshes last drew: what a size on the screen is measured in. */
export const screenHalf = (): number => …;

// src/render/cellInstances.ts:344-359 class IntList
/** The instances a kind draws at one level (near or far): ONE buffer of poses and one of tints for all the level's meshes (a plant's body and its cards). */
/** A list of slots or positions, kept from cull to cull: emptied by its `length`, its array kept. DO NOT use a JS array emptied by
 *  `length = 0`: V8 lets its store go and every cull grew it again from nothing (the flight's garbage, `npm run diagnose`, 2026-10-22). */
class IntList { … }

// src/render/cellInstances.ts:361-387 interface Level
interface Level {
  readonly meshes: MeshState[];
  poses: InstancedInterleavedBuffer | null;
  tints: InstancedBufferAttribute | null;
  /** Instances its buffers hold: what is in view, grown twice over and shrunk under a quarter, never the kind's. */
  capacity: number;
  /** Per drawn position: the slot the buffers hold there (-1: none). */
  shown: Int32Array;
  /** Per slot of the kind: its position in the buffers (-1: not drawn at this level). */
  at: Int32Array;
  /** Whether a mesh of it is drawn at the cull in progress. */
  live: boolean;
  /** Positions in use, from 0. */
  drawn: number;
  /** The slots the cull in progress draws here that hold no position yet; the positions it wrote. */
  readonly entering: IntList;
  readonly changed: IntList;
  /** Its buffers made and not yet sent: sent whole, no range of them. */
  fresh: boolean;
  /** The widest instance it drew at its last cull (radius, world units): what a cascade's texel is measured against (`shadowCasters.ts`). */
  largest: number;
  /** Positions from 0 to here, the level's front: the near level's drawn in the colour pass (those after them cast alone), the far level's
   *  cast (those after them, under the cover, cast by the near level); `castBySize` counts. */
  front: number;
  /** Per slot of the kind: whether it casts from this level now (`reconcile`: a change of it is a change of the shadow). */
  cast: Uint8Array;
}

// src/render/cellInstances.ts:389-399 interface MeshState
interface MeshState {
  readonly spec: KindMesh;
  readonly level: Level;
  mesh: InstancedMesh | null;
  /** Whether it was drawn (shown, not left out) when its kind was last culled. */
  wasDrawn: boolean;
  /** Takes it off the casters chosen by cascade (`castBySize`). */
  uncast: (() => void) | null;
  /** Where it was put (its owner's group) while it is out of the scene with nothing to draw (`settleInScene`). */
  home: Object3D | null;
}

// src/render/cellInstances.ts:401-433 interface Kind
interface Kind {
  readonly index: number;
  readonly spec: KindSpec;
  readonly meshes: MeshState[];
  /** Near, far. */
  readonly levels: readonly [Level, Level];
  /** The sphere round every mesh's, model units: what an instance is culled by. */
  readonly centre: Vector3;
  readonly reach: number;
  capacity: number;
  used: number;
  dead: number;
  data: Float32Array;
  colours: Float32Array;
  /** Per slot: centre x, y, z and radius, scene frame (radius -Infinity: dead). */
  spheres: Float32Array;
  /** Per slot: where it stands (its matrix's translation) and its size, world units across (`KindLevels.reach`). */
  feet: Float32Array;
  sizes: Float32Array;
  /** Per slot: its pick (`impostorPick`), the cull it was last in view at, the level it was drawn at, and the level the cull it was seen at draws it at (`NONE`: not drawn). */
  picks: Float32Array;
  seen: Uint32Array;
  level: Uint8Array;
  want: Uint8Array;
  bounds: Bounds | null;
  runs: Map<object, { start: number; count: number }>;
  /** Culls of this kind so far; culled for the current view; culled by the walk in progress. */
  serial: number;
  /** Whether its far pictures could be drawn at its last cull. */
  pictured: boolean;
  current: boolean;
  active: boolean;
}

// src/render/cellInstances.ts:447-1131 function createCellInstances
export function createCellInstances(kindSpecs: readonly KindSpec[], format: SlotFormat, margin: number): CellInstances;

// src/render/cellInstances.ts:1010-1051 method createCellInstances.update
update(change);

// src/render/cellInstances.ts:1067-1122 method createCellInstances.cull
cull(frustum, view);

// src/render/cellInstances.ts:1123-1129 method createCellInstances.dispose
dispose();
