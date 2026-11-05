// What the feature uses of src/ui/toolChoices.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/toolChoices.ts:9-17 function stored
/** What the player has chosen in the tools' panels and the game reads when the tool acts */

function stored<T extends string>(key: string, allowed: readonly T[], fallback: T): T;

// src/ui/toolChoices.ts:19-25 function keep
function keep(key: string, value: string): void;

// src/ui/toolChoices.ts:30-32 function streetscapeKind
export function streetscapeKind(): LandscapeKind;

// src/ui/toolChoices.ts:39-39 variable LAMP_KEY
const LAMP_KEY = 'roadcraft.poleLamps';

// src/ui/toolChoices.ts:40-40 variable lamps
let lamps: PoleLampMode = stored(LAMP_KEY, POLE_LAMP_MODES, 'alternate');

// src/ui/toolChoices.ts:42-44 function poleLampMode
export function poleLampMode(): PoleLampMode;

// src/ui/toolChoices.ts:46-49 function setPoleLampMode
export function setPoleLampMode(next: PoleLampMode): void;

// src/ui/toolChoices.ts:52-52 variable POLE_TOOL_MODES
/** What a click of the pole tool does: build a run, or remove the pole under it. */
export const POLE_TOOL_MODES = ['build', 'remove'] as const;

// src/ui/toolChoices.ts:53-53 type PoleToolMode
export type PoleToolMode = (typeof POLE_TOOL_MODES)[number];

// src/ui/toolChoices.ts:56-58 function poleToolMode
export function poleToolMode(): PoleToolMode;

// src/ui/toolChoices.ts:199-201 function roadWidth
export function roadWidth(): number | null;

// src/ui/toolChoices.ts:211-211 variable signChoice
/** What the sign tool puts down next: its kind and its words; and the next street name. */
export const signChoice: { type: SignType; text: string; streetName: string } = { type: 'stop', text: '', streetName: '' };

// src/ui/toolChoices.ts:226-226 function roadGridShown
/** Whether the universal grid (`world/grid.ts`) is drawn over the whole map, with any tool */
export function roadGridShown(): boolean;
