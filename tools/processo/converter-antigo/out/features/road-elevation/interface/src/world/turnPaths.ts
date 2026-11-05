// What the feature uses of src/world/turnPaths.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/turnPaths.ts:30-30 type Poly
type Poly = readonly Vec2[];

// src/world/turnPaths.ts:31-31 interface Box
interface Box { minX: number; minY: number; maxX: number; maxY: number }

// src/world/turnPaths.ts:32-40 interface Area
interface Area {
  readonly poly: Poly;
  readonly box: Box;
  readonly hashA: number;
  readonly hashB: number;
  /** Its edges by band (`core/polygonBands.ts`), made the first time a point is tested against it; its flat x, y list, the first time a worker needs it (`flatRings`). */
  bands: PolygonBands | null;
  flat: Float64Array | null;
}

// src/world/turnPaths.ts:44-58 function areaOf
function areaOf(points: Poly): Area;
