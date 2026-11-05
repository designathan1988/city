// What the feature uses of src/ui/builder/workspace.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/builder/workspace.ts:15-15 type ChromeMode
/** The Builder's side of the interface: its state */

type ChromeMode = 'road' | 'builder';

// src/ui/builder/workspace.ts:17-50 interface BuilderState
export interface BuilderState {
  readonly category: BuilderCategoryId;
  /** The active tool id (may be an action that ran once). */
  readonly tool: string;
  /** The free part the tool has armed, if any. */
  readonly armed: string | null;
  /** Tool ids that are implemented and clickable right now. */
  readonly ready: ReadonlySet<string>;
  readonly floor: { readonly active: number; readonly total: number };
  readonly snap: string;
  readonly grid: boolean;
  readonly hideOthers: boolean;
  readonly canUndo: boolean;
  readonly canRedo: boolean;
  readonly inspectorOpen: boolean;
  /** True while a gesture is running. */
  readonly busy: boolean;
  readonly selection: BuilderSelectionInfo | null;
  /** One sentence on what the pointer does now. */
  readonly hint: string;
  /** A plan is being drawn: the panel carries Finish, Back and Cancel. */
  readonly planning: boolean;
  readonly planPoints: number;
  readonly userBlueprints: readonly Blueprint[];
  /** The facade pattern of the selected mass, and where a pattern applies. */
  readonly pattern: string | null;
  readonly scope: string;
  /** The selected mass's roof, for the slope controls. */
  readonly roof: { readonly pitch: number; readonly ridge: 'x' | 'y'; readonly fall: number; readonly pitched: boolean } | null;
  /** The material the finish tools would paint now. */
  readonly material: { readonly finish: Finish; readonly colour: number } | null;
  /** What a drawn shape does now. */
  readonly drawAction: string;
}

// src/ui/builder/workspace.ts:52-86 interface BuilderActions
export interface BuilderActions {
  /** Asks for pictures of the parts a tab is about to show (they arrive a few per frame). */
  requestThumbnails(ids: readonly string[]): void;
  undo(): void;
  redo(): void;
  setCategory(id: BuilderCategoryId): void;
  chooseTool(id: string): void;
  setFloor(n: number): void;
  floorCommand(command: 'insertAbove' | 'insertBelow'): void;
  setSnap(mode: string): void;
  toggleGrid(): void;
  toggleHideOthers(): void;
  toggleInspector(): void;
  selectionAction(name: 'duplicate' | 'mirror' | 'group' | 'save' | 'delete'): void;
  setField(id: string, value: number): void;
  choosePreset(key: string): void;
  chooseUserBlueprint(key: string): void;
  removeUserBlueprint(key: string): void;
  chooseFinish(finish: Finish): void;
  chooseColour(colour: number): void;
  chooseStyle(key: string): void;
  choosePattern(pattern: string): void;
  setScope(scope: string): void;
  setDrawAction(action: string): void;
  planFinish(): void;
  planBack(): void;
  planCancel(): void;
  roofPitch(delta: number): void;
  roofRidge(ridge: 'x' | 'y'): void;
  roofFall(side: number): void;
  /** A view command: frame | top | turnLeft | turnRight. */
  view(id: string): void;
  /** "See inside" for the whole city: toggled, or its floor moved; returns where it stands. */
  seeInside(command: 'toggle' | 'up' | 'down'): { readonly on: boolean; readonly level: number };
}

// src/ui/builder/workspace.ts:88-106 interface BuilderWorkspace
export interface BuilderWorkspace {
  refresh(state: BuilderState): void;
  /** Shows the see-inside control as it now is (opened by a click on the map). */
  showInside(state: { on: boolean; level: number }): void;
  /** See inside as it is now, and each change of it (a building opened by a click on the map too). */
  inside(): { readonly on: boolean; readonly level: number };
  onInside(listen: (state: { readonly on: boolean; readonly level: number }) => void): void;
  /** Which half of the game the container is driving. */
  setMode(mode: ChromeMode): void;
  /** Opens a tab. */
  showGallery(category: BuilderCategoryId, gallery?: string): void;
  relabel(): void;
  setPresetThumbnails(images: ReadonlyMap<string, string>): void;
  readonly root: HTMLElement;
  /** Interface v2 drives the Builder with the same commands, from its own panels. */
  readonly actions: BuilderActions;
  subscribe(listen: (state: BuilderState) => void): void;
  thumbnail(id: string): string | undefined;
}

// src/ui/builder/workspace.ts:108-108 variable SNAP_MODES
export const SNAP_MODES = ['auto', 'grid', 'edge', 'face', 'centre', 'building', 'road', 'off'] as const;

// src/ui/builder/workspace.ts:117-371 function initBuilderWorkspace
export function initBuilderWorkspace(actions: BuilderActions): BuilderWorkspace;

// src/ui/builder/workspace.ts:322-333 function initBuilderWorkspace.refresh
const refresh = (state: BuilderState): void =>;
