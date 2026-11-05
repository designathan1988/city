// What the feature uses of src/people/body/morph.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/people/body/morph.ts:6-11 interface BaseMeta
/** The MakeHuman base mesh, morphed: the person model's body (Person track H1). */

/** `base.json` (the parts used here). */
export interface BaseMeta {
  readonly vertexCount: number;
  readonly sections: readonly { readonly name: string; readonly type: string; readonly itemSize: number; readonly count: number; readonly byteOffset: number }[];
  readonly faceGroups: readonly string[];
  readonly vertexGroups: Readonly<Record<string, readonly (readonly [number, number])[]>>;
}

// src/people/body/morph.ts:14-24 interface MacroPcaMeta
/** `targets-macro-pca.json`. */
export interface MacroPcaMeta {
  readonly components: number;
  readonly vertexCount: number;
  readonly layout: {
    readonly basis: { readonly byteOffset: number; readonly count: number; readonly scales: readonly number[] };
    readonly coefficients: { readonly byteOffset: number; readonly count: number };
    readonly residualIndices: { readonly byteOffset: number; readonly count: number };
    readonly residualDeltas: { readonly byteOffset: number; readonly count: number };
  };
  readonly targets: readonly { readonly name: string; readonly row: number; readonly residualStart: number; readonly residualCount: number; readonly residualScale: number }[];
}

// src/people/body/morph.ts:27-31 interface LocalMeta
/** `targets-local.json`. */
export interface LocalMeta {
  readonly entryCount: number;
  readonly layout: { readonly indices: { readonly byteOffset: number }; readonly deltas: { readonly byteOffset: number } };
  readonly targets: readonly { readonly name: string; readonly group: string; readonly start: number; readonly count: number; readonly scale: number }[];
}

// src/people/body/morph.ts:34-40 interface RegionalCategory
/** One regional slider (`modifiers.json` `.regional[group].categories[]`). */
export interface RegionalCategory {
  readonly name: string;
  readonly has_left_and_right: boolean;
  /** Two-way sliders: the target either way. One-target sliders (`head-oval`) have none. */
  readonly opposites?: Readonly<Record<string, string>>;
  readonly targets?: readonly string[];
}

// src/people/body/morph.ts:42-53 interface PeoplePacks
export interface PeoplePacks {
  readonly base: BaseMeta;
  readonly baseBin: ArrayBuffer;
  readonly macro: MacroPcaMeta;
  readonly macroBin: ArrayBuffer;
  readonly local: LocalMeta;
  readonly localBin: ArrayBuffer;
  readonly modifiers: {
    readonly macro: MacroDefinition;
    readonly regional: Readonly<Record<string, { readonly categories: readonly RegionalCategory[] }>>;
  };
}

// src/people/body/morph.ts:56-56 type RegionalValues
/** A regional slider's value, -1..1: negative towards its "decr/down/..." target, positive towards the other. */
export type RegionalValues = Readonly<Record<string, number>>;

// src/people/body/morph.ts:58-397 class Morpher
export class Morpher { … }

// src/people/body/morph.ts:59-59 field Morpher.vertexCount
readonly vertexCount: number;

// src/people/body/morph.ts:61-61 field Morpher.base
/** Base positions, decimetres, the OBJ frame (+Y up, +Z forward). */
  readonly base: Float32Array;

// src/people/body/morph.ts:63-63 field Morpher.coeff
private readonly coeff: Float32Array;

// src/people/body/morph.ts:68-68 field Morpher.macroNames
private readonly macroNames: string[];

// src/people/body/morph.ts:69-69 field Morpher.macroRow
private readonly macroRow = new Map<string, MacroPcaMeta['targets'][number]>();

// src/people/body/morph.ts:74-94 constructor Morpher.constructor
constructor(private readonly packs: PeoplePacks);

// src/people/body/morph.ts:171-188 method Morpher.addRegional
/** The regional sliders' moves added into `into` (decimetres, base frame) */
  addRegional(into: Float32Array, regional: RegionalValues): void;

// src/people/body/morph.ts:191-257 method Morpher.height
/** `bodyHeight(shape(params), bodyRange)` from the heights alone */
  height(params: MacroParams, bodyRange: readonly (readonly [number, number])[]): number;
