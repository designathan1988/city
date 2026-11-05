// What the feature uses of src/ui/toolChoices.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/toolChoices.ts:9-17 function stored
/** What the player has chosen in the tools' panels and the game reads when the tool acts */

function stored<T extends string>(key: string, allowed: readonly T[], fallback: T): T;

// src/ui/toolChoices.ts:27-27 variable KIND_KEY
const KIND_KEY = 'roadcraft.streetscapeKind';

// src/ui/toolChoices.ts:28-28 variable kind
let kind: LandscapeKind = stored(KIND_KEY, LANDSCAPE_KINDS, 'tree');

// src/ui/toolChoices.ts:30-32 function streetscapeKind
export function streetscapeKind(): LandscapeKind;

// src/ui/toolChoices.ts:39-39 variable LAMP_KEY
const LAMP_KEY = 'roadcraft.poleLamps';

// src/ui/toolChoices.ts:40-40 variable lamps
let lamps: PoleLampMode = stored(LAMP_KEY, POLE_LAMP_MODES, 'alternate');

// src/ui/toolChoices.ts:42-44 function poleLampMode
export function poleLampMode(): PoleLampMode;

// src/ui/toolChoices.ts:52-52 variable POLE_TOOL_MODES
/** What a click of the pole tool does: build a run, or remove the pole under it. */
export const POLE_TOOL_MODES = ['build', 'remove'] as const;

// src/ui/toolChoices.ts:53-53 type PoleToolMode
export type PoleToolMode = (typeof POLE_TOOL_MODES)[number];

// src/ui/toolChoices.ts:54-54 variable poleMode
let poleMode: PoleToolMode = 'build';

// src/ui/toolChoices.ts:56-58 function poleToolMode
export function poleToolMode(): PoleToolMode;

// src/ui/toolChoices.ts:66-66 variable fogErasing
/** The fog brush (Paisagem > Terreno > Neblina): laying fog, or taking it away. Not kept between sessions. */
let fogErasing = false;

// src/ui/toolChoices.ts:67-69 function fogErase
export function fogErase(): boolean;

// src/ui/toolChoices.ts:75-75 variable TREE_MODES
/** The tree brush (Landscape > Terrain > Trees, `world/trees.ts`) */
export const TREE_MODES = ['plant', 'one', 'cut'] as const;

// src/ui/toolChoices.ts:76-76 type TreeMode
export type TreeMode = (typeof TREE_MODES)[number];

// src/ui/toolChoices.ts:77-77 variable treeModeNow
let treeModeNow: TreeMode = 'plant';

// src/ui/toolChoices.ts:78-78 variable treeKindNow
let treeKindNow: TreeKind = 'mixed';

// src/ui/toolChoices.ts:83-83 function treeMode
export function treeMode(): TreeMode;

// src/ui/toolChoices.ts:85-85 function treeKind
export function treeKind(): TreeKind;

// src/ui/toolChoices.ts:87-87 function treeBrush
export function treeBrush(): TreeBrush;

// src/ui/toolChoices.ts:88-91 function setTreeBrush
export function setTreeBrush(change: Partial<TreeBrush>): void;

// src/ui/toolChoices.ts:94-94 variable gullyErasing
/** The gully brush (Paisagem > Terreno > Ravinas, `world/gullies.ts`): cutting gullies, or wiping them away. Not kept between sessions. */
let gullyErasing = false;

// src/ui/toolChoices.ts:95-97 function gullyErase
export function gullyErase(): boolean;

// src/ui/toolChoices.ts:103-103 type ElementBrushMode
/** The element brush (Paisagem > Terreno > Elementos, `world/elements.ts`): which kind it lays, whether it lays or erases */
export type ElementBrushMode = 'lay' | 'eraseKind' | 'eraseAll';

// src/ui/toolChoices.ts:104-104 variable elementKindNow
let elementKindNow: ElementKind = 'stones';

// src/ui/toolChoices.ts:105-105 variable elementModeNow
let elementModeNow: ElementBrushMode = 'lay';

// src/ui/toolChoices.ts:110-110 function elementKind
export function elementKind(): ElementKind;

// src/ui/toolChoices.ts:111-111 function elementMode
export function elementMode(): ElementBrushMode;

// src/ui/toolChoices.ts:113-115 function elementBrush
export function elementBrush(kind: ElementKind = elementKindNow): ElementBrush;

// src/ui/toolChoices.ts:116-119 function setElementBrush
export function setElementBrush(change: Partial<ElementBrush>): void;

// src/ui/toolChoices.ts:125-125 variable CLOUD_MODES
/** The cloud tool (Paisagem > Terreno > Nuvens): what a click does. Not kept between sessions. */
export const CLOUD_MODES = ['add', 'move', 'edit', 'remove'] as const;

// src/ui/toolChoices.ts:126-126 type CloudMode
export type CloudMode = (typeof CLOUD_MODES)[number];

// src/ui/toolChoices.ts:127-127 variable cloudModeNow
let cloudModeNow: CloudMode = 'add';

// src/ui/toolChoices.ts:128-130 function cloudMode
export function cloudMode(): CloudMode;

// src/ui/toolChoices.ts:135-139 interface CloudBrush
/** The next cloud's size (m), base height (m) and density (%). Kept between sessions. */
export interface CloudBrush {
  readonly size: number;
  readonly height: number;
  readonly density: number;
}

// src/ui/toolChoices.ts:150-152 function cloudBrush
export function cloudBrush(): CloudBrush;

// src/ui/toolChoices.ts:153-156 function setCloudBrush
export function setCloudBrush(next: Partial<CloudBrush>): void;

// src/ui/toolChoices.ts:159-163 interface FogBrush
/** The fog brush's settings: strength (%), height (m), speed (m/s). Kept between sessions. */
export interface FogBrush {
  readonly strength: number;
  readonly height: number;
  readonly speed: number;
}

// src/ui/toolChoices.ts:174-176 function fogBrush
export function fogBrush(): FogBrush;

// src/ui/toolChoices.ts:177-180 function setFogBrush
export function setFogBrush(next: Partial<FogBrush>): void;

// src/ui/toolChoices.ts:182-182 variable PAINT_KEY
const PAINT_KEY = 'roadcraft.paintKind';

// src/ui/toolChoices.ts:183-183 variable paint
let paint: PaintKind = stored(PAINT_KEY, PAINT_KINDS, 'sand');

// src/ui/toolChoices.ts:186-188 function paintKind
/** The ground the terrain brush paints in its Paint mode. */
export function paintKind(): PaintKind;

// src/ui/toolChoices.ts:196-196 variable width
/** Total width of the roads the road tool lays, whole metres (the 1 m subgrid), or null for the class's own. */
let width: number | null = null;

// src/ui/toolChoices.ts:197-197 variable ROAD_WIDTH_RANGE
const ROAD_WIDTH_RANGE = [6, 60] as const;

// src/ui/toolChoices.ts:199-201 function roadWidth
export function roadWidth(): number | null;

// src/ui/toolChoices.ts:208-208 variable blockGridChoice
/** The block grid the road tool lays in one click (`editor/blocks.ts`) */
export const blockGridChoice = { cols: 3, rows: 2, blockMetres: 100, angle: 0, armed: false };

// src/ui/toolChoices.ts:211-211 variable signChoice
/** What the sign tool puts down next: its kind and its words; and the next street name. */
export const signChoice: { type: SignType; text: string; streetName: string } = { type: 'stop', text: '', streetName: '' };

// src/ui/toolChoices.ts:214-214 variable strikeChoice
/** The demolish tool: knock a building down at once, or strike it (and streets) with a chosen force. */
export const strikeChoice: { mode: 'demolish' | 'strike' | 'shoot'; strength: number } = { mode: 'demolish', strength: 5 };

// src/ui/toolChoices.ts:216-216 variable ZONE_COLOUR_KEY
const ZONE_COLOUR_KEY = 'roadcraft.zoneColours';

// src/ui/toolChoices.ts:217-217 variable zoneColours
let zoneColours = stored(ZONE_COLOUR_KEY, ['on', 'off'] as const, 'on') === 'on';

// src/ui/toolChoices.ts:219-219 function zoneColoursShown
/** Whether zoned land is tinted with its use's colour outside the Zoning tool. */
export function zoneColoursShown(): boolean;

// src/ui/toolChoices.ts:222-222 variable ROAD_GRID_KEY
const ROAD_GRID_KEY = 'roadcraft.roadGrid';

// src/ui/toolChoices.ts:223-223 variable roadGrid
let roadGrid = stored(ROAD_GRID_KEY, ['on', 'off'] as const, 'off') === 'on';

// src/ui/toolChoices.ts:224-224 variable gridListeners
const gridListeners = new Set<() => void>();

// src/ui/toolChoices.ts:226-226 function roadGridShown
/** Whether the universal grid (`world/grid.ts`) is drawn over the whole map, with any tool */
export function roadGridShown(): boolean;
