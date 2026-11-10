// What the feature uses of src/render/shaderCatalogue.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/shaderCatalogue.ts:7-7 variable PROGRAM_FAMILIES
/** THE PROGRAMS COMPILED AT LOAD: a closed catalogue of what the game draws with, built before the curtain lifts */

/** The families of the catalogue, every one compiled before the town is shown. */
export const PROGRAM_FAMILIES = ['world', 'people'] as const;

// src/render/shaderCatalogue.ts:10-10 type ProgramFamily
type ProgramFamily = typeof PROGRAM_FAMILIES[number];

// src/render/shaderCatalogue.ts:13-16 interface FamilyStandIns
/** A family's stand-ins: drawn lit into the frame's target, and cast into the sun's shadow (`uploads.ts` shadowStandIn). */
interface FamilyStandIns {
  readonly lit: Object3D;
  readonly shadow: Object3D;
}

// src/render/shaderCatalogue.ts:19-23 interface FamilyCount
/** The debug counter of one family: its stand-ins, and whether their programs are built. */
interface FamilyCount {
  readonly lit: number;
  readonly shadow: number;
  readonly compiled: boolean;
}

// src/render/shaderCatalogue.ts:25-36 interface ShaderCatalogue
interface ShaderCatalogue {
  /** Settles once every family of this opening compiled (the loading curtain waits for it, `main.ts`). */
  readonly ready: Promise<void>;
  /** A map is being opened (`SceneHandle.beginLoad`): its world is compiled again when it is in. */
  begin(): void;
  /** The town is in (`RevealGate` warm or show): every family compiled, once an opening. */
  compile(scene: Object3D): void;
  /** The curtain lifted: the light variants the opening did not wait for are asked now, once the uploads that came with the town are in (`warmPending`), never while they hold the driver. */
  afterReveal(): void;
  /** Each family's stand-ins and whether they compiled: null for one not compiled yet. */
  counts(): Readonly<Record<ProgramFamily, FamilyCount | null>>;
}

// src/render/shaderCatalogue.ts:75-165 function createShaderCatalogue
/** `people`: every program a person can be drawn with (`people/proceduralCrowd.ts` personProgramStandIns), built without a body. `lightVariants`: the sets of the scene's lights a light switched on or off makes (`renderer.ts`), each compiled before the curtain lifts, one at a time and when nothing else compiles (`compilesQuiet`): a program first used in play waits for its link in the frame (three's WebGLRenderer.compileAsync: precompile before the first render). */
export function createShaderCatalogue(people: () => FamilyStandIns, lightVariants: () => readonly (readonly Object3D[])[] = () => []): ShaderCatalogue;
