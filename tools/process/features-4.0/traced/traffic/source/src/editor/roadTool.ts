// The code of src/editor/roadTool.ts that is the feature's (the rest of the file is shared).

// src/editor/roadTool.ts:65-100 slice of src/editor/roadTool.ts#RoadToolHost (shared): the feature's statements; "…" marks code left out
export interface RoadToolHost {
  // … 6 lines
  settings(): {
    readonly typeIndex: number; readonly lanes: number | null; readonly alignment: 'straight' | 'curve' | 'free';
    readonly heightOffset: number; readonly parking: SegmentParking | undefined; readonly width: number | null; readonly grid: boolean;
    /** A profile new roads are laid with, over the class, lanes, width and parking; null for those. */
    readonly profile?: { readonly profile: RoadProfileSpec; readonly type: number } | null;
    /** Retaining walls in the cuttings of new roads instead of a batter. */
    readonly cutWalls?: boolean;
    /** The furniture the new roads are built with (V7, `world/roads/furnitureSets.ts`); absent: none. */
    readonly furniture?: FurnitureSet;
  };
// … 18 lines
}
