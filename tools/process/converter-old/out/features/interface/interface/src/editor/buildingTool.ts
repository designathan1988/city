// What the feature uses of src/editor/buildingTool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/buildingTool.ts:94-107 interface ToolView
/** The building tool, as a state machine in world coordinates. */
export interface ToolView {
  /** Screen position (CSS px) of a world point at absolute height `z`. */
  project(x: number, y: number, z: number): Vec2;
  /** World point under a screen position, on the horizontal plane at `z`. */
  planeAt(screen: Vec2, z: number): Vec2;
  /** The pick ray through a screen position. */
  ray(screen: Vec2): Ray3;
  /** The height the terrain is drawn at. */
  groundAt(x: number, y: number): number;
  /** The paving an entrance opens onto (NaN off the roads); none in a headless test. */
  readonly pavedAt?: PavedAt;
  /** Handle hit radius, CSS px. */
  readonly pickPixels: number;
}

// src/editor/buildingTool.ts:109-120 interface ToolHost
export interface ToolHost {
  context(): BuildingContext;
  /** Records history, runs the edit, stores it; returns the edit's result. */
  commit(edit: () => EditResult): EditResult;
  /** Something the screen or the panel shows has changed. */
  changed(): void;
  /** Only what the pointer is over changed (`hover`, `hoverHandle`, `preview`): the screen, never the panel (`changed` where absent). */
  hovered?(): void;
  /** A transient message in the hint bar (a translation key). */
  flash(key: string): void;
  focus?(building: Building): void;
}

// src/editor/buildingTool.ts:130-140 interface BuildingPreview
export interface BuildingPreview {
  readonly building: Building;
  readonly valid: boolean;
  readonly problem: BuildingProblem | null;
  /** The stored building this preview stands in for, hidden meanwhile. */
  readonly hides: BuildingId | null;
  /** Bumped on every change, so the renderer can gate its rebuild. */
  readonly serial: number;
  /** Drawn in the building's own materials (the interior view), not as a ghost. */
  readonly solid?: boolean;
}

// src/editor/buildingTool.ts:188-2663 class BuildingTool
export class BuildingTool { … }

// src/editor/buildingTool.ts:916-932 method BuildingTool.interiorPreview
/** The selected building cut open at the interior level, drawn solid in place of the stored one. */
  interiorPreview(): BuildingPreview | null;
