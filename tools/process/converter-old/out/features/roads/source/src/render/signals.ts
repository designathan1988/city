// The code of src/render/signals.ts that is the feature's (the rest of the file is shared).

// src/render/signals.ts:60-65 slice of src/render/signals.ts#SignalHeads (shared): the feature's statements; "…" marks code left out
export interface SignalHeads {
  // … 2 lines
  sync(feed: SignalFeed, net: Network, detailed: boolean, frustum?: Frustum): void;
// … 1 lines
}

// src/render/signals.ts:105-730 slice of src/render/signals.ts#createSignalHeads (shared): the feature's statements; "…" marks code left out
export function createSignalHeads(
// … 2 lines
): SignalHeads {
  // … 385 lines
  let placedNetwork: Network | null = null;
// … 235 lines
}

// src/render/signals.ts:551-577 slice of src/render/signals.ts#createSignalHeads.junctionOf (shared): the feature's statements; "…" marks code left out
  const junctionOf = (feed: SignalFeed, net: Network, node: NodeId): Junction | null => {
    const layout = feed.layout(node);
    if (!layout?.groups.length) return null;
    const nodeRecord = net.doc.node(node);
    if (!nodeRecord) return null;
    const heads: Head[] = [];
    for (const segmentId of nodeRecord.incident) {
      const segment = net.doc.segment(segmentId);
      if (!segment) continue;
      const signalGroup = layout.groups.find((candidate) => candidate.segments.includes(segmentId));
      if (!signalGroup) continue;
      // Where the post stands is the world's (`world/signalPosts.ts`): the pedestrians walk round the same post.
      const post = signalPostPlace(net, node, segmentId);
      if (!post) continue;
      heads.push({
        x: post.x,
        height: elevationAt(post.x, post.y, segmentId),
        y: post.y,
        // Local +X carries the arm inward over the road.
        yaw: post.yaw,
        groupId: signalGroup.id,
        key: `${node}:${segmentId}`,
        state: null,
      });
    }
    return heads.length ? { node, heads, stage: '' } : null;
  };

// src/render/signals.ts:617-631 slice of src/render/signals.ts#createSignalHeads.placeAll (shared): the feature's statements; "…" marks code left out
  const placeAll = (feed: SignalFeed, net: Network): void => {
    dropCells();
    shown.clear();
    const byCell = new Map<number, Junction[]>();
    for (const node of feed.nodes()) {
      const entry = junctionOf(feed, net, node);
      if (!entry) continue;
      const key = cellKeyOf(entry);
      junctionCell.set(node, key);
      const list = byCell.get(key);
      if (list) list.push(entry); else byCell.set(key, [entry]);
    }
    for (const [key, junctions] of byCell) buildCell(key, junctions);
    countLamps();
  };

// src/render/signals.ts:634-658 slice of src/render/signals.ts#createSignalHeads.placeSome (shared): the feature's statements; "…" marks code left out
  const placeSome = (feed: SignalFeed, net: Network, nodes: Iterable<NodeId>): void => {
    const touched = new Map<number, Junction[]>();
    const cellJunctions = (key: number): Junction[] => {
      let list = touched.get(key);
      if (!list) touched.set(key, list = [...(cells.get(key)?.junctions ?? [])]);
      return list;
    };
    for (const node of new Set(nodes)) {
      const was = junctionCell.get(node);
      if (was !== undefined) {
        const list = cellJunctions(was);
        const at = list.findIndex((j) => j.node === node);
        if (at >= 0) for (const head of list[at]!.heads) shown.delete(head.key);
        if (at >= 0) list.splice(at, 1);
        junctionCell.delete(node);
      }
      const entry = junctionOf(feed, net, node);
      if (!entry) continue;
      const key = cellKeyOf(entry);
      junctionCell.set(node, key);
      cellJunctions(key).push(entry);
    }
    for (const [key, junctions] of touched) buildCell(key, junctions);
    countLamps();
  };

// src/render/signals.ts:662-707 slice of src/render/signals.ts#createSignalHeads.sync (shared): the feature's statements; "…" marks code left out
    sync(feed, net, detailed, frustum) {
      // … 7 lines
      const land = landRead.read(net.doc.changes);
      if (placedNetwork !== net || log.nodes === null || land === null || land.some((e) => e.rects === null)) {
        placedNetwork = net;
        placeAll(feed, net);
      } else {
        const nodes = new Set<NodeId>(log.nodes);
        for (const entry of land) {
          for (const rect of entry.rects!) {
            for (const id of net.segmentCells.idsMeeting(rect)) {
              const segment = net.doc.segment(id);
              if (segment) { nodes.add(segment.a); nodes.add(segment.b); }
            }
          }
        }
        if (nodes.size) placeSome(feed, net, nodes);
      }
    // … 21 lines
    },
