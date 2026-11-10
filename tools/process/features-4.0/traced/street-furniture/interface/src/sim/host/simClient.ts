// What the feature uses of src/sim/host/simClient.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/simClient.ts:58-66 interface SignalFeed
/** A junction's signal heads and their lights as the worker last told them (`render/signals.ts`). */
export interface SignalFeed {
  /** The junctions whose heads moved since `serial` (null: every junction), and the serial now. */
  changesSince(serial: number): { readonly serial: number; readonly nodes: readonly NodeId[] | null };
  layout(node: NodeId): SignalLayout | undefined;
  /** A group's colour, and the junction's stage key (moves when any of its lights does). */
  state(node: NodeId, group: number): SignalState;
  stage(node: NodeId): string;
  nodes(): Iterable<NodeId>;
}

// src/sim/host/simClient.ts:411-421 field SimClient.signals
readonly signals: SignalFeed = {
    changesSince: (serial) => {
      const now = this.signalBase + this.signalLog.length;
      if (serial < this.signalAll || serial < this.signalBase || serial > now) return { serial: now, nodes: null };
      return { serial: now, nodes: this.signalLog.slice(serial - this.signalBase) };
    },
    layout: (node) => this.layouts.get(node),
    state: (node, group) => this.lights.get(node)?.states.get(group) ?? 'red',
    stage: (node) => this.lights.get(node)?.stage ?? '',
    nodes: () => this.layouts.keys(),
  };
