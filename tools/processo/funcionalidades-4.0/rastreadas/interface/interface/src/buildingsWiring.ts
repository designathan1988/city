// What the feature uses of src/buildingsWiring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/buildingsWiring.ts:31-47 interface BuildingWiringDeps
/** The building tool's share of the composition root. */
export interface BuildingWiringDeps {
  readonly doc: RoadDoc;
  readonly net: Network;
  readonly history: History;
  readonly scene: SceneHandle;
  view(): Viewport;
  size(): { readonly w: number; readonly h: number };
  /** After a stored edit: autosave, history buttons, redraw. */
  afterEdit(): void;
  requestDraw(): void;
  focusBuilding?(building: Building): void;
  flash(key: string, params?: Readonly<Record<string, string | number>>): void;
  /** The tool's mode changed, so the hint bar's sentence did. */
  hintChanged(): void;
  undo(): void;
  redo(): void;
}

// src/buildingsWiring.ts:49-79 interface BuildingWiring
export interface BuildingWiring {
  readonly tool: BuildingTool;
  /** The shared chrome: main.ts mounts the road toolbar and panels into it. */
  readonly workspace: ReturnType<typeof initBuilderWorkspace>;
  pointerDown(screen: Vec2, world: Vec2, shift: boolean): void;
  /** A click with nothing in hand: opens (or switches) the building seen inside; true if taken. */
  insideClick(screen: Vec2, double: boolean): boolean;
  pointerMove(screen: Vec2, world: Vec2, shift: boolean): void;
  pointerUp(cancelled: boolean): void;
  /** Photographs the parts a gallery is about to show, a few per frame. */
  requestThumbnails(ids: readonly string[]): void;
  /** Returns true when the tool used the key. */
  key(e: KeyboardEvent): boolean;
  /** Right button: cancels the operation in progress, if any. */
  cancelOperation(): boolean;
  activate(): void;
  deactivate(): void;
  /** Before each draw: hands the preview to the renderer. */
  beforeDraw(active: boolean): void;
  /** A building opened from inside at its ground floor (the player went in), or closed. */
  openInside(id: BuildingId | null): void;
  drawOverlay(ctx: CanvasRenderingContext2D): void;
  /** After an undo, a redo or a load. */
  restored(): void;
  /** After a road edit, inside the same undo step: the road-wins rule over what changed since `before`, the network's state read just before the edit. */
  afterRoadEdit(before: object): void;
  /** The bulldozer: demolishes the building under a screen point, if any. */
  bulldozeAt(screen: Vec2): boolean;
  hintKey(prefix: string): string;
  languageChanged(): void;
}

// src/buildingsWiring.ts:81-1216 function createBuildingWiring
export function createBuildingWiring(deps: BuildingWiringDeps): BuildingWiring;

// src/buildingsWiring.ts:483-494 function createBuildingWiring.requestThumbnails
/** Photographs the parts a gallery is about to show, a few per frame, and hands each batch to the workspace as it is ready. */
  function requestThumbnails(ids: readonly string[]): void;
