// What the feature uses of src/render/riggedCitizens.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/riggedCitizens.ts:46-53 interface LevelMesh
/** One level of one part of a body: its own mesh, and each instance's palette row (`citizenRow`, read by the skinning). */
interface LevelMesh {
  readonly mesh: InstancedMesh;
  readonly rows: InstancedBufferAttribute;
  /** Figures written at this level this frame. */
  count: number;
  /** A slot's colour changed since the GPU took them (burnt, or a new buffer): DO NOT send every figure's colour every frame. */
  colourChanged: boolean;
}

// src/render/riggedCitizens.ts:55-58 interface CitizenPart
/** A part of a body: its distinct levels' meshes, and which of them each level (`CITIZEN_LEVEL_ERROR`) is drawn with. */
interface CitizenPart {
  readonly levels: LevelMesh[];
  readonly at: number[];
}

// src/render/riggedCitizens.ts:59-87 interface CitizenBatch
interface CitizenBatch {
  resources: Set<{ dispose(): void }>;
  animationBytes: number;
  lastUsed: number;
  disposed: boolean;
  /** Every level mesh of every part (`parts`), as added to the group. */
  meshes: InstancedMesh[]; parts: CitizenPart[]; sources: SkinnedMesh[]; local: Matrix4[]; clips: ClipFrames[];
  /** The same baked clips, by the name the gait plays them by. */
  gait: GaitClips;
  /** Floats of a body's palette (16 a bone), and the bodies written this frame. */
  width: number; count: number;
  /** The skeletons blended on the GPU (`people/crowdAnimation.ts`): the clips' atlas, the pass, its rows; the skinning reads `uniform`. */
  atlas: CitizenAtlas; pass: PalettePass; rows: number; uniform: { value: Texture };
  /** Its first row in the palettes' shared target, read by the skinning. */
  readonly base: { value: number };
  /** A body's palette worked out on the CPU (held by a ragdoll, maimed, more plays than the pass takes). */
  readonly scratch: Float32Array;
  /** Rest joints for the pass's refit, all at the origin: the cooked bodies are drawn on their own joints. */
  readonly joints: DataTexture;
  readonly parents: Int16Array;
  /** This body's helmet in its head bone's frame (`riderPoses.helmetShape`), or null. */
  helmet: Matrix4 | null;
  /** Clips still to bake, on first use (`Deferred`), one at a time: in a worker, on the body's rig (`rigNodes`, made at the first). */
  deferred: Deferred;
  baking: Promise<void>;
  readonly sex: WalkSex;
  readonly scene: Object3D;
  rigNodes: readonly RigNode[] | null;
}

// src/render/riggedCitizens.ts:339-1289 function createRiggedCitizens
export function createRiggedCitizens(models: readonly string[] = CROWD_IDS,
  onAssetsReady: () => void = () => {});
