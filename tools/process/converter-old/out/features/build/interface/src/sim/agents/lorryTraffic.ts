// What the feature uses of src/sim/agents/lorryTraffic.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lorryTraffic.ts:28-28 variable CALL_REACH
/** Farthest a passing lorry is called from, and how long it has to reach the gate. */
const CALL_REACH = m(400);

// src/sim/agents/lorryTraffic.ts:42-42 type Phase
type Phase = 'coming' | 'waiting' | 'arriving' | 'docked' | 'leaving' | 'joining';

// src/sim/agents/lorryTraffic.ts:44-67 interface Lorry
interface Lorry {
  readonly id: VehicleId;
  readonly building: BuildingId;
  readonly dock: number;
  phase: Phase;
  /** The lorry as drawn off the road; null while it drives in the traffic. */
  body: Vehicle | null;
  shunt: Shunt | null;
  /** The two ways, in the world. */
  into: LorryWay;
  away: LorryWay;
  /** The lane it stops on before the gate and joins past it. */
  readonly lane: LaneletId;
  readonly stopAt: number;
  readonly joinAt: number;
  /** The stretches of lane its street part sweeps, coming and going. */
  readonly closeIn: Closure[];
  readonly closeOut: Closure[];
  /** Lanes closed now. */
  closed: boolean;
  t: number;
  stay: number;
  waited: number;
}

// src/sim/agents/lorryTraffic.ts:73-73 field LorryTraffic.planning
/** The dock being planned now. */
  private planning: { building: BuildingId; dock: number } | null = null;

// src/sim/agents/lorryTraffic.ts:83-89 method LorryTraffic.reset
/** A different map (`LotTraffic.reset`, from `AmbientWorld.reset`) */
  reset(w: SimWorld | null = this.world): void;

// src/sim/agents/lorryTraffic.ts:165-165 field LorryTraffic.yards
private yards: readonly { building: BuildingId; docks: number }[] = [];

// src/sim/agents/lorryTraffic.ts:168-174 method LorryTraffic.readYards
/** Only which buildings have docks: a yard is worked out */
  private readYards(w: SimWorld): void;

// src/sim/agents/lorryTraffic.ts:180-201 method LorryTraffic.yardFor
/** A building's yard, made a slice a call (`buildYard`): undefined while it is being made. */
  private yardFor(w: SimWorld, building: BuildingId, begin = true): Yard | null | undefined;

// src/sim/agents/lorryTraffic.ts:204-215 method LorryTraffic.plan
/** A slice of planning for the dock in hand (a slice of making its yard first). */
  private plan(w: SimWorld): void;

// src/sim/agents/lorryTraffic.ts:218-262 method LorryTraffic.call
/** A yard with no lorry, a dock of it drawn; planned, it is sent the nearest passing lorry. */
  private call(w: SimWorld): void;

// src/sim/agents/lorryTraffic.ts:460-460 field YardList.docks
private readonly docks = new Map<BuildingId, number>();

// src/sim/agents/lorryTraffic.ts:461-461 field YardList.list
private list: { building: BuildingId; docks: number }[] = [];

// src/sim/agents/lorryTraffic.ts:463-463 field YardList.gone
/** The buildings the last `read` took out (gone, or with no yard now); null: it read the town whole. */
  gone: BuildingId[] | null = [];

// src/sim/agents/lorryTraffic.ts:466-468 method YardList.has
/** Whether a building has a yard, as of the last `read`. */
  has(id: BuildingId): boolean;

// src/sim/agents/lorryTraffic.ts:470-482 method YardList.read
read(w: BuildingSource): readonly { building: BuildingId; docks: number }[];
