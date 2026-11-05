// What the feature uses of src/render/citizenBake.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenBake.ts:55-76 interface ClipFrames
export interface ClipFrames {
  /** Twelve affine values per bone and frame; the fixed fourth row is restored for the GPU. */
  data: Float32Array;
  /** Intervals between baked frames; `data` holds `frames + 2` rows (the last repeated). */
  frames: number; duration: number;
  /** Ground one cycle covers on this body at scale 1, metres. */
  stride: number;
  /** Height of the pelvis above the model origin in the first frame, metres. */
  pelvisY: number;
  /** The pelvis's offset from the origin in plan in the first frame (model +X left, +Z forward), metres. */
  pelvisX: number;
  pelvisZ: number;
  loop: boolean;
  /** Ground covered by each baked frame on this body at scale 1, metres (start, stop). */
  travel?: Float32Array;
  /** Angle turned by each baked frame, radians, unsigned (turns). */
  yaw?: Float32Array;
  /** A rider's head bone in the first frame, in the model's frame: where a helmet goes. */
  head?: Matrix4;
  /** Per baked frame, the right then the left hand bone, in the model's frame (16 + 16): where a held thing goes. */
  hands?: Float32Array;
}

// src/render/citizenBake.ts:347-347 type Deferred
export type Deferred = Map<number, { readonly make: () => Promise<ClipFrames>; readonly standIn: number }>;

// src/render/citizenBake.ts:455-472 interface RigNode
/** One node of a rig: bone, skinned mesh or plain node, its parent and rest transform. */
export interface RigNode {
  readonly name: string;
  readonly kind: 'bone' | 'skinned' | 'node';
  readonly parent: number;
  readonly p: readonly number[];
  readonly q: readonly number[];
  readonly s: readonly number[];
  /** A skinned mesh: its bones (node indices), their inverses, its bind, and the vertex data the bakes read (feet, head). */
  readonly skin?: {
    readonly bones: readonly number[];
    readonly inverses: Float32Array;
    readonly bind: readonly number[];
    readonly bindMode: string;
    readonly position: Float32Array;
    readonly skinIndex: Float32Array;
    readonly skinWeight: Float32Array;
  };
}
