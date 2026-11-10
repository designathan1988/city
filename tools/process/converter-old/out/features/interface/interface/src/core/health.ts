// What the feature uses of src/core/health.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/health.ts:4-5 type HealthKind
/** THE GAME'S HEALTH: what broke and what was slow, written down as it happens, with what the player was doing */

/** What kind of trouble. */
export type HealthKind =
  | 'error' | 'rejection' | 'console' | 'worker' | 'webgl' | 'shader' | 'storage' | 'invariant' | 'slow-frame' | 'budget';

// src/core/health.ts:8-8 type HealthSeverity
/** `broken`: something failed; `slow`: a frame over the long-frame line; `warning`: a check that did not hold. */
export type HealthSeverity = 'broken' | 'slow' | 'warning';

// src/core/health.ts:10-32 interface HealthEntry
export interface HealthEntry {
  readonly serial: number;
  readonly kind: HealthKind;
  readonly severity: HealthSeverity;
  readonly message: string;
  /** A stack, a log, the scripts of a long frame. */
  readonly detail?: string;
  /** What the player was doing: the tool, the gesture, the latest change of the world. */
  readonly context?: string;
  /** For a slow frame: milliseconds per system, the largest first. */
  readonly systems?: readonly (readonly [string, number])[];
  /** How long, ms (a slow frame). */
  readonly ms?: number;
  /** `performance.now()` of the first time, and of the last repeat. */
  readonly at: number;
  readonly lastAt: number;
  /** Wall-clock time of the first time (`Date.now()`), to read across sessions. */
  readonly wall: number;
  /** How many times it happened (repeats of the same kind and message within `REPEAT_WINDOW_MS`). */
  readonly count: number;
  /** Written in an earlier session of the game (`restore`). */
  readonly earlier?: boolean;
}

// src/core/health.ts:35-35 variable KEPT
/** How many entries are kept. */
const KEPT = 256;

// src/core/health.ts:39-120 class HealthLog
export class HealthLog { … }

// src/core/health.ts:40-40 field HealthLog.serial
private serial = 0;

// src/core/health.ts:41-41 field HealthLog.ring
private readonly ring: (HealthEntry | undefined)[] = new Array<HealthEntry | undefined>(KEPT);

// src/core/health.ts:42-42 field HealthLog.count
private count = 0;

// src/core/health.ts:50-50 field HealthLog.touched
/** Bumped on every write, repeats too: what a panel watches to redraw. */
  private touched = 0;

// src/core/health.ts:56-81 method HealthLog.record
/** Writes an entry, or counts a repeat on the last one of the same kind and message. */
  record(kind: HealthKind, severity: HealthSeverity, message: string, extra: {
    readonly detail?: string; readonly context?: string; readonly systems?: readonly (readonly [string, number])[]; readonly ms?: number;
  } = {}): HealthEntry;

// src/core/health.ts:84-91 method HealthLog.latest
/** The last `n` entries, newest first. */
  latest(n: number): HealthEntry[];

// src/core/health.ts:123-123 variable LONG_FRAME_MS
/** A long frame: past this the player sees a hitch (MDN, Long Animation Frames: 50 ms). */
export const LONG_FRAME_MS = 50;

// src/core/health.ts:132-136 interface FrameRecord
/** One frame's systems, ms each. */
export interface FrameRecord {
  readonly start: number;
  readonly end: number;
  readonly systems: ReadonlyMap<string, number>;
}

// src/core/health.ts:144-179 class FrameTimer
/** Times the systems of each frame: `begin` at the top of the frame, `mark` after each system with its name */
export class FrameTimer { … }

// src/core/health.ts:148-148 field FrameTimer.frames
private readonly frames: FrameRecord[] = [];
