// What the feature uses of src/peopleLab/build.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/build.ts:22-23 function hairBase
/** A LAB PERSON BUILT THE GAME'S WAY: `buildPersonScene` (`render/people/personBuild.ts`), with the hair studio's hairstyle worn as an item */

const hairBase = (a: PeopleAssets, mo: Morpher): HairBase => …;

// src/peopleLab/build.ts:26-26 function labStyle
/** The style as the lab draws it: the eyes never swollen by `cartoonBody` (its sphere round each eye leaves a crater round the socket, an alien's face); big eyes come from the face's own `eye-scale` slider. */
const labStyle = (style: CartoonStyle): CartoonStyle => …;

// src/peopleLab/build.ts:29-34 function sculpted
/** The body with its brush sculpt (`bodySculpt.ts`) added. */
function sculpted(positions: Float32Array, body: HairSculpt | undefined): Float32Array;

// src/peopleLab/build.ts:36-47 interface BuiltPerson
export interface BuiltPerson {
  readonly scene: Object3D;
  readonly meshes: readonly SkinnedMesh[];
  readonly triangles: number;
  readonly morphs: number;
  /** A changed body shape (decimetres, `shape`) as relative moves of the mesh's vertices: live dragging (`PersonRig.morph`); absent on the tailored shells. */
  readonly morph?: (positions: Float32Array) => Float32Array;
  /** The body's shape it was built on (decimetres): what the face's shapes are added to (`faceRig.ts`). */
  readonly shape: Float32Array;
  /** A changed body shape as the build would make it, without building (`PersonRig.reshape`). */
  readonly reshape?: (positions: Float32Array) => Reshaped;
}

// src/peopleLab/build.ts:50-200 class LabBuilder
/** Builds the lab's people; keeps the MakeHuman packs and the grown hairstyles between builds. */
export class LabBuilder { … }

// src/peopleLab/build.ts:51-51 field LabBuilder.morpher
private morpher: Morpher | null = null;

// src/peopleLab/build.ts:52-52 field LabBuilder.assets
private assets: PeopleAssets | null = null;

// src/peopleLab/build.ts:54-54 field LabBuilder.grown
/** Grown hairstyles by their settings: the same settings are grown once. */
  private readonly grown = new Map<string, ProxyItem>();

// src/peopleLab/build.ts:56-66 method LabBuilder.grownHair
private grownHair(style: HairStyle, a: PeopleAssets, mo: Morpher): ProxyItem;

// src/peopleLab/build.ts:71-75 method LabBuilder.proportions
/** The face's proportions, measured and set to the canon (`proportions.ts`). */
  async proportions(): Promise<FaceProportions>;

// src/peopleLab/build.ts:78-82 method LabBuilder.setup
/** The MakeHuman packs, loaded once. */
  async setup(): Promise<{ assets: PeopleAssets; morpher: Morpher }>;

// src/peopleLab/build.ts:85-92 method LabBuilder.hairItem
/** The hairstyle worn, as an item, before any sculpt: grown or loaded. */
  async hairItem(person: LabPerson): Promise<ProxyItem | null>;

// src/peopleLab/build.ts:95-108 method LabBuilder.studioBody
/** The person's body for the hair studio: shaped, in the bind posture, its skin and the head and neck vertices hair is pinned to. */
  async studioBody(person: LabPerson): Promise<StudioBody>;

// src/peopleLab/build.ts:112-115 method LabBuilder.item
/** Any worn item by name, before its sculpt. */
  async item(person: LabPerson, name: string): Promise<ProxyItem | null>;

// src/peopleLab/build.ts:119-130 method LabBuilder.shaped
/** The hairstyle or the outfit with its length, volume and waves (`hairShape.ts`) on this body (dm), moved points kept out of the skin
   *  (`GroomBody.collide`); any other piece as it is. */
  private shaped(person: LabPerson, name: string, item: ProxyItem, body: Float32Array, a: PeopleAssets): ProxyItem;

// src/peopleLab/build.ts:133-138 method LabBuilder.wornPiece
/** A worn piece as the person wears it, before its sculpt: what the sculptor works on. */
  async wornPiece(person: LabPerson, name: string): Promise<ProxyItem | null>;

// src/peopleLab/build.ts:141-144 method LabBuilder.sculptBody
/** The person's body as the sculptor shows it: as built (shaped, styled, its own sculpt), in its bind posture. */
  async sculptBody(person: LabPerson): Promise<SculptBody>;

// src/peopleLab/build.ts:147-150 method LabBuilder.shape
/** The body's shape for a person (decimetres): what `BuiltPerson.morph` reads. */
  async shape(person: LabPerson): Promise<Float32Array>;

// src/peopleLab/build.ts:152-199 method LabBuilder.build
async build(person: LabPerson): Promise<BuiltPerson>;
