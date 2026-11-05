// What the feature uses of src/sim/meso/memory.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/meso/memory.ts:34-48 interface MesoDelta
/**
 * The links that changed since the last delta the page took, as the worker sends them (in the frame's message, its three buffers
 * transferred). DO NOT send every link each step: a link goes only when what the page draws of it changed (its rounded count or
 * queue, its speed past a step, its flags, its line), so a step costs the page the links that moved, not the map.
 */
export interface MesoDelta {
  /** The page's copy is cleared first (a new map, the page's first window): the links in it are every link there is. */
  readonly whole: boolean;
  /** One past the highest slot the worker holds. */
  readonly slots: number;
  /** The simulation's time of the step and the micro window it stood at. */
  readonly time: number;
  readonly windowX: number;
  readonly windowY: number;
  readonly windowR: number;
  /** Each changed link's slot, its link id (`tileLinks.ts` linkId), and its record (`MESO_RECORD` numbers, fields `ML`). */
  readonly at: Int32Array;
  readonly ids: Float64Array;
  readonly records: Float32Array;
}
