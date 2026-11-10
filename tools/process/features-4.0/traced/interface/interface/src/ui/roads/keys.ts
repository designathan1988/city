// What the feature uses of src/ui/roads/keys.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/roads/keys.ts:5-5 type RoadKeyAction
/** THE ROAD TOOL'S KEYS, rebindable (read by `main.ts`) */
export type RoadKeyAction = 'heightUp' | 'heightDown' | 'alignment';

// src/ui/roads/keys.ts:6-6 variable ROAD_KEY_ACTIONS
export const ROAD_KEY_ACTIONS: readonly RoadKeyAction[] = ['heightUp', 'heightDown', 'alignment'];

// src/ui/roads/keys.ts:8-8 variable DEFAULTS
const DEFAULTS: Readonly<Record<RoadKeyAction, string>> = { heightUp: 'PageUp', heightDown: 'PageDown', alignment: 'v' };

// src/ui/roads/keys.ts:9-9 variable STORAGE_KEY
const STORAGE_KEY = 'roadcraft.roadKeys';

// src/ui/roads/keys.ts:20-21 function codesOfKey
const codesOfKey = (key: string): readonly string[] => …;

// src/ui/roads/keys.ts:23-23 function lentByTheCamera
/** Page Up and Page Down tilt the camera, and are the road's own raise and lower by default */
const lentByTheCamera = (key: string): boolean => …;

// src/ui/roads/keys.ts:25-27 function cameraKey
/** DO NOT list the camera's keys here by hand. */
function cameraKey(key: string): boolean;

// src/ui/roads/keys.ts:28-28 function reserved
const reserved = (key: string): boolean => …;

// src/ui/roads/keys.ts:31-33 function normaliseKey
/** A key as stored: letters lower case, everything else as `KeyboardEvent.key` names it. */
function normaliseKey(key: string): string;

// src/ui/roads/keys.ts:35-35 variable bindings
let bindings: Record<RoadKeyAction, string> = load();

// src/ui/roads/keys.ts:36-36 variable listeners
const listeners = new Set<() => void>();

// src/ui/roads/keys.ts:38-51 function load
function load(): Record<RoadKeyAction, string>;

// src/ui/roads/keys.ts:86-89 function roadKeyAction
/** The road action a key press asks for, or null. */
export function roadKeyAction(key: string): RoadKeyAction | null;

// src/ui/roads/keys.ts:91-94 function onRoadKeysChange
export function onRoadKeysChange(listener: () => void): () => void;

// src/ui/roads/keys.ts:97-102 function keyLabel
/** A key as the interface writes it: "Page Up", "V", "F2". */
export function keyLabel(key: string, short = false): string;

// src/ui/roads/keys.ts:105-108 function roadKeyParams
/** The keys as placeholders for the interface's sentences (`{heightUp}`, `{heightDown}`, `{alignmentKey}`). */
export function roadKeyParams(): Record<string, string>;

// src/ui/roads/keys.ts:109-110 variable paramsOf
const paramsOf = identityMemo((b: Record<RoadKeyAction, string>): Record<string, string> =>
  ({ heightUp: keyLabel(b.heightUp), heightDown: keyLabel(b.heightDown), alignmentKey: keyLabel(b.alignment) }));
