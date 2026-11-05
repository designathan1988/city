// What the feature uses of src/render/partLevelsWork.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/partLevelsWork.ts:7-7 interface Ladder
/** One welded mesh simplified once per error: its positions, its other attributes interleaved (`weights` a float each), its index. */
interface Ladder { readonly positions: Float32Array; readonly attributes: Float32Array; readonly weights: readonly number[]; readonly index: Uint32Array; readonly errors: readonly number[] }

// src/render/partLevelsWork.ts:10-14 function ladderIndices
/** `index` simplified to the fewest triangles each error allows (meshoptimizer README: `ErrorAbsolute`, `Prune`), finest first. */
export function ladderIndices(req: Ladder): Uint32Array[];

// src/render/partLevelsWork.ts:27-27 type Weld
/** `smooth`: welded by position, normal and colour (a body, glass); `boxes`: by position and colour, normals creased again after (welded with its normals a box assembly is all locked corners, meshoptimizer README). */
export type Weld = 'smooth' | 'boxes';

// src/render/partLevelsWork.ts:30-30 interface ArrayOf
/** One attribute's numbers, as a geometry holds them. */
export interface ArrayOf { readonly array: TypedArray; readonly itemSize: number; readonly normalized: boolean }

// src/render/partLevelsWork.ts:33-38 interface PartArrays
/** A part as the page sends it: its position, colour and normal (where it has them) and its index. */
export interface PartArrays {
  readonly position: ArrayOf;
  readonly color: ArrayOf | null;
  readonly normal: ArrayOf | null;
  readonly index: Uint32Array | Uint16Array | null;
}

// src/render/partLevelsWork.ts:41-44 type LevelArrays
/** A level: the finer one again (`same`), nothing to draw (null), or its own attributes (none: the welded ones, `shared`), index and bounding sphere. */
export type LevelArrays = 'same' | null | {
  readonly position?: ArrayOf; readonly color?: ArrayOf; readonly normal?: ArrayOf;
  readonly index: Uint32Array | null; readonly sphere: readonly number[];
};

// src/render/partLevelsWork.ts:47-47 interface PartLevels
/** Levels 1.. of a part, and the welded attributes its smooth levels share. */
export interface PartLevels { readonly shared: { position: ArrayOf; color: ArrayOf; normal: ArrayOf | null } | null; readonly levels: LevelArrays[] }

// src/render/partLevelsWork.ts:54-110 function partLevelArrays
/** A part's coarser levels from its arrays: what `partLevels` made on the page, made here. */
export function partLevelArrays(part: PartArrays, weld: Weld): PartLevels;
