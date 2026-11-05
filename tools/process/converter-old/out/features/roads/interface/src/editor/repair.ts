// What the feature uses of src/editor/repair.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/repair.ts:10-16 interface Candidate
interface Candidate {
  readonly node: NodeId;
  readonly segment: SegmentId;
  readonly s: number;
  readonly at: { readonly x: number; readonly y: number };
  readonly distance: number;
}

// src/editor/repair.ts:19-37 function repairNearConnections
/** Repairs a legacy endpoint that visibly ends inside another road but was saved as a disconnected node. */
export function repairNearConnections(doc: RoadDoc, net: Network): number;

// src/editor/repair.ts:39-71 function nearestCandidate
function nearestCandidate(doc: RoadDoc, net: Network, rejected: ReadonlySet<string>): Candidate | null;
