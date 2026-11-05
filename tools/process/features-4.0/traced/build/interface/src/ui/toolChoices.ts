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

// src/ui/toolChoices.ts:219-219 function zoneColoursShown
/** Whether zoned land is tinted with its use's colour outside the Zoning tool. */
export function zoneColoursShown(): boolean;

// src/ui/toolChoices.ts:226-226 function roadGridShown
/** Whether the universal grid (`world/grid.ts`) is drawn over the whole map, with any tool */
export function roadGridShown(): boolean;
