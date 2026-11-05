// What the feature uses of src/sim/agents/lotTraffic.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lotTraffic.ts:43-43 variable JOIN_SPEED
/** Speed a car joins the lane at. */
const JOIN_SPEED = m(2.4);

// src/sim/agents/lotTraffic.ts:53-53 type Phase
type Phase = 'coming' | 'parking' | 'parked' | 'leaving';

// src/sim/agents/lotTraffic.ts:55-81 interface LotCar
interface LotCar {
  readonly id: VehicleId;
  bay: Bay;
  phase: Phase;
  /** The car as drawn off the road; null while it drives in the traffic. */
  body: Vehicle | null;
  path: Manoeuvre | null;
  /** Seconds in the phase. */
  t: number;
  /** The lots' time (`LotTraffic.time`) its least stay parked is over at. */
  until: number;
  /** Sent out (`depart`): it leaves once its stay is over. */
  go: boolean;
  /** Seconds waited for people on the way. */
  waited: number;
  /** Holding the ground of the rest of its way: it drives on without stopping for anybody. */
  reserved: boolean;
  /** Its place in the lots' order (`track`), and its slot among the cars `depart` may draw (`waiting`). */
  readonly order: number;
  slot: number;
  /** Parked, its least stay over and not sent out (`ripen`): drawn by its bay's building's class. */
  ripe: boolean;
  kind: number;
  /** Where its bay is and where its body stands (`spotKey`): neither place a free bay. */
  readonly bayAt: number;
  bodyAt: number | null;
}

// src/sim/agents/lotTraffic.ts:619-648 method LotTraffic.leaving
private leaving(w: SimWorld, car: LotCar): void;
