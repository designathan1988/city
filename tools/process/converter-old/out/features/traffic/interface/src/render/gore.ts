// What the feature uses of src/render/gore.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gore.ts:10-10 type Ends
/** What a body opened by a bullet or a blast shows */

/** Three's frame (y up). */
type Ends = () => readonly [Vector3, Vector3] | null;

// src/render/gore.ts:12-24 interface Gore
export interface Gore {
  readonly group: Group;
  /** Guts out of an opened belly: hanging from `anchor()` */
  spill(anchor: () => Vector3 | null, kick: Vector3): void;
  /** Organs dropped from a point (a body torn open), thrown by `kick`. */
  scatter(at: Vector3, count: number, kick: Vector3): void;
  /** A bone's end out of a stump: drawn from `ends()` (the joint, the tip) while it returns them. */
  bone(ends: Ends): void;
  update(dt: number, groundAt: (x: number, y: number) => number): void;
  /** All of it gone. */
  clear(): void;
  dispose(): void;
}
