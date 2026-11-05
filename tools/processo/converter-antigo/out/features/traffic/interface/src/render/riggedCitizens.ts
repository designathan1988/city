// What the feature uses of src/render/riggedCitizens.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/riggedCitizens.ts:40-40 variable CAPACITY
const CAPACITY = 1000;

// src/render/riggedCitizens.ts:42-42 variable LIMB_REACH
/** A figure's limbs reach past its rest pose's sphere by up to this much of it (a stride, an arm thrown out, a fall): the sphere it is culled by (`fitInstances`). */
const LIMB_REACH = 2;

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

// src/render/riggedCitizens.ts:90-100 interface KeptPose
/** A walker's last pose as played (`capturedPose`): the clips, phases and weights, whether one frame was copied, the limbs lost. */
interface KeptPose {
  index: number;
  readonly palette: Float32Array;
  readonly transform: Matrix4;
  frame: number;
  readonly clips: ClipFrames[];
  readonly phases: number[];
  readonly weights: number[];
  single: boolean;
  lost: readonly Severable[] | null;
}

// src/render/riggedCitizens.ts:103-105 function paletteBytes
/** Bytes of a body's palettes: the CPU's override rows and the GPU's target, a row a body, and the clips' atlas. */
function paletteBytes(batch: CitizenBatch): number;

// src/render/riggedCitizens.ts:113-113 variable CITIZEN_LEVEL_ERROR
/** The error each level of a body is simplified to, as a share of the body's size */
export const CITIZEN_LEVEL_ERROR = [0, 0.008, 0.03, 0.12] as const;

// src/render/riggedCitizens.ts:115-115 variable BODY_EXTENT
/** The tallest a body is drawn (a tall adult, scaled up), world units: the size its error is measured against. */
const BODY_EXTENT = m(1.9);

// src/render/riggedCitizens.ts:177-177 variable UNBURNT
/** A body's colour over its own: none, or burnt black by a bomb. */
const UNBURNT = new Color(1, 1, 1);

// src/render/riggedCitizens.ts:178-178 variable BURNT
const BURNT = new Color(0.075, 0.062, 0.055);

// src/render/riggedCitizens.ts:181-181 variable NEUTRAL_FACE
/** No expression: what a body drawn from a ragdoll palette shows (`drawPalette`). */
const NEUTRAL_FACE: FaceWeights = {};

// src/render/riggedCitizens.ts:339-1289 function createRiggedCitizens
export function createRiggedCitizens(models: readonly string[] = CROWD_IDS,
  onAssetsReady: () => void = () => {});

// src/render/riggedCitizens.ts:360-364 function createRiggedCitizens.disposeOwned
const disposeOwned = (owned: Set<{ dispose(): void }>): void =>;

// src/render/riggedCitizens.ts:443-451 function createRiggedCitizens.applyBand
/** A batch's materials on what bodies do at the current zoom (`citizenBandAt`): the skin's detail. */
  const applyBand = (batch: CitizenBatch): void =>;

// src/render/riggedCitizens.ts:514-747 function createRiggedCitizens.load
async function load(index: number): Promise<void>;
