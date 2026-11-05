// What the feature uses of src/render/buildings/signature.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/signature.ts:32-32 type Mat
/** Signature buildings: a building of the tower kit drawn with parts of its own */

/** A material's name: the shared ones of `makeMaterials`, or a facade made by `facade`. */
type Mat = string;

// src/render/buildings/signature.ts:34-34 variable materials
let materials: Record<Mat, Material> | null = null;

// src/render/buildings/signature.ts:36-36 variable facades
/** One material a facade style: the UV rhythm (bays, storeys, base) is baked into each piece's uv when it is pushed, so a building does not need a material and a texture of its own. */
const facades = new Map<string, Material>();

// src/render/buildings/signature.ts:42-46 function seedOf
/** A seed from a name (FNV-1a): the same pictures in every run. */
const seedOf = (name: string): number =>;

// src/render/buildings/signature.ts:48-50 type FacadeKind
type FacadeKind = 'curtainBlue' | 'curtainDark' | 'curtainGreen' | 'punchedBeige' | 'punchedCream' | 'gridDark' | 'brick' | 'stoneGrid'
  | 'archTerracotta' | 'ribbonWhite' | 'squareConcrete' | 'navyGrid' | 'copperFins' | 'ribbonBlack' | 'punchedPink' | 'curtainTeal'
  | 'archBrick' | 'archStone' | 'decoStripes' | 'loftBrick' | 'tallStone' | 'schoolBrick' | 'gothicStone' | 'officeWhite';

// src/render/buildings/signature.ts:187-197 function canvasTexture
function canvasTexture(size: number, paint: (g: CanvasRenderingContext2D, s: number) => void, repeatX = 1, repeatY = 1): CanvasTexture;

// src/render/buildings/signature.ts:361-611 class Parts
/** Collects parts by material, in metres of the building's frame. */
class Parts { … }

// src/render/buildings/signature.ts:362-362 field Parts.byMat
readonly byMat = new Map<Mat, BufferGeometry[]>();

// src/render/buildings/signature.ts:721-740 interface TowerLook
/** How one of the towers of the reference sheets is made. */
interface TowerLook {
  /** The shaft's face: a facade picture, its bay width. */
  readonly face: FacadeKind;
  readonly bay: number;
  readonly podium: Mat;
  readonly round?: boolean;
  /** Projecting piers, every `every` metres, and their material. */
  readonly piers?: { readonly mat: Mat; readonly every: number; readonly w: number };
  /** A slab edge at every floor (`deep` > 0.5: a balcony band with its railing). */
  readonly bands?: { readonly mat: Mat; readonly deep: number; readonly rail?: 'glassRail' | 'railing'; readonly sides?: boolean };
  /** A strip up the middle of the front, standing forward, one storey higher. */
  readonly strip?: { readonly mat: Mat; readonly frac: number };
  /** Balconies in some columns only (classic towers). */
  readonly loggias?: { readonly every: number };
  /** Storeys of the set-back crown; tiers of an art deco top. */
  readonly crown: number;
  readonly tiers?: number;
  /** A cornice band at the crown. */
  readonly cornice?: Mat;
}

// src/render/buildings/signature.ts:742-755 variable LOOKS
const LOOKS: Record<string, TowerLook> = {
  glassOffice: { face: 'curtainBlue', bay: 1.5, podium: 'stoneLight', piers: { mat: 'stoneLight', every: 6, w: 0.9 },
    strip: { mat: 'curtainStrip', frac: 0.24 }, crown: 2, cornice: 'stoneLight' },
  glassBalcony: { face: 'curtainBlue', bay: 1.5, podium: 'stoneLight', bands: { mat: 'white', deep: 1.4, rail: 'glassRail', sides: true },
    strip: { mat: 'stoneLight', frac: 0.14 }, crown: 2 },
  beigeClassic: { face: 'punchedBeige', bay: 1.8, podium: 'stoneLight', loggias: { every: 3 }, crown: 3, tiers: 2, cornice: 'trim' },
  darkGlass: { face: 'curtainDark', bay: 1.5, podium: 'darkPanel', crown: 4, tiers: 2 },
  whiteBalcony: { face: 'curtainBlue', bay: 1.5, podium: 'stoneLight', bands: { mat: 'white', deep: 1.6, rail: 'glassRail', sides: true }, crown: 1 },
  brickFrame: { face: 'curtainBlue', bay: 1.5, podium: 'stone', piers: { mat: 'brickPier', every: 3, w: 0.8 },
    bands: { mat: 'brickPier', deep: 0.35 }, crown: 1, cornice: 'brickPier' },
  roundGlass: { face: 'curtainBlue', bay: 1.5, podium: 'stoneLight', round: true, bands: { mat: 'white', deep: 0.6 }, crown: 1 },
  darkGrid: { face: 'gridDark', bay: 2.4, podium: 'darkPanel', piers: { mat: 'lightGrid', every: 4.8, w: 0.35 }, crown: 2 },
  artDeco: { face: 'punchedBeige', bay: 1.5, podium: 'stoneLight', piers: { mat: 'beige', every: 1.5, w: 0.35 }, crown: 3, tiers: 3, cornice: 'beige' },
};

// src/render/buildings/signature.ts:758-897 function towerOf
/** A tower of the reference sheet: podium, shaft, crown and roof, in its own look. */
function towerOf(look: TowerLook);

// src/render/buildings/signature.ts:901-901 type Design
type Design = (p: Parts, W: number, D: number, g: number, s: number, upper: number) => void;

// src/render/buildings/signature.ts:1740-1746 variable DESIGNS
const DESIGNS: Partial<Record<string, Design>> = {
  balconyMid,
  ...Object.fromEntries(Object.entries(LOOKS).map(([kind, look]) => [kind, towerOf(look)])),
  twistGreen, hexTerracotta, stepGarden, waveWhite, cubeBalcony, twinNavy, octCopper, stackedBlocks, pinkCorner, triangleDark,
  cityHallBrick, decoSpire, hospitalA, loftBrick, cityHallWhite, forum, church, bank, school, fireStation, police, hospitalB, cemetery,
  ...LANDMARKS,
};

// src/render/buildings/signature.ts:1749-1752 function drawsSignature
/** Whether this module draws the building's body. */
export function drawsSignature(b: Building): boolean;
