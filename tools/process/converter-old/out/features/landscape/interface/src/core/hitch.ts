// What the feature uses of src/core/hitch.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/hitch.ts:2-6 function measureHitch
/** A STEP OF THE GAME TIMED for the frame monitor (`ui/healthWatch.ts`) and the profiler (`core/profiler.ts`): a `hitch:` entry of the platform's timeline, in a development build only (decision 5.4, the player's: the game shipped measures nothing) */
export function measureHitch(name: string, start: number, end?: number): void;
