// What the feature uses of src/lab/recorder/recording.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/recorder/recording.ts:5-12 interface RecEvent
/** ONE RECORDING: every frame the lab sees, every event, the camera, the simulation's counts and the settings at each change, kept from Start to Stop. Times are ms since the recording began (page clock). */

export interface RecEvent {
  readonly t: number;
  readonly kind: string;
  readonly detail: string;
  readonly data?: unknown;
  /** A change that cuts the recording into a new stretch (`analysis.ts`). */
  readonly split: boolean;
}

// src/lab/recorder/recording.ts:15-28 interface FrameColumns
/** The frames as columns (one array a measure): compact in memory and in the JSON. */
export interface FrameColumns {
  readonly t: number[];
  readonly interval: number[];
  readonly cpu: number[];
  readonly gpu: (number | null)[];
  readonly calls: number[];
  readonly tris: number[];
  readonly heap: (number | null)[];
  readonly ratio: number[];
  /** The frame's interval spans a pause: not the game's. */
  readonly gap: boolean[];
  /** Each system's ms, aligned with `t` (0 where the frame did not run it). */
  readonly systems: Map<string, number[]>;
}

// src/lab/recorder/recording.ts:30-38 interface ViewSample
export interface ViewSample {
  readonly t: number;
  readonly kind: string;
  readonly x: number;
  readonly y: number;
  readonly zoom: number;
  readonly azimuth: number;
  readonly elevation: number;
}

// src/lab/recorder/recording.ts:40-47 interface SimSample
export interface SimSample {
  readonly t: number;
  readonly vehicles: number | null;
  readonly people: number | null;
  readonly stepMs: number | null;
  /** The renderer's compiled programs: a rise after a change is a recompile. */
  readonly programs: number | null;
}

// src/lab/recorder/recording.ts:49-54 interface SettingsSnapshot
export interface SettingsSnapshot {
  readonly t: number;
  readonly cause: string;
  readonly effects: Readonly<Record<string, unknown>>;
  readonly optimizations: unknown;
}

// src/lab/recorder/recording.ts:56-56 type RecState
export type RecState = 'recording' | 'paused' | 'stopped';

// src/lab/recorder/recording.ts:58-74 interface Recording
export interface Recording {
  readonly startedAt: string;
  readonly t0: number;
  readonly frames: FrameColumns;
  readonly events: RecEvent[];
  readonly views: ViewSample[];
  readonly sim: SimSample[];
  readonly settings: SettingsSnapshot[];
  readonly limitMs: number | null;
  readonly state: RecState;
  /** ms since the start, at the stop (or now). */
  readonly endMs: number;
  /** ms recorded, the pauses left out. */
  readonly activeMs: number;
  /** Why it stopped: the button, the duration reached, the frames' limit. */
  readonly stopReason: string;
}
