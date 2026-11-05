// What the feature uses of src/render/terrain.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/terrain.ts:120-123 type TerrainSource
/** What the surface reads of the document: one plate's share of it, in the plate's own coordinates (`flatAtlas.ts`). */
export type TerrainSource = Pick<RoadDoc, 'changes' | 'terrainRevision' | 'terrainStamps' | 'terrainRelief' | 'natureRevision' | 'nature' | 'paintRevision' | 'terrainPaint'> & {
  /** The map's relief under the stamps, in the plate's own coordinates. */
  readonly terrainGround?: (x: number, y: number) => number;
};

// src/render/terrain.ts:125-208 interface TerrainSurface
export interface TerrainSurface {
  readonly meshes: readonly Object3D[];
  readonly ground: Mesh;
  /** Where the sun is (a direction towards it, three's axes) */
  setSun(direction: { readonly x: number; readonly y: number; readonly z: number }): void;
  /** Once a frame, after `focus`: the map's far woods (`flatAtlas.ts`, `farForest.ts`). */
  bakeRelief(renderer: WebGLRenderer, focus: { readonly x: number; readonly z: number } | null): void;
  /** How the rivers and lakes look and move: waves, foam, current (`render/water.ts` WaterLook). */
  setWaterLook(look: WaterLook): void;
  /** The gullies the player cut or wiped, and how much of the steep land carries them of itself (`world/gullies.ts`). */
  setGullies(dabs: readonly GullyDab[], auto: number): void;
  /** The terrain's own surface for the batter from a footway down to the ground (`roadSurfaces.ts`) */
  readonly vergeMaterial: Material;
  /** The land's material for an ordinary mesh of the ground: the far patches (`flatAtlas.ts`). */
  readonly meshMaterial: Material;
  /** Once a frame, before drawing: the patches the camera needs, `pixels` the drawing buffer's height (`terrainLod.ts`). */
  lookFrom?(camera: PerspectiveCamera | OrthographicCamera, pixels: number): void;
  /** The analytic height field: the smooth surface the stamps describe. */
  heightAt(x: number, y: number): number;
  /** The ground as DRAWN, before any road cut or filled it. */
  naturalRenderedHeightAt(x: number, y: number): number;
  /** A digest of everything `renderedHeightAt` and `naturalRenderedHeightAt` read inside a rectangle */
  digest(minX: number, minY: number, maxX: number, maxY: number): number;
  /** The height the terrain is actually DRAWN at. */
  renderedHeightAt(x: number, y: number): number;
  /** Whether a point is under a river's water. */
  wetAt(x: number, y: number): boolean;
  /** Rewrites the heightfield from the document's stamps alone. */
  update(doc: TerrainSource, stroking?: boolean): boolean;
  /** Brings the painted ground up to `doc.paintRevision`. */
  updatePaint(doc: TerrainSource, deferEcology?: boolean): void;
  /** Whether the ecosystem waits to be read again (`settleEcology`). */
  readonly ecologyPending?: boolean;
  /** Reads the ecosystem again if it waits to be: true when it did. */
  settleEcology?(): boolean;
  /** How much forest was painted at a point, 0..1 (`world/terrainPaint.ts` 'forest'). */
  forestAt(x: number, y: number): number;
  /** How much of a cover (forest, scrub, flowers, rocks) was painted at a point, 0..1. */
  coverAt(kind: CoverKind, x: number, y: number): number;
  /** Which rock the land is made of at a point (the painted geology; granite where none was). */
  geologyAt(x: number, y: number): GeologyKind;
  /** What grows on the land (`world/ecology.ts`), or null on a map with no ecosystem. */
  ecology(): EcologyField | null;
  /** Where the painted geology changed: the stones on the ground take its colour. */
  readonly geologyChanges: GroundChanges;
  /** The water's level near a point (lake or river, within a few cells of it), or null. */
  shoreLevelAt(x: number, y: number): number | null;
  /** The grid cells (x0, x1, y0, y1) the last `update` rewrote, or null when it rewrote the whole plate */
  readonly lastRegion: TerrainRegion | null;
  /** After a stroke: what was deferred while it was held (the water) is brought up to date. */
  settle(): void;
  /** Cuts and fills the ground so it meets the roads. */
  /** `region`: one region or several, shaped in one pass */
  shapeToRoads(shape: TerrainShaper | null, region?: TerrainRegion | readonly TerrainRegion[] | null): boolean;
  /** `shapeToRoads` asked off the page (`groundShape.ts`): the corners its pass reads for `region` */
  askShape(region: TerrainRegion | readonly TerrainRegion[] | null, bounds: readonly Aabb[] | null): ShapeAsking;
  /** The plate's natural corners and the diagonals they are drawn with (`naturalRenderedHeightAt`), as kept: never written by the caller. */
  naturalCorners?(): { readonly natural: Float64Array; readonly diagonals: Uint8Array };
  /** The atlas's plates' natural ground a set of world boxes reads */
  naturalCut?(boxes: readonly (readonly [number, number, number, number])[]): NaturalPlate[];
  /** The grid corners a world box covers, as `shapeToRoads` takes them. */
  regionOf(box: readonly [number, number, number, number]): TerrainRegion;
  /** A region back as the world box it covers, a cell round. */
  rectOf(region: TerrainRegion): readonly [number, number, number, number];
  /** The plates the ground is made of, each with its own surface (`TerrainPart`). */
  readonly parts: readonly TerrainPart[];
  /** The highest corner of the drawn ground, when the surface keeps it as its ground moves (the atlas, from its plates') */
  highest?(): number;
  /** Told each time the levels of the drawn ground land after a dab or a shaping pass (`highest` to read again): they are measured off the page and come after it (`landStroke.ts`). */
  onLevels?: (() => void) | null;
  /** Where the view looks (world x, y) and how much ground it takes in (the screen's half height, world units) */
  focus?(x: number, y: number, reach: number): void;
  /** Whether a plate the view wants still waits to be shown (its corners from the workers, its programs, the refresh that shows it): the frames must go on meanwhile, or a still camera keeps its far patch. */
  waiting?(): boolean;
  /** The vegetation the quality tier allows (`quality.ts` vegetation, as the near trees' budget reads it) */
  setVegetation?(amount: number): void;
  /** The plates the quality tier holds in full (`quality.ts`): drawn round the view at most, and made but not drawn kept at most (`flatAtlas.ts`; 9 and 9 unless a tier says fewer). */
  setPlates?(near: number, kept: number): void;
  /** The woods' density but the ecosystem's share per cell, made off the page with the plate's first build */
  woodsBase?(): Float32Array | null;
  /** Moves whenever the woods' base changes (a stamp of the land redoes its cells). */
  readonly woodsRevision?: number;
  dispose(): void;
}

// src/render/terrain.ts:213-216 interface ShapeAsking
/** A shaping pass asked off the page (`TerrainSurface.askShape`). */
export interface ShapeAsking {
  readonly plates: readonly PlateShapeAsk[];
  take(answers: readonly PlateShapeAnswer[]): boolean;
}

// src/render/terrain.ts:219-232 interface PlateSurface
/** One plate's own ground: its own revisions and water, never merged over the plates shown (a product follows them plate by plate, `plateFollow.ts`). */
export interface PlateSurface extends TerrainSurface {
  /** Moves whenever this plate's `ecology()` changes. */
  readonly ecologyRevision: number;
  /** Where this plate's water is, in the plate's own (x, y), or null when it holds none. */
  waterArea(): { minX: number; maxX: number; minY: number; maxY: number } | null;
  /** Where this plate's water changed, its own (x, y): the box it had and the box it has at each rebuild. */
  readonly waterChanges: GroundChanges;
  /** What its textures and meshes hold on the GPU, bytes: what the atlas keeps a hidden plate by (`flatAtlas.ts`). */
  gpuBytes(): number;
  /** Its own textures (its corners, the paint and corner layers, the shore, the water's ground): sent ahead of its first draw (`uploads.ts`). */
  textures(): Texture[];
  /** Whether the view is near the plate (`flatAtlas.ts`): its painted layers at their full side only then (`fitPaint`). */
  paintDetail?(near: boolean): void;
}

// src/render/terrain.ts:235-239 interface TerrainPart
/** A plate of the ground and where its centre stands (`flatAtlas.ts`). */
export interface TerrainPart {
  readonly surface: PlateSurface;
  readonly cx: number;
  readonly cy: number;
}

// src/render/terrain.ts:242-250 function terrainBakes
/** The land's surfaces, baked at build time (`bake/terrainRecipes.ts`): the grass, the dirt, and the rocks as ONE texture array, a single texture unit for all of them */
function terrainBakes(anisotropy: number): TerrainBakes;

// src/render/terrain.ts:253-253 variable grassMean
/** The grass map's mean luma, linear: what the shader read as its last mip (`texture(map, uv, 14.0)`), taken once before the texels are let go (`surfaceTextures.ts`). */
let grassMean: number | null = null;

// src/render/terrain.ts:254-266 function meanLuma
function meanLuma(texture: Texture): number;

// src/render/terrain.ts:374-380 interface TerrainBakes
/** The land's baked textures (`terrainBakes`). */
interface TerrainBakes {
  /** The grass's maps, and its colour map's mean luma (`meanLuma`). */
  readonly grass: { readonly map: Texture; readonly normalMap: Texture; readonly mean: number };
  /** The detail's grass and soil, the rocks and the soil as the layers of one colour array and one normal array (`LAND_ROCK`, `LAND_DIRT`). */
  readonly landMaps: Texture;
  readonly landNormals: Texture;
}
