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

// src/render/quality.ts:122-264 variable QUALITY
export const QUALITY: Readonly<Record<QualityLevel, QualitySettings>> = {
  low: {
    pixelRatio: 1,
    shadows: false,
    shadowMapSize: 1024,
    postProcessing: false,
    ambientOcclusion: false,
    smaa: false,
    msaa: 0,
    aoFull: false,
    sharpen: 0,
    anisotropy: 2,
    detailProps: false,
    vegetation: 0,
    grassBlades: 0,
    surfaceDetail: false,
    detailCutoffZoom: perM(1.25),
    pedestrianDetail: 0,
    occupantZoom: perM(7.5),
    // The weak machine's integrated GPU: parts a pixel off, fewer whole vehicles, strands on the two nearest (the other tiers draw as they always did).
    vehicleErrorPixels: 1,
    vehicleWholeCap: 60,
    hairStrandPeople: 2,
    cloudShadows: false,
    vegetationDistance: m(180),
    impostors: true,
    birds: 0,
    rain: false,
    skyClouds: false,
    mist: false,
    waterfalls: false,
    triangleScale: 0.35,
    detailMemoryMB: 256,
  },
  medium: {
    pixelRatio: 1.25,
    shadows: true,
    shadowMapSize: 1024,
    postProcessing: false,
    ambientOcclusion: false,
    smaa: false,
    msaa: 0,
    aoFull: false,
    sharpen: 0,
    anisotropy: 4,
    detailProps: true,
    vegetation: 600,
    grassBlades: 30_000,
    surfaceDetail: true,
    detailCutoffZoom: perM(0.85),
    pedestrianDetail: 1,
    occupantZoom: perM(5.5),
    vehicleErrorPixels: VEHICLE_ERROR_PIXELS,
    vehicleWholeCap: VEHICLE_WHOLE_CAP,
    hairStrandPeople: HAIR_STRAND_PEOPLE,
    cloudShadows: false,
    vegetationDistance: m(300),
    impostors: true,
    birds: 150,
    rain: true,
    skyClouds: true,
    mist: false,
    waterfalls: true,
    triangleScale: 0.6,
    detailMemoryMB: 448,
  },
  high: {
    pixelRatio: 1.5,
    shadows: true,
    // Three cascades of 2048: the nearest a few centimetres a texel round the street looked at
    shadowMapSize: 2048,
    postProcessing: true,
    ambientOcclusion: true,
    smaa: true,
    // SMAA smooths every edge on its own: the scene's MSAA samples cost 2.2 ms of the 2560-wide frame for edges SMAA draws as well.
    msaa: 0,
    aoFull: false,
    // No sharpening at the display's own size: see ultra.
    sharpen: 0,
    anisotropy: 8,
    detailProps: true,
    vegetation: 1_400,
    grassBlades: 60_000,
    surfaceDetail: true,
    detailCutoffZoom: perM(0.65),
    pedestrianDetail: 2,
    occupantZoom: perM(4),
    vehicleErrorPixels: VEHICLE_ERROR_PIXELS,
    vehicleWholeCap: VEHICLE_WHOLE_CAP,
    hairStrandPeople: HAIR_STRAND_PEOPLE,
    cloudShadows: true,
    vegetationDistance: m(440),
    impostors: true,
    birds: 400,
    rain: true,
    skyClouds: true,
    mist: true,
    waterfalls: true,
    triangleScale: 1,
    detailMemoryMB: 768,
  },
  ultra: {
    pixelRatio: 2,
    shadows: true,
    // DO NOT raise it to 3072 with MSAA 4x and full-resolution GTAO (80074433): the picture came out washed out, 18 % of the district's pixels changed against the approved look (bisected, 2026-10-22).
    shadowMapSize: 2048,
    postProcessing: true,
    ambientOcclusion: true,
    // Kept over the MSAA: it smooths what multisampling cannot (the shading's own edges - window grids, specular glints, cut-out leaves).
    smaa: true,
    // DO NOT turn on 4x here for the district's sparkle: measured alone it cost ~4 ms of GPU there (p95 13 -> 17 ms) and left 32 % of its
    // pixels off a supersampled frame against 34 % (`npm run diagnose`, 2026-10-22): the sparkle is the shading's, not the edges'.
    msaa: 0,
    // At the frame's own size, alone (not with the 3072 shadow nor the MSAA of 80074433): at half size the occlusion made the district's
    // frame differ from a supersampled one in 34 % of its pixels, 27 % without it (`npm run diagnose`, 2026-10-22, per-pass ablation).
    // Its GPU paid back by the parts' coarser levels (`partCells.ts` PART_ERROR_PX).
    aoFull: true,
    // DO NOT sharpen the picture drawn at the display's own size after SMAA: CAS is for what a temporal filter or an upscale softened; here it lifted the
    // textures' grain into a 1-pixel checker and put back the steps SMAA smoothed, 42 % of a street view's pixel-level energy (the player, 2026-10-23: "pixelizado").
    sharpen: 0,
    anisotropy: 16,
    detailProps: true,
    vegetation: 2_600,
    grassBlades: 90_000,
    surfaceDetail: true,
    detailCutoffZoom: perM(0.5),
    pedestrianDetail: 2,
    occupantZoom: perM(3.25),
    vehicleErrorPixels: VEHICLE_ERROR_PIXELS,
    vehicleWholeCap: VEHICLE_WHOLE_CAP,
    hairStrandPeople: HAIR_STRAND_PEOPLE,
    cloudShadows: true,
    vegetationDistance: m(640),
    impostors: true,
    birds: 600,
    rain: true,
    skyClouds: true,
    mist: true,
    waterfalls: true,
    triangleScale: 1,
    detailMemoryMB: 1024,
  },
};
