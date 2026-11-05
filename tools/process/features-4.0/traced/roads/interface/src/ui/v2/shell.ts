// What the feature uses of src/ui/v2/shell.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/v2/shell.ts:247-248 function svg
const svg = (name: string, size = 22): string => …;

// src/ui/v2/shell.ts:256-264 function button
const button = (className: string, label: string, run: () => void, icon?: string): HTMLButtonElement =>;

// src/ui/v2/shell.ts:267-267 type TransitToolKind
/** What the public transport tool is set to and does, as the panel shows it (`editor/transitTools.ts`). */
type TransitToolKind = 'stop' | 'terminal' | 'bay' | 'track' | 'station' | 'line';

// src/ui/v2/shell.ts:268-276 interface ShellTransitTool
interface ShellTransitTool {
  /** The kind in hand: one of the tabs below, or another the editor has (a mode without a tab of its own). */
  readonly kind: string;
  readonly rail: 'train' | 'metro';
  setKind(kind: TransitToolKind, rail?: 'train' | 'metro'): void;
  lines(): readonly TransitLine[];
  setVehicles(line: number, n: number): void;
  removeLine(line: number): void;
}

// src/ui/v2/shell.ts:280-307 interface ShellGameValues
/** What the shell needs from the rest of the game. */
/** The game's state as the shell reads it (`main.ts` `gameState`) */
interface ShellGameValues {
  readonly tool: string;
  readonly paused: boolean;
  readonly speed: number;
  readonly quality: string;
  readonly roadTypeIndex: number;
  readonly alignment: string;
  readonly roadHeightOffset: number;
  readonly roadLanePreset: number | null;
  readonly roundaboutRadius: number;
  readonly terrainMode: string;
  readonly terrainRadius: number;
  readonly terrainStrength: number;
  readonly terrainHardness: number;
  readonly barrierKind: string;
  readonly zoneUse: string;
  readonly zoneDensity: string;
  readonly zoneEraser: boolean;
  readonly zoneMode: string;
  readonly lotSplitKind: string;
  readonly lotSplitParts: number;
  readonly congestionOverlay: boolean;
  readonly perspective: boolean;
  readonly canUndo: boolean;
  readonly canRedo: boolean;
  readonly sky: string;
  readonly inspectorOpen: boolean;
}

// src/ui/v2/shell.ts:308-311 interface ShellGame
interface ShellGame {
  readonly values: Readonly<ShellGameValues>;
  watch(keys: readonly (keyof ShellGameValues)[], fn: () => void): () => void;
}

// src/ui/v2/shell.ts:313-321 interface ShellSlider
/** A number the player sets with a slider: its range, and how the game reads and takes it. */
export interface ShellSlider {
  readonly min: number;
  readonly max: number;
  readonly step: number;
  get(): number;
  set(value: number): void;
  /** The drag ended (one undo step a drag, for the map's own numbers). */
  end?(): void;
}

// src/ui/v2/shell.ts:323-335 interface ShellStatus
/** The city's numbers, as `main.ts` computes them for the top bar and the simulation menu. */
export interface ShellStatus {
  readonly clock: string;
  readonly vehicles: string;
  readonly people: string;
  readonly trips: string;
  readonly lost: string;
  readonly meanSpeed: string;
  readonly queued: string;
  /** Junctions that cannot be built (`Network.impossible`). */
  readonly impossible: number;
  /** The money in hand (`world/economy.ts`). */
  readonly balance: number;
}

// src/ui/v2/shell.ts:337-343 interface ShellReadouts
/** The map's own numbers the old status bar showed (`main.ts`): its roads and nodes, the zoom */
interface ShellReadouts {
  readonly roads: number;
  readonly nodes: number;
  readonly zoom: number;
  readonly x: number;
  readonly y: number;
}

// src/ui/v2/shell.ts:345-352 interface ShellZonePatch
/** What a lot or zone setting is changed to (`main.ts` `setZone`). */
export interface ShellZonePatch {
  readonly mode?: string;
  readonly use?: string;
  readonly density?: string;
  readonly eraser?: boolean;
  readonly splitKind?: string;
  readonly splitParts?: number;
}

// src/ui/v2/shell.ts:354-361 interface ShellRoadClass
/** A road class, as the upgrade tool shows it. */
export interface ShellRoadClass {
  readonly index: number;
  readonly name: string;
  readonly title: string;
  readonly picture: string;
  /** Its total width in whole metres, which the width stepper starts from. */
  readonly widthM: number;
}

// src/ui/v2/shell.ts:363-434 interface ShellDeps
export interface ShellDeps {
  readonly workspace: BuilderWorkspace;
  readonly game: ShellGame;
  /** The road inspector's panel (`ui/inspector.ts`), shown in the selection column. */
  readonly inspectorPanel: HTMLElement | null;
  /** Picks a tool, or puts it down when it is already in hand (`main.ts` `pickTool`). */
  pickTool(tool: string): void;
  /** Takes a tool in hand (`main.ts` `setTool`). */
  setTool(tool: string): void;
  /** Puts everything down: no tool, nothing picked. */
  putDown(): void;
  setSpeed(speed: number): void;
  undo(): void;
  redo(): void;
  newMap(): void;
  /** The world without an edge: generated tile by tile round the camera as it goes (CLAUDE.md "Streaming world"). */
  newWorld(): void;
  /** A large planned city (`planCity`), opened at a district with the rest filled after the curtain. */
  newLargeWorld(): void;
  openMap(): void;
  saveMap(): void;
  about(): void;
  readonly qualityLevels: readonly string[];
  setQuality(level: string): void;
  /** Opens the graphics options (`ui/graphicsPanel.ts`). */
  graphicsOptions(): void;
  /** Opens the effects panel (`ui/effectsPanel.ts`): each effect switched to measure its cost. */
  effectsPanel(): void;
  markingStyle(): string;
  setMarkingStyle(style: string): void;
  setSky(mode: string): void;
  toggleCongestion(): void;
  /** A camera command: turnLeft, turnRight, tiltUp, tiltDown, north, perspective, reset. */
  camera(command: string): void;
  /** Whether the camera turns at all (the flat view does not). */
  readonly cameraTurns: boolean;
  // The road tool.
  readonly roundabout: boolean;
  roadClasses(): readonly ShellRoadClass[];
  selectRoadType(index: number): void;
  setAlignment(alignment: string): void;
  stepRoadHeight(direction: number): void;
  laneChoices(): readonly { readonly id: string; readonly label: string; readonly on: boolean }[];
  setLanes(id: string): void;
  // Zoning, the land, the walls.
  setZone(patch: ShellZonePatch): void;
  setTerrainMode(mode: string): void;
  setBarrierKind(kind: string): void;
  mapBiome(): string;
  setMapBiome(key: string): void;
  /** An action on the whole map, asking first: clearTerrain, clearGullies, clearFog, clearTrees, clearElements, scatterClouds, clearClouds. */
  mapAction(name: string): void;
  // The simulation.
  demand(): number;
  setDemand(value: number): void;
  /** Every slider the panels show, by name (`main.ts` `SLIDERS`). */
  readonly sliders: Readonly<Record<string, ShellSlider>>;
  /** Told the city's numbers whenever the game computes them (a few times a second while it runs). */
  onStatus(listen: (status: ShellStatus) => void): void;
  /** Told the tool's hint, and each answer of the game (saved, refused, undone...) as a flash. */
  onHint(listen: (text: string, flash: boolean) => void): void;
  roadSnap(): { readonly on: boolean; readonly angles: boolean; readonly grid: boolean };
  setRoadSnap(patch: Partial<{ on: boolean; angles: boolean; grid: boolean }>): void;
  readonly parkingPresets: readonly string[];
  roadParkingPreset(): string;
  setRoadParkingPreset(preset: string): void;
  transitTool(): ShellTransitTool | null;
  /** Takes the camera and the inspector to the first junction that cannot be built. */
  showImpossible(): void;
  /** The map's numbers (`ShellReadouts`), read while the Simulation or the Camera menu is open */
  readouts?(): ShellReadouts;
}

// src/ui/v2/shell.ts:437-444 interface ShellHandle
/** What the game asks of the shell. */
export interface ShellHandle {
  /** Opens a tool's panel with that tool in hand (the road inspector's eyedropper). */
  openTool(tool: string): void;
  /** A tool's key was pressed (`editor/gameKeys.ts`), after the tool machine took it or put it down */
  toolKeyed(tool: string): void;
  /** Puts a control of the game's own in the top bar, before Help (F9's button, `ui/healthPanel.ts`). */
  addToBar(control: HTMLElement): void;
}

// src/ui/v2/shell.ts:446-2084 function mountShell
export function mountShell(deps: ShellDeps): ShellHandle;

// src/ui/v2/shell.ts:1223-1338 function mountShell.renderRoads
function renderRoads(current: string): void;
