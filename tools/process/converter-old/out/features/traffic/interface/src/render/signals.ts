// What the feature uses of src/render/signals.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/signals.ts:60-65 interface SignalHeads
export interface SignalHeads {
  readonly group: Group;
  /** `frustum`: what the camera sees this frame; the lamps of cells out of it wait until they come in. */
  sync(feed: SignalFeed, net: Network, detailed: boolean, frustum?: Frustum): void;
  dispose(): void;
}

// src/render/signals.ts:72-80 interface Head
interface Head {
  readonly x: number;
  readonly height: number;
  readonly y: number;
  readonly yaw: number;
  readonly groupId: number;
  readonly key: string;
  state: SignalState | null;
}

// src/render/signals.ts:105-730 function createSignalHeads
/** Builds and updates the physical signal heads used by the Three.js renderer. */
export function createSignalHeads(
  scene: Scene,
  elevationAt: (x: number, y: number, segment?: SegmentId) => number = () => 0,
): SignalHeads;
