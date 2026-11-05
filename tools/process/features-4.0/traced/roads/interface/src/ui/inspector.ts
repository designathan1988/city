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

// src/ui/inspector.ts:72-78 interface InspectorContext
interface InspectorContext {
  readonly doc: RoadDoc;
  readonly net: Network;
  readonly sim: SimClient;
  readonly selection: InspectorSelection;
  readonly actions: InspectorActions;
}

// src/ui/inspector.ts:80-80 variable current
let current: InspectorContext | null = null;

// src/ui/inspector.ts:83-83 variable builtFor
/** What the panel's controls were last built for. */
let builtFor = '';

// src/ui/inspector.ts:86-96 function openInspector
/** Inspector panel. */
export function openInspector(
  doc: RoadDoc,
  net: Network,
  sim: SimClient,
  selection: InspectorSelection,
  actions: InspectorActions,
): void;

// src/ui/inspector.ts:103-145 function renderCurrent
function renderCurrent(): void;

// src/ui/inspector.ts:160-171 function closeInspector
export function closeInspector(): void;

// src/ui/inspector.ts:175-181 function endSelect
/** A choice of on-street parking for one side: the kinds this class can carry */
/** What a road's end is (V8), for an end that is the end of this road alone: the map's edge or a turning circle. */
function endSelect(doc: RoadDoc, node: NodeId, which: 'A' | 'B'): string;

// src/ui/inspector.ts:183-187 function parkingSelect
function parkingSelect(id: string, label: string, value: ParkingKind, rt: RoadType): string;

// src/ui/inspector.ts:190-208 function segmentStats
/** The live numbers for a road, or null when the road no longer exists (the traffic's, as the simulation's worker last told them). */
function segmentStats(doc: RoadDoc, net: Network, sim: SimClient, id: SegmentId): string | null;

// src/ui/inspector.ts:210-343 function renderSegment
function renderSegment(
  doc: RoadDoc,
  net: Network,
  sim: SimClient,
  body: HTMLElement,
  id: SegmentId,
  actions: InspectorActions,
): void;

// src/ui/inspector.ts:384-444 function renderNode
function renderNode(
  doc: RoadDoc,
  net: Network,
  sim: SimClient,
  body: HTMLElement,
  id: NodeId,
): void;

// src/ui/inspector.ts:447-458 function junctionHost
/** What the junction panel changes, through the inspector's actions. */
function junctionHost(doc: RoadDoc, sim: SimClient, id: NodeId): Parameters<typeof mountJunctionPanel>[1];

// src/ui/inspector.ts:463-465 function actionsForNode
/** Whether the junction's list of movements is unfolded (kept across the panel's rebuilds). */

function actionsForNode(): InspectorActions;

// src/ui/inspector.ts:467-472 function directionOptions
function directionOptions(value: SegmentDirection): string;

// src/ui/inspector.ts:474-485 function laneOptions
function laneOptions(direction: SegmentDirection, configured: number | null, resolved: number): string;

// src/ui/inspector.ts:493-493 function meters
const meters = (units: number): string => …;

// src/ui/inspector.ts:496-497 function curveText
/** A curve's bulge: how far the arc leaves the straight line, signed by side. */
const curveText = (units: number): string => …;

// src/ui/inspector.ts:499-499 function percent
const percent = (fraction: number): string => …;

// src/ui/inspector.ts:502-505 function setTitle
/** Names what is selected in the panel's own heading. */
function setTitle(value: string): void;

// src/ui/inspector.ts:510-511 function grid
const grid = (rows: readonly [string, string][]): string => …;

// src/ui/inspector.ts:514-519 function heightStepper
/** A height as a stepper of the interface, half a metre a step (V5). */
function heightStepper(end: 'A' | 'B', label: string, metres: number): string;
