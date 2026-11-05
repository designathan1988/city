// What the feature uses of src/ui/inspector.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/inspector.ts:23-26 interface InspectorSelection
export interface InspectorSelection {
  readonly segment: SegmentId | null;
  readonly node: NodeId | null;
}

// src/ui/inspector.ts:28-70 interface InspectorActions
export interface InspectorActions {
  readonly onUpgrade: (id: SegmentId) => void;
  readonly onSetType: (id: SegmentId, type: number) => void;
  readonly onSetLanes?: (id: SegmentId, lanes: number | null) => void;
  readonly onSetSection?: (id: SegmentId, section: RoadSection | undefined) => void;
  /** A whole cross-section applied to the road, with its class when it comes from a template. */
  readonly onApplyProfile?: (id: SegmentId, profile: RoadProfileSpec, type?: number) => void;
  /** V8: this road's profile copied to the whole street (`world/roads/streetChain.ts`); how many segments that is. */
  readonly onProfileToStreet?: (id: SegmentId) => void;
  readonly streetLength?: (id: SegmentId) => number;
  /** V8: the road tool taken up with this road's profile (the eyedropper). */
  readonly onDrawWithProfile?: (held: DrawProfile) => void;
  readonly onSetParking?: (id: SegmentId, parking: SegmentParking) => void;
  readonly onDelete: (id: SegmentId) => void;
  readonly onSetDirection?: (id: SegmentId, direction: SegmentDirection) => void;
  readonly onSetNodeHeight?: (id: NodeId, metres: number) => void;
  /** V8: a road's end as the map's edge (undefined) or a turning circle. */
  readonly onSetNodeEnd?: (id: NodeId, end: RoadEnd | undefined) => void;
  readonly onReverseDirection?: (id: SegmentId) => void;
  readonly onSplit?: (id: SegmentId) => void;
  /** Places a mid-block pedestrian crossing on the road (`commitPedestrianCrossing`). */
  readonly onAddCrossing?: (id: SegmentId, kind: NodeCrossingKind) => void;
  readonly onRemoveCrossing?: (node: NodeId) => void;
  /** V8: a U-turn through the median where the road was clicked, or its middle (`commitUturn`). */
  readonly onAddUturn?: (id: SegmentId, toward: 'a' | 'b') => void;
  readonly onAddHeightPoint?: (id: SegmentId) => void;
  readonly onDuplicate?: (id: SegmentId) => void;
  readonly onSetControl?: (id: NodeId, control: JunctionControl) => void;
  readonly onSetMovementBlocked?: (node: NodeId, from: SegmentId, to: SegmentId, blocked: boolean) => void;
  /** The player's lane connections at a node; undefined: all derived. */
  readonly onSetLaneLinks?: (node: NodeId, links: LaneLink[] | undefined) => void;
  /** Each leg's rule at a junction; undefined: derived. */
  readonly onSetApproachRules?: (node: NodeId, rules: ApproachRuleEntry[] | undefined) => void;
  /** A junction's signal settings (V5); and several at once (a green wave). */
  readonly onSetSignal?: (node: NodeId, settings: SignalSettings | undefined) => void;
  readonly onSetSignals?: (settings: ReadonlyMap<NodeId, SignalSettings>) => void;
  /** World to screen, for plans drawn as the player sees the map (the lane connectors). */
  readonly project?: (x: number, y: number) => { readonly x: number; readonly y: number };
  readonly onSetCurve?: (id: SegmentId, curve: CurveShape | null) => void;
  readonly onJoin?: (node: NodeId) => void;
  /** Removes a node and the roads that meet at it. Offered only for a node the editor could not have created. */
  readonly onRemoveNode?: (node: NodeId) => void;
}

// src/ui/inspector.ts:86-96 function openInspector
/** Inspector panel. */
export function openInspector(
  doc: RoadDoc,
  net: Network,
  sim: SimClient,
  selection: InspectorSelection,
  actions: InspectorActions,
): void;
