// What the feature uses of src/world/waiting.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/waiting.ts:2-2 variable WAITING
/** Yielded by a preparation while the workers do its pure work (`ConflictIndex.prepare`, `Walkways.updateSteps`) */
export const WAITING = 'waiting' as const;

// src/world/waiting.ts:5-13 function answered
/** The workers' answer, `WAITING` yielded until it comes; null when they failed, and the page then does `what` as before. */
export function* answered<T>(asked: Promise<T>, what: string): Generator<typeof WAITING, T | null>;
