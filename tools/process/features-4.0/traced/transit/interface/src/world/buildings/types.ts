// What the feature uses of src/world/buildings/types.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/types.ts:9-9 type BuildingId
export type BuildingId = number & { readonly [BuildingIdBrand]: true };

// src/world/buildings/types.ts:15-15 variable BUILDING_USES
const BUILDING_USES = ['residential', 'commercial', 'industrial', 'mixed'] as const;

// src/world/buildings/types.ts:16-16 type BuildingUse
export type BuildingUse = (typeof BUILDING_USES)[number];

// src/world/buildings/types.ts:19-39 variable BAY_COMPONENTS
/** Replaceable facade components, one per bay. */
export const BAY_COMPONENTS = [
  'wall',
  'window',
  'sashWindow',
  'frenchWindow',
  'bayWindow',
  'ribbon',
  'wideWindow',
  'balcony',
  'door',
  'doubleDoor',
  'garageDoor',
  'shopfront',
  'loadingDoor',
  'pillar',
  // The facade library of the procedural buildings (`architecture.ts`)
  'loggia',
  'brise',
  'shutteredWindow',
  'gourmet',
] as const;

// src/world/buildings/types.ts:40-40 type BayComponent
export type BayComponent = (typeof BAY_COMPONENTS)[number];

// src/world/buildings/types.ts:42-42 variable FACADE_PATTERNS
export const FACADE_PATTERNS = ['residential', 'storefront', 'office', 'industrial', 'arcade', 'gallery', 'artDeco', 'artDecoCrown', 'observation'] as const;

// src/world/buildings/types.ts:43-43 type FacadePattern
export type FacadePattern = (typeof FACADE_PATTERNS)[number];

// src/world/buildings/types.ts:47-47 variable ROOF_KINDS
export const ROOF_KINDS = ['flat', 'terrace', 'gable', 'hip', 'shed', 'sawtooth'] as const;

// src/world/buildings/types.ts:48-48 type RoofKind
export type RoofKind = (typeof ROOF_KINDS)[number];

// src/world/buildings/types.ts:52-52 variable ROOF_DETAIL_KINDS
const ROOF_DETAIL_KINDS = ['solar', 'skylight', 'vent', 'chimney', 'waterTank', 'spire', 'lantern'] as const;

// src/world/buildings/types.ts:53-53 type RoofDetailKind
export type RoofDetailKind = (typeof ROOF_DETAIL_KINDS)[number];

// src/world/buildings/types.ts:56-71 interface RoofDetail
export interface RoofDetail {
  id: number;
  kind: RoofDetailKind;
  /** Position in the building's local plan, on the owning volume's roof. */
  x: number;
  y: number;
  rotation: number;
  w: number;
  d: number;
  /** Height above the roof, in world units; used by adjustable details. */
  h?: number;
  /** Optional flag on a spire or a lantern's mast; plain leaves a coloured banner for generic buildings. */
  flag?: 'none' | 'plain' | 'saoPaulo' | 'saoPauloState';
  /** The flag's own design (pattern and colours); absent, `flag` names a fixed one. */
  flagDesign?: FlagDesign;
}

// src/world/buildings/types.ts:74-74 type Side
/** A face of a volume, in the building's local frame: 0 front (local -y, the street side), 1 right (+x), 2 back */
export type Side = 0 | 1 | 2 | 3;

// src/world/buildings/types.ts:77-77 type FaceId
/** Any edge of an authored footprint. Cardinal `Side` is for oriented parts. */
export type FaceId = number;

// src/world/buildings/types.ts:79-79 variable CORE_KINDS
export const CORE_KINDS = ['stair', 'lift', 'stairLift'] as const;

// src/world/buildings/types.ts:80-80 type CoreKind
export type CoreKind = (typeof CORE_KINDS)[number];

// src/world/buildings/types.ts:82-82 variable SPACE_KINDS
export const SPACE_KINDS = ['unit', 'shop', 'office', 'workshop', 'corridor', 'lobby'] as const;

// src/world/buildings/types.ts:83-83 type SpaceKind
export type SpaceKind = (typeof SPACE_KINDS)[number];

// src/world/buildings/types.ts:86-93 interface Facade
/** A facade: the storey's default component, optional whole-side overrides, and single-bay overrides keyed `"side:index"`. */
export interface Facade {
  fill: BayComponent;
  /** Optional composition for this storey; side compositions take priority. */
  pattern?: FacadePattern;
  patterns?: Partial<Record<FaceId, FacadePattern>>;
  sides?: Partial<Record<FaceId, BayComponent>>;
  bays?: Record<string, BayComponent>;
}

// src/world/buildings/types.ts:96-104 interface Space
/** Extension point: a subdivision of a storey (a flat, a shop, a corridor). */
export interface Space {
  id: number;
  x: number;
  y: number;
  w: number;
  d: number;
  kind: SpaceKind;
  use?: BuildingUse;
}

// src/world/buildings/types.ts:106-114 interface Storey
export interface Storey {
  /** Overrides the building's use on this storey (mixed use). */
  use?: BuildingUse;
  facade: Facade;
  /** Face finishes for this floor, over mass and building finishes. */
  materials?: Partial<Record<FaceId, MaterialSpec>>;
  /** Extension point, see `Space`. */
  spaces?: Space[];
}

// src/world/buildings/types.ts:117-124 interface Relief
/** A region of one face pushed in or out */
export interface Relief {
  side: FaceId;
  bay0: number;
  bay1: number;
  storey0: number;
  storey1: number;
  depth: number;
}

// src/world/buildings/types.ts:127-139 interface FacadeGeometry
/** Measured facade rhythm on one physical face of a volume. */
export interface FacadeGeometry {
  /** Number of equally spaced structural bays along this face. */
  bays?: number;
  /** Fractions of each bay's width and storey's height used by openings. */
  windowWidth?: number;
  windowHeight?: number;
  /** Absolute height of the window sill above its floor, in world units. */
  sill?: number;
  /** Projecting vertical pier dimensions, in world units; depth 0 removes it. */
  pierWidth?: number;
  pierDepth?: number;
  pierEvery?: number;
}

// src/world/buildings/types.ts:142-142 variable DRESS_LINES
/** The horizontal articulation of a block, as an architectural style draws it */
export const DRESS_LINES = ['every', 'base', 'slab', 'none'] as const;

// src/world/buildings/types.ts:143-143 variable DRESS_CROWNS
export const DRESS_CROWNS = ['cornice', 'deco', 'slab', 'attic', 'none'] as const;

// src/world/buildings/types.ts:144-147 interface VolumeDress
export interface VolumeDress {
  lines?: (typeof DRESS_LINES)[number];
  crown?: (typeof DRESS_CROWNS)[number];
}

// src/world/buildings/types.ts:150-188 interface Volume
/** A rectangular block, standing on level `base`: a rectangle of the local frame, world units. */
export interface Volume {
  id: number;
  x: number;
  y: number;
  w: number;
  d: number;
  /** Optional simple counterclockwise polygon, normalized to x/y/w/d. */
  outline?: Vec2[];
  /** The generative composition last applied to this mass. */
  facadePattern?: FacadePattern;
  /** Independent bay and pier proportions on any face. */
  facadeGeometry?: Partial<Record<FaceId, FacadeGeometry>>;
  roofDetails?: RoofDetail[];
  base: number;
  roof: RoofKind;
  /** Bottom to top: storey k occupies level `base + k`. */
  storeys: Storey[];
  /** This volume's own walls and roof, over the building's (see `materials.ts`). */
  materials?: VolumeMaterials;
  /** Faces pushed in or out (see `Relief`). */
  reliefs?: Relief[];
  /** How its storeys are marked and its top finished (see `VolumeDress`); absent, a line at every floor and a plain cornice. */
  dress?: VolumeDress;
  /** Roof pitch in degrees, for pitched roofs; absent = the roof kind's default. */
  pitch?: number;
  /** Which way a pitched roof runs. */
  ridge?: 'x' | 'y';
  fall?: Side;
  /** How the block combines with the others, never by changing them: absent, it adds its space (union) */
  mode?: BlockMode;
  /** An open block: a lot, not a building */
  open?: LotSurface;
  /** An open block laid as a TERRACE of a hillside lot: this far above */
  terrace?: number;
  /** An open block laid as a RAMP: the drive of a hillside lot going down or up to a car park or a loading yard at a level of */
  ramp?: number;
  /** A block standing at a floor of its own on a hillside: this far above */
  lift?: number;
}

// src/world/buildings/types.ts:196-196 variable LOT_SURFACES
/** What an open block is laid with. */
export const LOT_SURFACES = ['grass', 'paving', 'gravel', 'sand', 'water', 'asphalt', 'concrete', 'pavers', 'tiles'] as const;

// src/world/buildings/types.ts:197-197 type LotSurface
export type LotSurface = (typeof LOT_SURFACES)[number];

// src/world/buildings/types.ts:200-213 variable BUILDING_FUNCTIONS
/** What a building is FOR, in the city: where people live, work, study, buy, eat, are treated, are buried. */
export const BUILDING_FUNCTIONS = [
  // homes
  'house', 'townhouse', 'apartments', 'residentialTower',
  // public services
  'cityHall', 'council', 'courthouse', 'postOffice', 'police', 'fireStation', 'hospital', 'clinic',
  'school', 'university', 'library', 'museum', 'prison', 'church', 'cemetery', 'busStation',
  // private: shops, food, services, nights out
  'shop', 'supermarket', 'mall', 'bank', 'pharmacy', 'bakery', 'restaurant', 'snackBar', 'bar',
  'nightclub', 'cinema', 'hotel', 'gym', 'club', 'gasStation', 'office',
  // work
  'factory', 'warehouse',
  // leisure
  'park', 'square', 'playground', 'sportsCourt',
] as const;

// src/world/buildings/types.ts:214-214 type BuildingFunction
export type BuildingFunction = (typeof BUILDING_FUNCTIONS)[number];

// src/world/buildings/types.ts:217-223 interface PlacedFurniture
/** One piece of furniture or a light placed by the player, in the building's local frame. */
export interface PlacedFurniture {
  readonly kind: string;
  readonly x: number;
  readonly y: number;
  /** The way it faces, radians (0 faces -y). */
  readonly angle: number;
}

// src/world/buildings/types.ts:225-225 type BlockMode
export type BlockMode = 'void' | 'intersect' | 'xor';

// src/world/buildings/types.ts:228-255 variable ELEMENT_KINDS
/** Free parts a building can be given besides its volumes. */
export const ELEMENT_KINDS = [
  'stair',
  'ramp',
  'pillar',
  'canopy',
  'wall',
  'slab',
  'pavement',
  'fence',
  'tree',
  'bench',
  'ac',
  'planter',
  'railing',
  'awning',
  'flowers',
  'rocks',
  'parking',
  'clock',
  'hedge',
  'shrub',
  // Lot furniture: a gate in a boundary
  'gate',
  'bin',
  'lamp',
  'bollard',
  'drain',
] as const;

// src/world/buildings/types.ts:256-256 type ElementKind
export type ElementKind = (typeof ELEMENT_KINDS)[number];

// src/world/buildings/types.ts:259-272 interface BuildingElement
/** A free part of a building: a flight of stairs, a ramp, a pillar, a canopy, a free-standing wall, a slab. */
export interface BuildingElement {
  id: number;
  kind: ElementKind;
  x: number;
  y: number;
  facing: Side;
  w: number;
  d: number;
  z: number;
  h: number;
  /** Rotation of the box about its own centre, radians in the building's local frame. */
  angle?: number;
  material?: MaterialSpec;
}

// src/world/buildings/types.ts:275-282 interface Core
/** Extension point: a vertical circulation shaft, a module square whose corner is at `(x, y)` in the local frame. */
export interface Core {
  id: number;
  x: number;
  y: number;
  kind: CoreKind;
  from: number;
  to: number;
}

// src/world/buildings/types.ts:284-328 interface Building
export interface Building {
  readonly id: BuildingId;
  schema: number;
  /** World position of the local frame's origin (cell 0,0's corner). */
  x: number;
  y: number;
  /** Radians, counter-clockwise, of the local +x axis. */
  rotation: number;
  use: BuildingUse;
  /** The width a facade bay aims at, world units: each side is shared into bays of about this. */
  module: number;
  /** Height of level 0, world units. */
  groundHeight: number;
  /** Height of every level above 0, world units. */
  storeyHeight: number;
  /** Per-level height overrides, building-wide; `null`/absent = default. */
  levels?: (number | null)[];
  /** Colour scheme: the default material of every surface (`PALETTE_MATERIALS`). */
  palette: number;
  /** Building-wide materials, over the palette (see `materials.ts`). */
  materials?: BuildingMaterials;
  volumes: Volume[];
  /** Free parts: stairs, ramps, pillars, canopies, walls, slabs. */
  elements?: BuildingElement[];
  cores: Core[];
  /** What the building is for (absent: a plain building of its `use`). */
  function?: BuildingFunction;
  /** When it was built or last renovated, city minutes; absent: it does not age. */
  builtAt?: number;
  /** The version of the lot generator a zoned building was grown with (`LOT_PLAN_VERSION`). */
  lotPlan?: number;
  /** How run-down it is, 0 (new) to 1 (falling apart), from its age (`decayOf`). */
  decay?: number;
  /** The furniture and lights of each floor as the player arranged them, by level. */
  furnishing?: Record<string, PlacedFurniture[]>;
  /** Drawing only, never stored: the building cut open above this level, to show its inside */
  cutaway?: number;
  /** Drawing only: the way the camera looks (world, horizontal), for which walls of the cut floor come down. */
  cutView?: { readonly x: number; readonly y: number };
  nextVolumeId: number;
  nextElementId?: number;
  name?: string;
  /** The blueprint this building was placed from, for the UI only. */
  blueprint?: string;
}
