// What the feature uses of src/people/body/assets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/people/body/assets.ts:15-32 interface PeopleAssets
/** Loads the MakeHuman packs (about 10 MB gzipped) once, when something first needs a person model */

export interface PeopleAssets {
  readonly packs: PeoplePacks;
  /** The game_engine skeleton: names, parents, where each head is read from. */
  readonly skeleton: SkeletonMeta;
  readonly bodyRange: readonly (readonly [number, number])[];
  readonly mesh: {
    readonly vertexCount: number;
    readonly faces: Uint16Array;
    readonly uvs: Float32Array;
    readonly faceUvs: Uint16Array;
    readonly faceGroup: Uint8Array;
    readonly faceGroups: readonly string[];
    readonly vertexGroups: Readonly<Record<string, readonly (readonly [number, number])[]>>;
    readonly joints: Uint8Array;
    readonly weights: Uint16Array;
    readonly boneNames: readonly string[];
  };
}

// src/people/body/assets.ts:34-41 interface SkeletonMeta
interface SkeletonMeta {
  readonly bones: readonly {
    readonly name: string;
    readonly parent: string | null;
    readonly head: { readonly strategy: string; readonly cubeName?: string; readonly vertexIndices?: readonly number[] };
  }[];
  readonly weights: { readonly vertexCount: number; readonly layout: { readonly joints: { readonly byteOffset: number }; readonly weights: { readonly byteOffset: number } } };
}
