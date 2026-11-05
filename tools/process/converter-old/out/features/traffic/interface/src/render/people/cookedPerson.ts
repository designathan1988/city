// What the feature uses of src/render/people/cookedPerson.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/cookedPerson.ts:7-7 type ArrayKind
/** A person's body cooked ahead (`scripts/cook-people.mjs`): the output of building it */

type ArrayKind = 'f32' | 'u32' | 'u16' | 'u8';

// src/render/people/cookedPerson.ts:8-8 interface BufferRef
interface BufferRef { readonly at: number; readonly length: number; readonly kind: ArrayKind }

// src/render/people/cookedPerson.ts:9-9 interface CookedAttribute
interface CookedAttribute { readonly name: string; readonly itemSize: number; readonly normalized: boolean; readonly data: BufferRef }

// src/render/people/cookedPerson.ts:10-10 interface CookedMorph
interface CookedMorph { readonly name: string; readonly vertices: BufferRef; readonly deltas: BufferRef }

// src/render/people/cookedPerson.ts:11-29 interface CookedMesh
interface CookedMesh {
  readonly attributes: CookedAttribute[];
  readonly index: BufferRef | null;
  readonly groups: { start: number; count: number; materialIndex: number }[];
  readonly morphs: CookedMorph[];
  readonly morphRelative: boolean;
  readonly dictionary: Record<string, number>;
  readonly influences: number[];
  readonly lods: { readonly index: BufferRef; readonly groups: { start: number; count: number; materialIndex: number }[] }[];
  readonly bones: number[];
  /** Full precision (the skeleton works in doubles): 16 numbers a bone. */
  readonly inverses: number[];
  readonly bind: number[];
  readonly bindMode: string;
  readonly materials: unknown[];
  readonly materialArray: boolean;
  /** What the shaders read off the geometry (`skinAppearance.ts`): the face's origin and scale, the garments worn. */
  readonly extra: { faceOrigin?: number[]; faceScale?: number; wornGroups?: string[] };
}

// src/render/people/cookedPerson.ts:30-38 interface CookedNode
interface CookedNode {
  readonly name: string;
  readonly kind: 'bone' | 'skinned' | 'group' | 'node';
  readonly parent: number;
  readonly p: number[];
  readonly q: number[];
  readonly s: number[];
  readonly mesh?: CookedMesh;
}

// src/render/people/cookedPerson.ts:39-39 interface CookedHeader
interface CookedHeader { readonly version: 2; readonly nodes: CookedNode[] }

// src/render/people/cookedPerson.ts:135-135 type Typed
/** A typed array as the cook wrote it. */
type Typed = Float32Array | Uint32Array | Uint16Array | Uint8Array;

// src/render/people/cookedPerson.ts:137-144 interface DecodedMesh
/** A skinned mesh's arrays read from the cook, each its own buffer (movable to the page): its face shapes whole, its bounds worked out. */
interface DecodedMesh {
  readonly attributes: readonly Typed[];
  readonly index: Typed | null;
  readonly morphs: readonly Float32Array[];
  readonly lods: readonly Typed[];
  /** Box min and max, then the sphere's centre and radius, as three's `computeBoundingBox` / `computeBoundingSphere` give them. */
  readonly bounds: readonly number[];
}

// src/render/people/cookedPerson.ts:146-146 interface DecodedPerson
/** A cooked person read: the header, and each node's mesh arrays (null: no mesh). */
export interface DecodedPerson { readonly header: CookedHeader; readonly meshes: readonly (DecodedMesh | null)[] }

// src/render/people/cookedPerson.ts:205-266 function assemblePerson
/** The person's scene from its decoded arrays: three's objects only, no loop over a vertex. */
export function assemblePerson(person: DecodedPerson): Group;
