// What the feature uses of src/render/people/personRig.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/personRig.ts:41-61 variable CAPTURE_NAME
/** game_engine bone -> the capture's name for it. */
export const CAPTURE_NAME: Readonly<Record<string, string>> = (() => {
  const names: Record<string, string> = {
    Root: 'Bip01', pelvis: 'Bip01_Pelvis', spine_01: 'Bip01_Spine', spine_02: 'Bip01_Spine1', spine_03: 'Bip01_Spine2',
    neck_01: 'Bip01_Neck', head: 'Bip01_Head',
  };
  const fingers = ['thumb', 'index', 'middle', 'ring', 'pinky'];
  for (const [side, s] of [['l', 'L'], ['r', 'R']] as const) {
    Object.assign(names, {
      [`clavicle_${side}`]: `Bip01_${s}_Clavicle`, [`upperarm_${side}`]: `Bip01_${s}_UpperArm`,
      [`lowerarm_${side}`]: `Bip01_${s}_Forearm`, [`hand_${side}`]: `Bip01_${s}_Hand`,
      [`thigh_${side}`]: `Bip01_${s}_Thigh`, [`calf_${side}`]: `Bip01_${s}_Calf`,
      [`foot_${side}`]: `Bip01_${s}_Foot`, [`ball_${side}`]: `Bip01_${s}_Toe0`,
    });
    fingers.forEach((finger, f) => {
      names[`${finger}_01_${side}`] = `Bip01_${s}_Finger${f}`;
      names[`${finger}_02_${side}`] = `Bip01_${s}_Finger${f}1`;
      names[`${finger}_03_${side}`] = `Bip01_${s}_Finger${f}2`;
    });
  }
  return names;
})();

// src/render/people/personRig.ts:70-76 interface SkeletonMeta
export interface SkeletonMeta {
  readonly bones: readonly {
    readonly name: string;
    readonly parent: string | null;
    readonly head: { readonly strategy: string; readonly cubeName?: string; readonly vertexIndices?: readonly number[] };
  }[];
}

// src/render/people/personRig.ts:91-107 interface PersonRigInput
export interface PersonRigInput {
  readonly data: PersonMeshData;
  readonly skeleton: SkeletonMeta;
  readonly bodyRange: readonly (readonly [number, number])[];
  /** The morphed body, decimetres (`Morpher.shape`). */
  readonly positions: Float32Array;
  readonly look: PersonLook;
  readonly texturedSkin?: boolean;
  /** The capture avatar's bind, by capture bone name (`captureBind`). */
  readonly capture: ReadonlyMap<string, Vector3>;
  /** And its bones' own axes (`captureBindRotations`). */
  readonly captureAxes?: ReadonlyMap<string, Quaternion>;
  /** The MakeHuman items the look wears, loaded (`loadProxyItem`), by name. */
  readonly proxies?: ReadonlyMap<string, ProxyItem>;
  /** A base body for the procedural crowd (`people/proceduralCrowd.ts`) */
  readonly nude?: boolean;
}

// src/render/people/personRig.ts:110-116 interface WornPiece
/** A MakeHuman item fitted to a rigged body, as a separate skinned piece. */
export interface WornPiece {
  /** Vertices in the body's bind posture, metres. */
  readonly positions: Float32Array;
  /** Bones in the skeleton's order, four a vertex, and their weights. */
  readonly joints: Uint16Array;
  readonly weights: Float32Array;
}

// src/render/people/personRig.ts:118-136 interface PersonRig
export interface PersonRig {
  /** As a GLTF scene would be: a root holding the bones and the skinned mesh. */
  readonly scene: Group;
  readonly mesh: SkinnedMesh;
  /** Standing height in the bind posture, metres. */
  readonly height: number;
  /** A changed body shape (decimetres, as \`positions\`) as relative moves of the mesh's vertices, when known. */
  readonly morph?: (positions: Float32Array) => Float32Array;
  /** An item fitted to this body as a piece of its own: on the body as built, or on a changed shape of it */
  readonly wear?: (item: ProxyItem, shape?: Float32Array) => WornPiece;
  /** A changed body shape (decimetres, as `positions`) as the build would make it, without building: the mesh's vertex moves and each bone's head in the bind posture (skeleton order, metres). */
  readonly reshape?: (shape: Float32Array) => Reshaped;
  /** A changed body shape (decimetres, as `positions`) as moves of every base mesh vertex in the bind posture, metres */
  readonly deltas?: (shape: Float32Array) => Float32Array;
  /** `deltas(positions + offset)` at the listed vertices only */
  readonly deltasAt?: (offset: Float32Array, vertices: ArrayLike<number>) => Float32Array;
  /** What `deltasAt` reads of this body, to work it out on another thread (`rigDeltasAt`, `proceduralCrowd.ts` faces in a worker). */
  readonly deltasData?: RigDeltas;
}

// src/render/people/personRig.ts:139-142 interface Reshaped
/** `PersonRig.reshape`: a changed shape as relative moves of the mesh's vertices and the bones' heads in the bind posture (metres). */
export interface Reshaped {
  readonly moves: Float32Array;
  readonly heads: Vector3[];
}

// src/render/people/personRig.ts:145-150 interface RigDeltas
/** A body's shape and posture as `deltasAt` reads them: its positions (decimetres), its lowest body vertex, and each bone's posture as a 3 x 3 (column-major, nine a bone). */
export interface RigDeltas {
  readonly positions: Float32Array;
  readonly lowestDm: number;
  readonly bodyRange: readonly (readonly [number, number])[];
  readonly linear: Float64Array;
}

// src/render/people/personRig.ts:153-179 function rigDeltasAt
/** `deltasAt` from its data alone: moves of `positions + offset` at `vertices`, metres, each turned by its bones' postures (`weights` and `joints` of the base mesh, four a vertex). */
export function rigDeltasAt(d: RigDeltas, weights: ArrayLike<number>, joints: ArrayLike<number>, offset: Float32Array, vertices: ArrayLike<number>): Float32Array;

// src/render/people/personRig.ts:181-433 function createPersonRig
export function createPersonRig(input: PersonRigInput): PersonRig;

// src/render/people/personRig.ts:374-385 function createPersonRig.wear
const wear = (item: ProxyItem, shape: Float32Array = positions): WornPiece =>;

// src/render/people/personRig.ts:401-402 function createPersonRig.deltasAt
const deltasAt = (offset: Float32Array, vertices: ArrayLike<number>): Float32Array => …;
