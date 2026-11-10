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
