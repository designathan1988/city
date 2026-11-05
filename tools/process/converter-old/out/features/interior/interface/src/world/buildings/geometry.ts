// What the feature uses of src/world/buildings/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/geometry.ts:23-23 variable GRID
/** Every spatial question about a building, answered from the record. */

/** The step every horizontal dimension snaps to in the editor: half a metre. */
export const GRID = m(0.5);

// src/world/buildings/geometry.ts:25-25 variable MIN_SIZE
/** The smallest a volume may be on either side. */
export const MIN_SIZE = m(2);

// src/world/buildings/geometry.ts:27-27 variable EPS
/** Two lengths closer than this (0.04 mm) are the same one (shared walls, touching volumes). */
export const EPS = 1e-4 * TUNED_UNIT;

// src/world/buildings/geometry.ts:30-30 type Frame
/** What a building's frame is read from: its origin and rotation (a worker is sent no more, `sim/agents/lotBays.ts`). */
type Frame = Pick<Building, 'x' | 'y' | 'rotation'>;

// src/world/buildings/geometry.ts:32-36 function localToWorld
export function localToWorld(b: Frame, lx: number, ly: number): Vec2;

// src/world/buildings/geometry.ts:39-44 interface BuildingFrame
/** DO NOT call `localToWorld` point by point in a building's loops */
export interface BuildingFrame {
  readonly x: number;
  readonly y: number;
  readonly c: number;
  readonly s: number;
}

// src/world/buildings/geometry.ts:46-46 function buildingFrame
export const buildingFrame = (b: Building): BuildingFrame => …;

// src/world/buildings/geometry.ts:48-48 function frameX
/** World x of a local point, as `localToWorld` gives it. */
export const frameX = (f: BuildingFrame, lx: number, ly: number): number => …;

// src/world/buildings/geometry.ts:50-50 function frameY
/** World y of a local point, as `localToWorld` gives it. */
export const frameY = (f: BuildingFrame, lx: number, ly: number): number => …;

// src/world/buildings/geometry.ts:59-65 function worldToLocal
export function worldToLocal(b: Frame, p: Vec2): Vec2;

// src/world/buildings/geometry.ts:73-77 function localDirToWorld
/** A local direction turned into the world. */
export function localDirToWorld(b: Frame, dx: number, dy: number): Vec2;

// src/world/buildings/geometry.ts:80-85 variable SIDE_NORMAL
/** Outward unit normal of a side, in the local frame. */
export const SIDE_NORMAL: Readonly<Record<Side, Vec2>> = {
  0: { x: 0, y: -1 },
  1: { x: 1, y: 0 },
  2: { x: 0, y: 1 },
  3: { x: -1, y: 0 },
};

// src/world/buildings/geometry.ts:90-94 function levelHeight
/** Height of level `L`, world units. */
export function levelHeight(b: Building, level: number): number;

// src/world/buildings/geometry.ts:97-101 function levelElevation
/** Height of the floor of level `L` above the building's ground floor. */
export function levelElevation(b: Building, level: number): number;

// src/world/buildings/geometry.ts:104-108 function topLevel
/** Highest level any volume reaches (one past the top storey). */
export function topLevel(b: Building): number;

// src/world/buildings/geometry.ts:114-114 function volumeLift
/** How far a block's own floor stands from the building's ground floor (`Volume.lift`). */
export const volumeLift = (v: Volume): number => …;

// src/world/buildings/geometry.ts:117-117 function volumeElevation
/** Height of the floor of a block's level `level` above the building's ground floor, its lift included. */
export const volumeElevation = (b: Building, v: Volume, level: number): number => …;

// src/world/buildings/geometry.ts:120-128 function liftAt
/** The lift of the block a point of the plan is in, on `level`: where a floor's furniture, partitions and cores stand */
export function liftAt(b: Building, level: number, x: number, y: number): number;

// src/world/buildings/geometry.ts:148-150 function volumeRectLocal
/** Local rectangle of a volume, world units: [x0, y0, x1, y1]. */
export function volumeRectLocal(_b: Building, v: Volume): [number, number, number, number];

// src/world/buildings/geometry.ts:153-162 function volumeCorners
/** The four world corners of a volume, anticlockwise in the local frame. */
export function volumeCorners(b: Building, v: Volume, grow = 0): Vec2[];

// src/world/buildings/geometry.ts:168-168 function isMass
/** A block that is mass (solid or exclusive), not a cut or a clip of others. */
export const isMass = (v: Volume): boolean => …;

// src/world/buildings/geometry.ts:171-173 function solidFootprints
/** World corner rings of the BUILT ground volumes: what a person cannot walk through. */
export function solidFootprints(b: Building, grow = 0): Vec2[][];

// src/world/buildings/geometry.ts:176-178 function footprintRects
/** World corner rings of every ground volume: the building's footprint. */
export function footprintRects(b: Building, grow = 0): Vec2[][];

// src/world/buildings/geometry.ts:181-196 function footprintBox
/** Local bounding box of the footprint, world units. */
export function footprintBox(b: Building): { x0: number; y0: number; x1: number; y1: number };

// src/world/buildings/geometry.ts:205-216 function elementRect
/** An element's plan, local frame: [x0, y0, x1, y1]. `w` runs across its facing, `d` along it. */
export function elementRect(e: BuildingElement): [number, number, number, number];

// src/world/buildings/geometry.ts:219-267 function buildingBounds
/** World bounding box of a building: its volumes and its elements. */
export function buildingBounds(b: Building, grow = 0): { minX: number; minY: number; maxX: number; maxY: number };

// src/world/buildings/geometry.ts:269-269 type Bounds
type Bounds = { readonly minX: number; readonly minY: number; readonly maxX: number; readonly maxY: number };

// src/world/buildings/geometry.ts:277-283 function storedBounds
/** `buildingBounds` of a record held by the store, worked out once per record and margin. */
export function storedBounds(b: Building, grow = 0): Bounds;

// src/world/buildings/geometry.ts:294-304 function isSupported
/** Whether a volume above the ground has something under every part of it */
export function isSupported(b: Building, v: Volume): boolean;

// src/world/buildings/geometry.ts:315-316 function baysOn
/** Bays on a side: as many as fit at about one module each, at least one. */
export const baysOn = (b: Building, v: Volume, side: FaceId): number => …;

// src/world/buildings/geometry.ts:319-319 function bayWidth
/** Width of every bay of a side: the side shared evenly. */
export const bayWidth = (b: Building, v: Volume, side: FaceId): number => …;

// src/world/buildings/geometry.ts:322-330 function sideStart
/** Local start of a side (where its along coordinate is 0) and its along direction. */
export function sideStart(v: Volume, side: FaceId): { x: number; y: number; tx: number; ty: number };

// src/world/buildings/geometry.ts:471-495 function groundProjections
/** World rings of the projections that stand on the ground (from the first storey of a ground volume) */
export function groundProjections(b: Building, grow = 0): Vec2[][];

// src/world/buildings/geometry.ts:505-521 interface BayOf
/** What one exposed bay (or piece of one) of one storey is wherever its building stands. */
interface BayOf {
  readonly volume: number;
  /** Storey index within the volume. */
  readonly storey: number;
  readonly level: number;
  readonly side: FaceId;
  readonly index: number;
  readonly component: BayComponent;
  /** Height of the bay's floor above the building's ground floor. */
  readonly z: number;
  /** Where the piece starts along the side, local units from the side's start. */
  readonly start: number;
  /** How far the bay's plane stands out from the side (negative: set back), by a relief. */
  readonly push: number;
  readonly width: number;
  readonly height: number;
}

// src/world/buildings/geometry.ts:524-531 interface FacadeBay
/** One exposed bay (or piece of one) of one storey: everything a mesh builder or a picker needs. */
export interface FacadeBay extends BayOf {
  /** World point at the bottom centre of the bay (of the piece), on the facade line. */
  readonly x: number;
  readonly y: number;
  /** Outward normal, world. */
  readonly nx: number;
  readonly ny: number;
}

// src/world/buildings/geometry.ts:542-548 interface Rhythm
/** What a building's bays and floors are laid by besides its blocks: the bay module across, the storey heights up (`levelHeight`). */
export interface Rhythm {
  readonly module: number;
  readonly groundHeight: number;
  readonly storeyHeight: number;
  /** A copy of `Building.levels`: a draft's list changed in place does not change what was keyed by it. */
  readonly levels: readonly (number | null)[] | undefined;
}

// src/world/buildings/geometry.ts:550-551 function rhythmOf
export const rhythmOf = (b: Building): Rhythm => …;

// src/world/buildings/geometry.ts:554-562 function sameRhythm
/** Whether a building has this rhythm: every number the same (`Object.is`), level by level. */
export function sameRhythm(r: Rhythm, b: Building): boolean;

// src/world/buildings/geometry.ts:565-566 variable closedBlocks
/** A building's closed blocks (an open one is a lot, laid on the ground): one list for every record with the same blocks, the blocks themselves when none is open */
export const closedBlocks: (volumes: Volume[]) => Volume[] = identityMemo((volumes: Volume[]): Volume[] =>
  (volumes.some((v) => v.open) ? volumes.filter((v) => !v.open) : volumes));

// src/world/buildings/geometry.ts:739-739 variable DEFAULT_PITCH
/** Default roof pitches, degrees. */
export const DEFAULT_PITCH: Readonly<Record<string, number>> = { gable: 30, hip: 30, shed: 12, sawtooth: 35 };

// src/world/buildings/geometry.ts:748-748 function ridgeAlongX
/** Whether a gable or hip ridge runs along the local x axis: as set, else along the longer side. */
export const ridgeAlongX = (v: Volume): boolean => …;

// src/world/buildings/geometry.ts:757-772 function roofRise
/** How far a volume's roof rises above its top floor, world units. */
export function roofRise(b: Building, v: Volume): number;

// src/world/buildings/geometry.ts:775-792 function roofHeightAt
/** Height over the eaves at one local roof point, shared by roof parts. */
export function roofHeightAt(b: Building, v: Volume, p: Vec2): number;

// src/world/buildings/geometry.ts:795-795 function volumeHeight
/** Height of a volume's top (eaves), above the building's ground floor, its lift included. */
export const volumeHeight = (b: Building, v: Volume): number => …;
