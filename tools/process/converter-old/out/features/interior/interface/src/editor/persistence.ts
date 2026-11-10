// What the feature uses of src/editor/persistence.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/persistence.ts:28-50 interface SavedSettings
export interface SavedSettings {
  readonly camera: {
    readonly x: number;
    readonly y: number;
    readonly zoom: number;
    /** The free camera's bearing and tilt, rad. Absent in saves older than the orbit. */
    readonly azimuth?: number;
    readonly elevation?: number;
  };
  readonly paused: boolean;
  readonly speed: number;
  readonly trafficIntensity: number;
  readonly pedestrianIntensity: number;
  /** Counts of cars and people on foot saved by the builds of 2026-10-06 to 10-11 (10 at first, 400 at most). */
  readonly cars?: number;
  readonly people?: number;
  readonly demandMultiplier?: number;
  readonly congestionOverlay: boolean;
  /** The city's time of day, minutes after midnight of its first day; absent in saves before 2026-10-18 (the city wakes at 06:30). */
  readonly minutes?: number;
  /** Metres per world unit the camera was saved in; absent: 0.4, before 2026-10-10. */
  readonly unit?: number;
}

// src/editor/persistence.ts:55-58 interface SavedSession
export interface SavedSession {
  readonly document: SerializedDoc;
  readonly settings: SavedSettings;
}

// src/editor/persistence.ts:61-345 class Persistence
/** Autosave and file export. */
export class Persistence { … }

// src/editor/persistence.ts:203-210 method Persistence.saveSessionSoon
saveSessionSoon(doc: RoadDoc, settings: () => SavedSettings): void;

// src/editor/persistence.ts:395-395 variable MAX_INTENSITY
/** The panel's Traffic and People: shares of the city's own density (`SimWorld.followCity`), 1 = 100 %, at most this. */
export const MAX_INTENSITY = 3;

// src/editor/persistence.ts:430-433 type ImportResult
/** What a file pick came to: a map, nothing chosen, or a file that is not one. */
export type ImportResult =
  | { readonly status: 'ok'; readonly session: SavedSession }
  | { readonly status: 'cancelled' }
  | { readonly status: 'invalid' };

// src/editor/persistence.ts:436-468 function importFromFile
/** Reads a map file chosen by the user. */
export async function importFromFile(): Promise<ImportResult>;

// src/editor/persistence.ts:625-629 interface LargeStore
/** Where a session too large for Web Storage is kept (`LOCAL_LIMIT_CHARS`). */
export interface LargeStore {
  get(key: string): Promise<string | null>;
  put(key: string, text: string): Promise<boolean>;
  delete(key: string): Promise<boolean>;
}
