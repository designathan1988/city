// What the feature uses of src/ui/toolChoices.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/toolChoices.ts:30-32 function streetscapeKind
export function streetscapeKind(): LandscapeKind;

// src/ui/toolChoices.ts:42-44 function poleLampMode
export function poleLampMode(): PoleLampMode;

// src/ui/toolChoices.ts:52-52 variable POLE_TOOL_MODES
/** What a click of the pole tool does: build a run, or remove the pole under it. */
export const POLE_TOOL_MODES = ['build', 'remove'] as const;

// src/ui/toolChoices.ts:53-53 type PoleToolMode
export type PoleToolMode = (typeof POLE_TOOL_MODES)[number];

// src/ui/toolChoices.ts:56-58 function poleToolMode
export function poleToolMode(): PoleToolMode;

// src/ui/toolChoices.ts:87-87 function treeBrush
export function treeBrush(): TreeBrush;

// src/ui/toolChoices.ts:88-91 function setTreeBrush
export function setTreeBrush(change: Partial<TreeBrush>): void;

// src/ui/toolChoices.ts:113-115 function elementBrush
export function elementBrush(kind: ElementKind = elementKindNow): ElementBrush;

// src/ui/toolChoices.ts:116-119 function setElementBrush
export function setElementBrush(change: Partial<ElementBrush>): void;

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

// src/ui/toolChoices.ts:199-201 function roadWidth
export function roadWidth(): number | null;

// src/ui/toolChoices.ts:203-205 function setRoadWidth
export function setRoadWidth(next: number | null): void;

// src/ui/toolChoices.ts:208-208 variable blockGridChoice
/** The block grid the road tool lays in one click (`editor/blocks.ts`) */
export const blockGridChoice = { cols: 3, rows: 2, blockMetres: 100, angle: 0, armed: false };

// src/ui/toolChoices.ts:211-211 variable signChoice
/** What the sign tool puts down next: its kind and its words; and the next street name. */
export const signChoice: { type: SignType; text: string; streetName: string } = { type: 'stop', text: '', streetName: '' };

// src/ui/toolChoices.ts:214-214 variable strikeChoice
/** The demolish tool: knock a building down at once, or strike it (and streets) with a chosen force. */
export const strikeChoice: { mode: 'demolish' | 'strike' | 'shoot'; strength: number } = { mode: 'demolish', strength: 5 };

// src/ui/toolChoices.ts:226-226 function roadGridShown
/** Whether the universal grid (`world/grid.ts`) is drawn over the whole map, with any tool */
export function roadGridShown(): boolean;
