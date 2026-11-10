// What the feature uses of src/world/roads/laneEnds.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/laneEnds.ts:8-24 function nodeLanes
/** A JUNCTION'S LANES READ FROM THE LANE GRAPH, for the connector editor: apart from `connectors.ts`, which the document reads and the graph does not */

/** What the node's lanes and built connections are, read from the lane graph. */
export function nodeLanes(graph: LaneletGraph, node: NodeId): NodeLanes | null;

// src/world/roads/laneEnds.ts:27-41 function laneEnds
/** `nodeLanes` with each lane's end at the junction (`LaneEnds`), for the connector editor. */
export function laneEnds(graph: LaneletGraph, node: NodeId): LaneEnds | null;
