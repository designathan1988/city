// What the feature uses of src/sim/roads/greenWave.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/roads/greenWave.ts:8-36 function greenWave
/** A GREEN WAVE along the main road through a junction */
export function greenWave(w: SimWorld, start: NodeId): Map<NodeId, SignalSettings>;

// src/sim/roads/greenWave.ts:38-45 interface CorridorStop
interface CorridorStop {
  readonly node: NodeId;
  /** The leg the platoon arrives on, and the one it leaves by. */
  readonly from: SegmentId;
  readonly to: SegmentId;
  /** Seconds from the first junction, at each road's speed. */
  readonly travel: number;
}

// src/sim/roads/greenWave.ts:48-87 function corridorFrom
/** The junctions along the main road through `start`, in the direction of its higher leg, up to a dozen */
function corridorFrom(w: SimWorld, start: NodeId): CorridorStop[];
