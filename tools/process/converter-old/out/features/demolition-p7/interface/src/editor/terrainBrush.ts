// What the feature uses of src/editor/terrainBrush.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/terrainBrush.ts:15-15 variable TERRAIN_MIN_MS
/** The shortest time between dabs, ms. */
export const TERRAIN_MIN_MS = 45;

// src/editor/terrainBrush.ts:21-25 interface Landform
/** How often a held flatten levels a little further, ms. */

/** A landform: a shape and its rock in one tool (`main.ts` LANDFORMS). */
interface Landform {
  readonly mode: 'raise' | 'lower';
  readonly rock: GeologyKind;
  readonly profile?: 'dome';
}

// src/editor/terrainBrush.ts:28-43 interface DabSettings
/** Everything a dab reads of the brush in hand, read once per dab. */
export interface DabSettings {
  /** The brush (`main.ts` BrushMode): a land mode, a landform, or paint/fog/gully/trees/elements. */
  readonly mode: string;
  readonly radius: number;
  readonly strength: number;
  /** 0..95: how hard the raise/lower edge is (0 the smooth dome). */
  readonly hardness: number;
  readonly landform: Landform | undefined;
  readonly element: { readonly mode: string; readonly kind: ElementKind; readonly brush: ElementBrush };
  readonly tree: { readonly mode: string; readonly kind: TreeKind; readonly brush: TreeBrush };
  /** 0..100. */
  readonly gully: { readonly strength: number; readonly erase: boolean };
  /** Strength 0..100; height and speed in metres. */
  readonly fog: { readonly strength: number; readonly height: number; readonly speed: number; readonly erase: boolean };
  readonly paint: PaintKind;
}

// src/editor/terrainBrush.ts:170-265 class TerrainBrush
export class TerrainBrush { … }
