// What the feature uses of src/render/gpuTimer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gpuTimer.ts:6-25 interface GpuTimer
/** THE GPU TIME OF A FRAME */
export interface GpuTimer {
  /** Whether the extension is there (Chrome/Edge on desktop; not every browser). */
  readonly supported: boolean;
  /** The GPU drawing, as the driver names it (`WEBGL_debug_renderer_info`). */
  readonly label: string;
  /** GPU ms of the frames measured, the last 2 400 (40 s at 60 Hz). */
  readonly samples: SampleRing;
  /** Frames not measured: no free query. */
  readonly skipped: number;
  /** Results thrown away: the GPU was disjoint (a clock change, a context loss). */
  readonly disjoint: number;
  /** Whether the frames go unmeasured for a while (`pause`). */
  readonly paused: boolean;
  /** No frame measured while on: the lab times the passes one after another with queries of its own (`lab/gpu/passTimer.ts`), and TIME_ELAPSED queries cannot overlap (EXT_disjoint_timer_query). Set between frames, never inside `begin`/`end`. */
  pause(on: boolean): void;
  begin(): void;
  end(): void;
  /** Reads what came in; once a frame, before `begin`. */
  poll(): void;
}
