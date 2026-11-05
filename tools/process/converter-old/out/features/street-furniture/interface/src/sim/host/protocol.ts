// What the feature uses of src/sim/host/protocol.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/protocol.ts:70-73 interface SignalLayout
/** A signalised junction's heads by group (`render/signals.ts`): which roads each group lights. */
export interface SignalLayout {
  readonly node: NodeId;
  readonly groups: readonly { readonly id: number; readonly segments: readonly SegmentId[] }[];
}
