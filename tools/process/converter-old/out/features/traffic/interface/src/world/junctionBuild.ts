// What the feature uses of src/world/junctionBuild.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/junctionBuild.ts:23-23 function connectorId
/** DO NOT read the document, the network or the graph here: a worker has none of them. */

export const connectorId = (from: string, to: string): string => …;

// src/world/junctionBuild.ts:26-28 interface JunctionLane
/** A link lane arriving at or leaving the junction (`LaneletGraph.buildSegmentLinks`), its centre made (`LaneAsk` as sent). */
export interface JunctionLane extends Omit<LaneAsk, 'centre'> {
  readonly centre: Polyline;
}

// src/world/junctionBuild.ts:31-38 interface JunctionInput
/** Everything one junction's connectors are made of, but its surface (`junctionMovements`). */
export interface JunctionInput {
  readonly node: JunctionNode;
  /** Every road meeting it, once each. */
  readonly legs: readonly JunctionLeg[];
  /** The lanes arriving, and leaving, in id order. */
  readonly inbound: readonly JunctionLane[];
  readonly outbound: readonly JunctionLane[];
}

// src/world/junctionBuild.ts:41-53 interface MadeConnector
/** A movement through the junction as `junctionMovements` made it. */
interface MadeConnector {
  /** Its lanes, by their place in `JunctionInput.inbound` and `outbound`. */
  readonly from: number;
  readonly to: number;
  readonly turn: TurnKind;
  readonly carried: boolean;
  readonly group: number;
  readonly speedLimit: number;
  readonly path: Polyline;
  readonly maxBodyClass: BodyClass | -1;
  /** Set when its turn was left to be chosen off the page (`JunctionTurns.defer`): how it is shaped again once chosen. */
  readonly again?: () => TurnPathResult;
}

// src/world/junctionBuild.ts:56-60 interface JunctionMade
/** One junction's movements, in the order made, with its approach groups and its control. */
export interface JunctionMade {
  readonly groups: readonly ApproachGroup[];
  readonly signalised: boolean;
  readonly connectors: readonly MadeConnector[];
}

// src/world/junctionBuild.ts:70-78 function junctionKey
/** The key a junction's connectors are kept under, from its surface's key (`JunctionSurface.key`). */
export function junctionKey(j: JunctionInput, surfaceKey: string): string;

// src/world/junctionBuild.ts:81-84 function legLine
/** A leg's centreline oriented to start at `from` (`geometry.ts` `orientedPolyline`, from the leg's own numbers). */
function legLine(leg: JunctionLeg, from: NodeId): Polyline;

// src/world/junctionBuild.ts:87-225 function junctionMovements
/** The connectors of one junction (`isJunction`), shaped across its surface */
export function junctionMovements(j: JunctionInput, surfaceOf: () => JunctionSurface | null, turns: JunctionTurns): JunctionMade;

// src/world/junctionBuild.ts:116-136 function junctionMovements.addConnector
const addConnector = (i: number, o: number, turn: TurnKind, carried = false): void =>;

// src/world/junctionBuild.ts:275-306 function buildJunctionOf
/** One junction built from what the page sent (`JunctionAsk`) */
export function buildJunctionOf(ask: JunctionAsk, lines: Map<Float64Array, Polyline>, rings: Map<Float64Array, Vec2[]>): JunctionAnswer;

// src/world/junctionBuild.ts:313-320 function classifyTurn
/** Classifies a movement from its heading change. */
export function classifyTurn(inDir: Vec2, outDir: Vec2): TurnKind;

// src/world/junctionBuild.ts:322-334 function shouldSignalise
function shouldSignalise(
  control: JunctionControl,
  incident: readonly SegmentId[],
  leg: (segment: SegmentId) => JunctionLeg,
): boolean;

// src/world/junctionBuild.ts:336-337 function turnSpeedFactor
const turnSpeedFactor = (turn: TurnKind): number => …;

// src/world/junctionBuild.ts:345-348 function tangentAtStart
/** The immediate tangent where a lane leaves its node. */
function tangentAtStart(pl: Polyline): Vec2;

// src/world/junctionBuild.ts:355-355 variable STRAIGHT_BEND
/** Below this bend a movement is already classified as straight on. */
const STRAIGHT_BEND = (30 * Math.PI) / 180;

// src/world/junctionBuild.ts:358-384 function carriedPair
/** The two legs that are ONE ROAD carried across a node, when that road bends. */
function carriedPair(node: JunctionNode, leg: (segment: SegmentId) => JunctionLeg): readonly SegmentId[] | null;

// src/world/junctionBuild.ts:387-390 function smoothedDirectionFromNode
/** The direction of a leg once a kink at the node is smoothed away. */
function smoothedDirectionFromNode(pl: Polyline): Vec2;

// src/world/junctionBuild.ts:394-406 function laneIsPlausible
/** Rejects lane pairings that no driver would make */
/** Whether a movement from this lane to that one is one a driver would make. */
function laneIsPlausible(
  inLane: JunctionLane,
  outLane: JunctionLane,
  turn: TurnKind,
  inLanes: number,
  outLanes: number,
): boolean;

// src/world/junctionBuild.ts:409-414 function fallbackTurnRank
/** Prefer a forward continuation when a necessary lane merge has no exact match. */
function fallbackTurnRank(turn: TurnKind): number;
