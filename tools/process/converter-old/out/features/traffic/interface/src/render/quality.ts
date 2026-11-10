// What the feature uses of src/render/quality.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/quality.ts:5-5 type QualityLevel
export type QualityLevel = 'low' | 'medium' | 'high' | 'ultra';

// src/render/quality.ts:8-8 variable VEHICLE_ERROR_PIXELS
/** What every tier above the weak one draws people and vehicles with, as before tiers chose them (`vehicleLevels.ts`, `agents.ts`, `proceduralCrowd.ts`). */
export const VEHICLE_ERROR_PIXELS = 0.5;

// src/render/quality.ts:9-9 variable VEHICLE_WHOLE_CAP
export const VEHICLE_WHOLE_CAP = 200;

// src/render/quality.ts:10-10 variable HAIR_STRAND_PEOPLE
export const HAIR_STRAND_PEOPLE = 8;

// src/render/quality.ts:12-68 interface QualitySettings
export interface QualitySettings {
  /** Cap on the device pixel ratio the canvas is rendered at. */
  readonly pixelRatio: number;
  readonly shadows: boolean;
  readonly shadowMapSize: number;
  readonly postProcessing: boolean;
  readonly ambientOcclusion: boolean;
  readonly smaa: boolean;
  /** Samples of the scene's multisampled target (MSAA), as many as the GPU has at most. */
  readonly msaa: number;
  /** Ground-truth ambient occlusion at the scene's full resolution, its denoise too; false: at half, scaled up. */
  readonly aoFull: boolean;
  /** AMD FidelityFX Contrast Adaptive Sharpening on the final image, 0 (off) to 1 (strongest). */
  readonly sharpen: number;
  /** Maximum anisotropic filtering taps requested for ground textures. */
  readonly anisotropy: number;
  /** Draw street furniture, kerb detail, parapets and roadside props. */
  readonly detailProps: boolean;
  /** Scatter vegetation over the terrain. */
  readonly vegetation: number;
  /** Blades in the GPU grass field round the camera (`grass.ts`); 0 draws none. */
  readonly grassBlades: number;
  /** The close-zoom texture detail layer (`mesh/detailLayer.ts`). */
  readonly surfaceDetail: boolean;
  /** Zoom below which markings, props and agents stop being drawn. */
  readonly detailCutoffZoom: number;
  /** Crowd detail ceiling: silhouette, wardrobe, or full facial animation. */
  readonly pedestrianDetail: 0 | 1 | 2;
  /** Zoom from which the people inside vehicles are drawn. */
  readonly occupantZoom: number;
  /** A vehicle part's level: the coarsest whose error on the screen stays under this many pixels drawn (`vehicleLevels.ts` ERROR_PIXELS on every tier but the weak one). */
  readonly vehicleErrorPixels: number;
  /** Vehicles drawn whole (body, cabin, wheels apart) at most: the biggest on the screen (`agents.ts`). */
  readonly vehicleWholeCap: number;
  /** People nearest the camera drawn with their hair's strands (`proceduralCrowd.ts`). */
  readonly hairStrandPeople: number;
  /** Soft shadows of drifting clouds over the land (`postprocess.ts`, needs post-processing). */
  readonly cloudShadows: boolean;
  /** The ecosystem's vegetation (`render/nature/`) */
  readonly vegetationDistance: number;
  /** Far plants as impostors - billboards baked from each species. */
  readonly impostors: boolean;
  /** Most birds in the air at once; 0 draws none. */
  readonly birds: number;
  /** Raindrops in the air (the wet ground is drawn on every tier). */
  readonly rain: boolean;
  /** The cloud layer of the play sky. */
  readonly skyClouds: boolean;
  /** Morning mist in the valleys and over the water. */
  readonly mist: boolean;
  /** The spray and mist at the foot of waterfalls (their falling water is drawn on every tier). */
  readonly waterfalls: boolean;
  /** Share of the view's budget of triangles the buildings' levels may add up to (`core/perfBudget.ts` VIEW_BUDGETS, `buildings/frameBudget.ts`): past it their error scale rises. */
  readonly triangleScale: number;
  /** The most the buildings' cells' detail may hold on the GPU, MB (`BuildingLayer.setDetailBudget`). */
  readonly detailMemoryMB: number;
}

// src/render/quality.ts:101-109 interface GeometryBudget
/**
 * THE GEOMETRY EACH TIER MAY HOLD ON THE GPU (vertex and index buffers, `memoryCounters.ts` geometryBytes; textures and render targets
 * apart), for cards of 4 to 12 GB: the whole under `totalMB`, the buildings' masses (cells, 1 km tiles, 2 km plates, and the base
 * kept for good, `massCells.ts`) under `massesMB`, the roads' blocks at their full level under `roadsNearMB` and at their far level under
 * `roadsFarMB` (`mesh/roadResidency.ts`: past it a cell's blocks go and the network's far level, `networkFarRoads.ts`, draws it), the far props' and poles' blocks (`viewCells.ts` gpuBudget, 192 MB and 128 MB at ultra) at `viewCellsShare` of their
 * own. What is out of the view goes first, the least recently seen first (Cesium's `cacheBytes` LRU, 3DTilesRendererJS `lruCache`); the
 * buildings' detail (`detailMemoryMB`) gives way to its facade level and masses only when that is not enough (`renderer.ts` governGeometry).
 *
 * | tier   | total | masses | view cells | roads near | roads far | detail at most |
 * |--------|-------|--------|------------|------------|-----------|----------------|
 * | ultra  | 1200  | 320    | 1          | 192        | 160       | 1024           |
 * | high   | 900   | 240    | 0.75       | 128        | 128       | 768            |
 * | medium | 600   | 160    | 0.5        | 96         | 96        | 448            |
 * | low    | 448   | 96     | 0.25       | 48         | 48        | 256            |
 *
 * LOW AT 448: the governor's whole is every holder of the scene (`countMemory` over the scene, what `__roadcraft.memory()` reports:
 * the people, vehicles, props and roads with the buildings). The tiles and plates IN the view that their children draw are let go for
 * the masses' budget too (`massCells.ts` evictLevels): they stood beside them, 125 MB of the 244 MB of masses of the city of 25,000
 * buildings' district view on low. With them gone the game's district view at 1920x1080 holds 425 MB (506 before), the street 404, the
 * map 343 (step-25k on low, 2026-10-21), the Node tour at 1280x720 199 to 311 (`tests/perf/geometryBudget.spec.ts` BUDGET_TIER=low,
 * none chosen coarser). What is left at the district is the view's own need with the detail at its least (21 MB): the cells' masses of
 * the 1.4 km the player keeps every facade in (`DETAIL_REACH`, 132 MB, the triangle budget holding most of them far), the roads (89),
 * the people (23), the far gardens (22). 400 could only be met by drawing that reach in tiles: the budget is set to the need, with room.
 * The whole GPU on low adds the textures (~400 MB in the game) and no render target of its own (no post chain): some 0.85 GB of the
 * 1.4 GB the device's cap gives the weak machine (`gpuLedger.ts` deviceGpuCap: an UHD 770 sharing 8 GB, at 1080p).
 *
 * THE ROADS' WHOLE (`roadBudgetMB`): their near and far levels' budgets, and the network's far level kept for good for the whole map.
 * That last one grows with the map, not with the view: 10 MB for the city of 25,000 buildings (`tests/perf/noHoles.spec.ts`, 21 bytes a
 * vertex), some 80 MB for one of 200,000 by its area; the blocks' far levels it stands for took 264 MB for the 25,000 (2.1 GB for the
 * 200,000), every block of the map kept.
 */
export interface GeometryBudget {
  readonly totalMB: number;
  readonly massesMB: number;
  readonly viewCellsShare: number;
  /** The roads' blocks at their full level (`mesh/roadResidency.ts`): only in the view and under this. */
  readonly roadsNearMB: number;
  /** The roads' blocks at their far level (`mesh/roadResidency.ts`): the cells nearest the view in it, past it the network's far level. */
  readonly roadsFarMB: number;
}
