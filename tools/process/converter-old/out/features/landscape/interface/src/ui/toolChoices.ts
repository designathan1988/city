// What the feature uses of src/ui/toolChoices.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/toolChoices.ts:9-17 function stored
/** What the player has chosen in the tools' panels and the game reads when the tool acts */

function stored<T extends string>(key: string, allowed: readonly T[], fallback: T): T;

// src/ui/toolChoices.ts:19-25 function keep
function keep(key: string, value: string): void;

// src/ui/toolChoices.ts:27-27 variable KIND_KEY
const KIND_KEY = 'roadcraft.streetscapeKind';

// src/ui/toolChoices.ts:28-28 variable kind
let kind: LandscapeKind = stored(KIND_KEY, LANDSCAPE_KINDS, 'tree');

// src/ui/toolChoices.ts:30-32 function streetscapeKind
export function streetscapeKind(): LandscapeKind;

// src/ui/toolChoices.ts:34-37 function setStreetscapeKind
export function setStreetscapeKind(next: LandscapeKind): void;

// src/ui/toolChoices.ts:42-44 function poleLampMode
export function poleLampMode(): PoleLampMode;

// src/ui/toolChoices.ts:52-52 variable POLE_TOOL_MODES
/** What a click of the pole tool does: build a run, or remove the pole under it. */
export const POLE_TOOL_MODES = ['build', 'remove'] as const;

// src/ui/toolChoices.ts:53-53 type PoleToolMode
export type PoleToolMode = (typeof POLE_TOOL_MODES)[number];

// src/ui/toolChoices.ts:56-58 function poleToolMode
export function poleToolMode(): PoleToolMode;

// src/ui/toolChoices.ts:78-78 variable treeKindNow
let treeKindNow: TreeKind = 'mixed';

// src/ui/toolChoices.ts:79-79 variable TREE_BRUSH_KEY
const TREE_BRUSH_KEY = 'roadcraft.treeBrush';

// src/ui/toolChoices.ts:80-82 variable treeBrushNow
let treeBrushNow: TreeBrush = (() => {
  try { return { ...DEFAULT_TREE_BRUSH, ...((JSON.parse(localStorage.getItem(TREE_BRUSH_KEY) ?? '{}') ?? {}) as Partial<TreeBrush>) }; } catch { return DEFAULT_TREE_BRUSH; }
})();

// src/ui/toolChoices.ts:85-85 function treeKind
export function treeKind(): TreeKind;

// src/ui/toolChoices.ts:86-86 function setTreeKind
export function setTreeKind(next: TreeKind): void;

// src/ui/toolChoices.ts:87-87 function treeBrush
export function treeBrush(): TreeBrush;

// src/ui/toolChoices.ts:88-91 function setTreeBrush
export function setTreeBrush(change: Partial<TreeBrush>): void;

// src/ui/toolChoices.ts:113-115 function elementBrush
export function elementBrush(kind: ElementKind = elementKindNow): ElementBrush;

// src/ui/toolChoices.ts:116-119 function setElementBrush
export function setElementBrush(change: Partial<ElementBrush>): void;

// src/ui/toolChoices.ts:125-125 variable CLOUD_MODES
/** The cloud tool (Paisagem > Terreno > Nuvens): what a click does. Not kept between sessions. */
export const CLOUD_MODES = ['add', 'move', 'edit', 'remove'] as const;

// src/ui/toolChoices.ts:126-126 type CloudMode
export type CloudMode = (typeof CLOUD_MODES)[number];

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

// src/ui/toolChoices.ts:190-193 function setPaintKind
export function setPaintKind(next: PaintKind): void;

// src/ui/toolChoices.ts:199-201 function roadWidth
export function roadWidth(): number | null;

// src/ui/toolChoices.ts:211-211 variable signChoice
/** What the sign tool puts down next: its kind and its words; and the next street name. */
export const signChoice: { type: SignType; text: string; streetName: string } = { type: 'stop', text: '', streetName: '' };

// src/ui/toolChoices.ts:226-226 function roadGridShown
/** Whether the universal grid (`world/grid.ts`) is drawn over the whole map, with any tool */
export function roadGridShown(): boolean;
