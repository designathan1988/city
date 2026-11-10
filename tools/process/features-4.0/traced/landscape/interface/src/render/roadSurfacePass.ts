// What the feature uses of src/render/roadSurfacePass.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadSurfacePass.ts:23-23 variable GROUND_MAX_EDGE
/** ONE PASS OF THE ROADS' SURFACES AS PLAIN DATA (`roadSurfaces.ts` on the page, `nearRoadTiles.ts` in a worker): what it reads, how it is described and each of its tiles' surfaces, the same code on both sides */

/** Longest triangle edge on a road at grade. */
export const GROUND_MAX_EDGE = TERRAIN_CELL / 2;

// src/render/roadSurfacePass.ts:25-25 variable RAISED_MAX_EDGE
/** A raised deck follows nothing, so it needs vertices only for its shading. */
export const RAISED_MAX_EDGE = TERRAIN_CELL;

// src/render/roadSurfacePass.ts:27-27 function followsGround
/** Whether a road at grade has ground to follow: on a new map's level ground (`RELIEF_FLAT`, no stamp) it follows nothing either, and is laid as a deck is (`RAISED_MAX_EDGE`). DO NOT refine it to half a terrain cell there: a large city's streets drew 6 M triangles of a flat plane, a quarter of the frame's (2026-10-20). */
const followsGround = (net: Network): boolean => …;

// src/render/roadSurfacePass.ts:30-36 interface PassLook
/** The page's constants a pass is drawn with: its materials' texture scales, whether the ground verge wears the terrain's material, the carriageway's tints (made on the page with three's colour management); numbers, handed to a worker as they are. */
export interface PassLook {
  readonly groundMaterial: boolean;
  readonly scale: PassDesc['scale'];
  readonly classTint: readonly number[];
  readonly referenceTint: readonly [number, number, number];
  readonly pavingTint: Readonly<Record<string, readonly [number, number, number]>>;
}

// src/render/roadSurfacePass.ts:56-67 function passRaised
/** Whether a pass is drawn as a raised deck: an alignment lifted by hand where some point of its roads stands a structure's lift over the ground; a structural level by its kind. */
export function passRaised(net: Network, pass: SurfacePass, elevation: Pick<RoadReads, 'onSegment'>, terrainAt: (x: number, y: number) => number): boolean;

// src/render/roadSurfacePass.ts:70-92 function passReads
/** What a pass's surfaces read where the network is at hand (the page's, or a worker's own): the solved field, the ground as drawn, each road's look and line, the footways' rise and edges (made once a pass). */
export function passReads(net: Network, elevation: RoadReads, terrainAt: (x: number, y: number) => number, inputs: SurfaceInputs, passId: string): SurfaceReads;

// src/render/roadSurfacePass.ts:95-113 function passDesc
/** The pass as its surfaces read it, plain data: the same on the page and in a worker (`roadSurfaceReads.ts`). */
export function passDesc(net: Network, pass: SurfacePass, raised: boolean, look: PassLook, paint: readonly string[]): PassDesc;

// src/render/roadSurfacePass.ts:128-131 interface MeshedSurface
/** A surface of a pass as a tile is made of it: what it is cut from and how it is meshed. */
interface MeshedSurface {
  readonly source: TileSource;
  readonly options: TileMeshOptions & { readonly name: string; readonly maxEdge: number };
}

// src/render/roadSurfacePass.ts:134-142 function tileBundle
/** Every surface of one tile, from the rings that reach it, each vertex read here (`meshTileShape`): by surface name, in the order of `specs`. */
export function tileBundle(into: Reach, rect: TileRect, specs: readonly MeshedSurface[]): Map<string, Tile>;
