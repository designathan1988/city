// What the feature uses of src/sim/agents/lorryTraffic.ts (shared with the program): declarations as the source writes them, bodies left out.

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

// src/sim/agents/lorryTraffic.ts:69-419 class LorryTraffic
export class LorryTraffic { … }

// src/sim/agents/lorryTraffic.ts:70-70 field LorryTraffic.lorries
private readonly lorries = new Map<VehicleId, Lorry>();

// src/sim/agents/lorryTraffic.ts:71-71 field LorryTraffic.clock
private clock = 0;

// src/sim/agents/lorryTraffic.ts:73-73 field LorryTraffic.planning
/** The dock being planned now. */
  private planning: { building: BuildingId; dock: number } | null = null;

// src/sim/agents/lorryTraffic.ts:80-80 field LorryTraffic.world
/** The world last stepped: a reset opens the lanes its lorries closed there. */
  private world: SimWorld | null = null;

// src/sim/agents/lorryTraffic.ts:118-138 method LorryTraffic.step
/** Stepped by the lots' traffic (`lotTraffic.ts`) each tick. `carsBusy`: whether a lot's own cars are moving now. */
  step(w: SimWorld, carsBusy: (building: number) => boolean): void;

// src/sim/agents/lorryTraffic.ts:177-177 field LorryTraffic.making
/** The yard being made, a slice a call, with what it is made from (`yardInputs`) as of the roads it was last checked on (`key`). */
  private making: { building: TrafficBuilding; key: string; inputs: number; steps: Generator<void, Yard, void> } | null = null;

// src/sim/agents/lorryTraffic.ts:381-397 method LorryTraffic.mayGo
/** Whether nobody stands where the lorry goes over the next few metres (its body as discs) */
  private mayGo(w: SimWorld, lorry: Lorry, shunt: Shunt): boolean;

// src/sim/agents/lorryTraffic.ts:406-409 method LorryTraffic.drop
private drop(w: SimWorld, lorry: Lorry): void;
