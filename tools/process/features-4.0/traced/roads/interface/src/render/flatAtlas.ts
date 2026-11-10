// What the feature uses of src/render/flatAtlas.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/flatAtlas.ts:71-85 class PlateSource
class PlateSource implements TerrainSource { … }

// src/render/flatAtlas.ts:87-109 interface Plate
interface Plate {
  /** Its key (`world/bounds.ts` `plateOf`) and centre. */
  readonly key: number;
  readonly cx: number;
  readonly cy: number;
  readonly source: PlateSource;
  /** The document's stamps and dabs that reach it, moved to its map (about its centre). */
  /** The copies, moved to the plate's map, of the stamps and dabs that reach it (`copyOn`): none for the others. */
  readonly stampCopies: WeakMap<TerrainStamp, TerrainStamp>;
  readonly dabCopies: WeakMap<PaintDab, PaintDab>;
  surface: PlateSurface | null;
  shown: boolean;
  /** Its surface's programs built (`uploads.ts` programsReady): not shown before, the far patch standing in. */
  ready: boolean;
  /** The land at the plate's grid corners, the base level added, read off the page (`flatGroundPool.ts`), rows from the north edge; null until then. */
  heights: Float64Array | null;
  /** Its first build made off the page, until the plate is made from it (`plateMesh.ts`). */
  mesh: PlateMesh | null;
  /** Whether its corners were asked of the workers (and not yet come back). */
  asked: boolean;
  /** The highest corner of its drawn ground, kept as it changes. */
  top: number;
}
