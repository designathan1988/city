// What the feature uses of src/render/people/proceduralCrowd.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/proceduralCrowd.ts:52-52 variable WOUND_SLOTS
/** Procedural people (`people-lab.html`, then the game): one MakeHuman body per class - sex and age band */

/** Wounds kept a person (`procPerson`): bullet holes and where blood soaks out of them. */
const WOUND_SLOTS = 8;

// src/render/people/proceduralCrowd.ts:54-54 variable PLACE_FLOATS
/** A person's matrix as drawn this frame (with their height), read by every piece they are in: DO NOT write it into each piece. */
const PLACE_FLOATS = 16;

// src/render/people/proceduralCrowd.ts:57-57 variable PERSON_FLOATS
/** A person's row of `procPerson`: their wounds (`WOUND_SLOTS` texels), their expression weights (`EXPR_SLOTS` / 4 texels), their place (4
 *  texels), then their far colours (`FAR_ZONES` texels: a zone's colour, and 0 cloth, 1 skin, 2 hair). */
const PERSON_FLOATS = (WOUND_SLOTS + EXPR_SLOTS / 4) * 4 + PLACE_FLOATS + FAR_ZONES * 4;

// src/render/people/proceduralCrowd.ts:59-59 variable EXPR_AT
/** Where a person's expression weights start in their row, floats. */
const EXPR_AT = WOUND_SLOTS * 4;

// src/render/people/proceduralCrowd.ts:61-61 variable PLACE_AT
/** Where their place starts, floats. */
const PLACE_AT = (WOUND_SLOTS + EXPR_SLOTS / 4) * 4;

// src/render/people/proceduralCrowd.ts:63-63 variable ZONE_AT
/** Where their far colours start (`FarZone`), floats. */
const ZONE_AT = PLACE_AT + PLACE_FLOATS;

// src/render/people/proceduralCrowd.ts:65-69 variable WOUND_BONES
const WOUND_BONES: Readonly<Record<BodyPart, readonly string[]>> = {
  head: ['Bip01_Head'], torso: ['Bip01_Spine', 'Bip01_Spine1', 'Bip01_Spine2', 'Bip01_Pelvis'],
  armL: ['Bip01_L_UpperArm', 'Bip01_L_Forearm'], armR: ['Bip01_R_UpperArm', 'Bip01_R_Forearm'],
  legL: ['Bip01_L_Thigh', 'Bip01_L_Calf'], legR: ['Bip01_R_Thigh', 'Bip01_R_Calf'],
};

// src/render/people/proceduralCrowd.ts:70-70 variable spotsOf
const spotsOf = new WeakMap<object, Record<BodyPart, number[]>>();

// src/render/people/proceduralCrowd.ts:104-104 variable COVER_WIDTH
const COVER_WIDTH = 4096;

// src/render/people/proceduralCrowd.ts:113-113 variable COVERING
/** A garment that hides the skin under it (as `dressedGeometry` decides). */
const COVERING = new Set(['clothes', 'shoes', 'top', 'bottom', 'skirt', 'dress', 'suit', 'gloves']);

// src/render/people/proceduralCrowd.ts:116-116 variable INSTANCED
/** The attributes a piece has an entry of per person drawn. */
const INSTANCED = new Set(['aPerson', 'aDye', 'aWorn', 'aSkin', 'aOut', 'aShoe']);

// src/render/people/proceduralCrowd.ts:119-131 interface Piece
/** One level of an item on a class, or the far mesh of a class and outfit */
interface Piece {
  readonly item: Item | null;
  readonly level: number;
  readonly mesh: InstancedMesh;
  readonly attrs: Map<string, InstancedBufferAttribute>;
  capacity: number;
  people: ProceduralPerson[];
  /** Entries written since the last upload. */
  dirty: boolean;
  /** Triangles drawn per person. */
  readonly triangles: number;
  readonly ready: Promise<void>;
}

// src/render/people/proceduralCrowd.ts:134-148 interface Item
/** An item (or a skin) fitted to a class: one set of vertices, and a piece a level it is drawn at. */
interface Item {
  readonly name: string;
  readonly kind: Kind;
  readonly role: PieceRole;
  readonly source: BufferGeometry;
  readonly levels: (Piece | null)[];
  readonly vertices: number;
  /** Settles once every level's shaders are built and its meshes are in the scene. */
  ready: Promise<void>;
  /** Its texture's mean colour at its own vertices, linear: its colour on a far body (`zoneColours`). */
  readonly colour: Color;
  /** Its widest side over its wearer's height: the levels it is too small to draw at left out (`itemShownAt`). */
  readonly share: number;
  readonly map: Texture | null;
}

// src/render/people/proceduralCrowd.ts:150-228 interface BodyClass
interface BodyClass {
  readonly key: string;
  readonly sex: WalkSex;
  readonly band: AgeBand;
  readonly base: MacroParams;
  readonly shape: Float32Array;
  readonly coefficients: Float64Array;
  readonly rig: PersonRig;
  /** An item fitted to the class's body (`PersonRig.wear`): on the people's worker, where the rig is kept. */
  readonly fit: (item: ProxyItem) => Promise<WornPiece>;
  readonly height: number;
  readonly clips: Record<ProcClip, ClipFrames>;
  readonly bones: number;
  /** Bones parents first, and each one's parent (-1 for the root). */
  readonly order: readonly number[];
  readonly parent: Int16Array;
  /** Each bone's head per unit of each shape coefficient, metres: [bone][k][xyz]. */
  readonly jointBasis: Float32Array;
  /** The shape basis, kept for `probe`. */
  readonly shapePixels: Float32Array;
  readonly bodies: ClassBodies;
  readonly anim: ClassAnimation;
  readonly pass: PalettePass;
  readonly uniforms: {
    procBones: { value: Texture };
    /** The class's first row in the palettes' shared target (`paletteTargets.ts`). */
    procBase: { value: number };
    procBodies: { value: Texture };
    procBodyStride: { value: number };
    procCover: { value: DataTexture };
    procCoverRows: { value: number };
    /** The face index (four vertices a texel), the expressions' shapes from `procExprHead`, each person's face from `procFaceHead`. */
    procFace: { value: Texture };
    procFaceRows: { value: number };
    procFaceHead: { value: number };
    procExprHead: { value: number };
    /** Each person's row (`PERSON_FLOATS`): wounds - where on the body at rest, and since when (-1: none) - then expression weights, then their place. */
    procPerson: { value: DataTexture };
    /** The clock the wounds spread by (seconds), and the body's units a metre. */
    procTime: { value: number };
    procWoundUnit: { value: number };
  };
  /** `procPerson`'s pixels: each person's wounds and expression weights now. */
  personData: Float32Array;
  /** The rows whose place was written this frame. */
  readonly placed: number[];
  /** Palette rows worked out here: those held, bent, jolted or maimed, and the ragdolls' questions. */
  palette: Float32Array;
  /** `procFace` on the GPU only: the head (index, expressions), then each person's own face (their regional sliders) as moves of the head's vertices, `faceRows` rows each. */
  readonly faces: GpuRows;
  /** The sliders each row's face was made from (null: none), to make it again on a restore. */
  readonly faceOf: (Readonly<Record<string, number>> | null)[];
  /** The head's rows read again from the cook (or baked again), for a restore. */
  readonly reloadHead: () => Promise<Float32Array>;
  readonly faceVerts: Int32Array;
  rows: number;
  capacity: number;
  readonly cover: Map<string, number>;
  readonly body: BufferGeometry;
  readonly skins: Map<string, Item>;
  readonly items: Map<string, Item>;
  /** The far mesh (level 3): the class's body, every far person of the class in it, each in their colours a zone (null while it is made). */
  far: Piece | null;
  /** Each body vertex's zone (`FarZone`), its area a zone, and the head's frame the hair is told by: what a garment covers (`zoneCover`). */
  readonly zones: Uint8Array;
  readonly zoneArea: Float32Array;
  readonly head: HeadFrame;
  /** Each item's share of a zone: covered (`deleteVerts`), or its own area over the body's there; made once a class and item. */
  readonly zoneCover: Map<string, Float32Array>;
  /** Every piece of the class, for the frame's matrices. */
  readonly pieces: Piece[];
  readonly people: ProceduralPerson[];
  /** Each bone's zone (`boneZone`), for an item's vertices. */
  readonly boneZones: Uint8Array;
  /** The body's height at rest, its own units: an item's size is held against it (`Item.share`). */
  readonly bodyHeight: number;
  /** People drawn this frame. */
  drawn: number;
}

// src/render/people/proceduralCrowd.ts:231-231 type ProcClip
/** What a procedural person plays: walking, standing, running, sprinting for their life, cowering, photographing. */
export type ProcClip = 'walk' | 'idle' | 'run' | 'sprint' | 'cower' | 'photo' | 'getUp' | 'duck' | 'hurtWalk' | 'hurtRun' | 'nervous' | 'hit' | 'staggerBack' | 'riseFront' | 'riseBack' | 'crawl';

// src/render/people/proceduralCrowd.ts:233-261 interface ProceduralPerson
export interface ProceduralPerson {
  readonly spec: PersonSpec;
  readonly band: AgeBand;
  readonly sex: WalkSex;
  /** Their row in the class's palette, face and wound textures. */
  readonly row: number;
  /** Standing height over the class body's, the instance's scale. */
  readonly scale: number;
  /** Metres. */
  readonly height: number;
  readonly items: readonly string[];
  /** Their grown hairstyle's item (`hair:...`) and colour, for its strands close up. */
  readonly grown: string | null;
  readonly hairColour: Color;
  /** How far each of their joints is from the class body's, metres (their baked body's, `crowdBodies.ts`). */
  readonly joints: Float32Array;
  /** Where they stand and face; what they play. Set by the caller each frame (a zero scale hides them). */
  readonly matrix: Matrix4;
  clip: ProcClip;
  phase: number;
  /** The clip they are leaving, still playing under the new one at `weight` */
  fade?: { clip: ProcClip; phase: number; weight: number } | undefined;
  /** What they are doing, as the face shows it (`faceAt`): 'talk', 'panic'... */
  activity?: string | undefined;
  /** Limbs (or the head) lost to shots: their bones closed at the joint they were torn from. Set by the caller. */
  lost?: readonly Severable[] | undefined;
  /** How tall they stand on the screen, pixels: their level of detail. Set by the caller each frame; unset, they are drawn in full. */
  pixels?: number | undefined;
}

// src/render/people/proceduralCrowd.ts:278-278 variable ROW_START
const ROW_START = 64;

// src/render/people/proceduralCrowd.ts:280-280 variable PIECE_START
/** A piece's room for people at first; doubled when full. */
const PIECE_START = 16;

// src/render/people/proceduralCrowd.ts:282-282 variable BLOB_RADIUS
/** A far person's shadow disc, metres across their middle. */
const BLOB_RADIUS = 0.34;

// src/render/people/proceduralCrowd.ts:285-285 variable ACCESSORY_COLOURS
/** Colours a generated accessory (a headband) is dyed. */
const ACCESSORY_COLOURS = [0xc0392b, 0x1f3a93, 0xf2f0ea, 0x111111, 0xd35400, 0x8e44ad, 0x16a085] as const;

// src/render/people/proceduralCrowd.ts:287-287 variable plain
let plain: DataTexture | null = null;

// src/render/people/proceduralCrowd.ts:329-331 type ClassShading
/** What a person's programs read of their class (`uniformsInto`): its textures and its bind matrices. */
type ClassShading = Pick<BodyClass, 'uniforms'> & {
  readonly rig: { readonly mesh: { readonly bindMatrix: Matrix4; readonly bindMatrixInverse: Matrix4 } };
};

// src/render/people/proceduralCrowd.ts:475-578 function pieceMaterial
/** `grown`: a procedural hair item, its texture a strand atlas */
function pieceMaterial(cls: ClassShading, v: Variant, map: Texture | null, eyes: Texture | null, widen: number): Material;

// src/render/people/proceduralCrowd.ts:581-624 function farMaterial
/** The far mesh's program (level 3): a vertex's colour the person's for its zone (`FarZone`, their row of `procPerson` past `ZONE_AT`). */
function farMaterial(cls: ClassShading): Material;

// src/render/people/proceduralCrowd.ts:627-638 function depthMaterial
/** DO NOT cast a piece with its own level's program: it casts the next level's triangles (`castsAt`), so the next level's skinning, without the face or expressions the shadow cannot show. */
function depthMaterial(cls: ClassShading, v: Variant, widen: number): MeshDepthMaterial;

// src/render/people/proceduralCrowd.ts:738-738 variable samplers
const samplers = new WeakMap<Texture, ((u: number, v: number) => readonly [number, number, number]) | null>();

// src/render/people/proceduralCrowd.ts:855-855 variable __CROWD_LOD_HASH__
declare const __CROWD_LOD_HASH__: string | undefined;

// src/render/people/proceduralCrowd.ts:856-856 variable __PROCEDURAL_COOK_HASH__
declare const __PROCEDURAL_COOK_HASH__: string | undefined;

// src/render/people/proceduralCrowd.ts:858-859 variable LOD_PLAN_HASH
/** The fingerprint the pieces' levels are kept under (`derivedCache.ts`): the code that plans them */
const LOD_PLAN_HASH = typeof __CROWD_LOD_HASH__ !== 'undefined' && typeof __PROCEDURAL_COOK_HASH__ !== 'undefined'
  && __CROWD_LOD_HASH__ && __PROCEDURAL_COOK_HASH__ ? `${__CROWD_LOD_HASH__}-${__PROCEDURAL_COOK_HASH__}` : null;
