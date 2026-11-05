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

// src/world/junctionBuild.ts:87-225 function junctionMovements
/** The connectors of one junction (`isJunction`), shaped across its surface */
export function junctionMovements(j: JunctionInput, surfaceOf: () => JunctionSurface | null, turns: JunctionTurns): JunctionMade;

// src/world/junctionBuild.ts:116-136 function junctionMovements.addConnector
const addConnector = (i: number, o: number, turn: TurnKind, carried = false): void =>;
