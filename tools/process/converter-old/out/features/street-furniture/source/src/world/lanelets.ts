// The code of src/world/lanelets.ts that is the feature's (the rest of the file is shared).

// src/world/lanelets.ts:726-734 method LaneletGraph.junctionConnectors: the feature's own
  static junctionConnectors(doc: RoadDoc, net: Network, nodeId: NodeId): { readonly connectors: readonly Connector[]; readonly centres: ReadonlyMap<ConnectorId, Polyline> } {
    const node = doc.node(nodeId);
    if (!node) return { connectors: [], centres: new Map() };
    const graph = new LaneletGraph();
    for (const segId of [...new Set(node.incident)].sort((a, b) => a - b)) graph.buildSegmentLinks(doc, net, segId, new Map(), new Map());
    graph.buildJunction(doc, net, nodeId, node, new Map(), new Map(), () => undefined);
    const connectors = (graph.junctions.get(nodeId)?.connectors ?? []).map((id) => graph.connectors.get(id)!);
    return { connectors, centres: new Map(connectors.map((c) => [c.id, graph.lanelets.get(c.lanelet)!.centre])) };
  }
