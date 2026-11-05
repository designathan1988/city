// What the feature uses of src/sim/agents/lotBays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lotBays.ts:26-26 type LotBlock
/** What a lot's bays read of a block of its building (`bayRows`, `stallRoom`, `gatedProperty`). */
type LotBlock = Pick<Volume, 'x' | 'y' | 'w' | 'd' | 'open' | 'base'>;

// src/sim/agents/lotBays.ts:28-35 interface LotRecord
/** What a lot's bays read of its building's record: its frame, its blocks' boxes and its parts. */
export interface LotRecord {
  readonly id: BuildingId;
  readonly x: number;
  readonly y: number;
  readonly rotation: number;
  readonly volumes: readonly LotBlock[];
  readonly elements?: readonly LotPart[];
}

// src/sim/agents/lotBays.ts:41-45 interface WallRing
/** A building's solid footprint, world frame, with its bounds. */
export interface WallRing {
  readonly building: BuildingId;
  readonly ring: readonly { x: number; y: number }[];
  readonly x0: number; readonly y0: number; readonly x1: number; readonly y1: number;
}

// src/sim/agents/lotBays.ts:60-62 function stallFits
/** Whether a body this long stands wholly inside the stall, parked centred in it. */
export function stallFits(bay: Pick<Bay, 'room'>, length: number): boolean;

// src/sim/agents/lotBays.ts:242-245 function personGates
/** A building's people's gates: the `gate` elements narrower than a car's (`carGates`). */
export function personGates<E extends LotPart = BuildingElement>(b: { readonly elements?: readonly E[] }): E[];

// src/sim/agents/lotBays.ts:394-400 interface LotAsk
/** One lot to work out away from the page, as the page reads it: the record, the lanes and walls within its reach. */
export interface LotAsk {
  readonly building: LotRecord;
  /** The lanes within the lot's reach (`LaneGrid.near`), in id order, each with its flags now. */
  readonly lanes: readonly { readonly lane: Lanelet; readonly ghost: boolean; readonly bothWays: boolean }[];
  /** The walls within the lot's reach. */
  readonly walls: readonly WallRing[];
}
