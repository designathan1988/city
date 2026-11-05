// What the feature uses of src/ui/roads/keys.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/roads/keys.ts:5-5 type RoadKeyAction
/** THE ROAD TOOL'S KEYS, rebindable (read by `main.ts`) */
export type RoadKeyAction = 'heightUp' | 'heightDown' | 'alignment';

// src/ui/roads/keys.ts:86-89 function roadKeyAction
/** The road action a key press asks for, or null. */
export function roadKeyAction(key: string): RoadKeyAction | null;
