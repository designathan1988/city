// What the feature uses of src/sim/agents/trafficBuildings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/trafficBuildings.ts:26-26 interface TrafficBox
interface TrafficBox { readonly minX: number; readonly minY: number; readonly maxX: number; readonly maxY: number }

// src/sim/agents/trafficBuildings.ts:28-28 type TrafficBlock
/** What traffic reads of a block: its box in the building's frame, open (a lot) or built, its floor (`lotBays.ts` LotBlock). */
type TrafficBlock = Pick<Volume, 'x' | 'y' | 'w' | 'd' | 'open' | 'base'>;

// src/sim/agents/trafficBuildings.ts:30-30 type TrafficPart
/** What traffic reads of a part of a lot (`lotNav.ts` LotPart), and its id (a dock's element). */
export type TrafficPart = LotPart & Pick<BuildingElement, 'id'>;

// src/sim/agents/trafficBuildings.ts:33-55 interface TrafficBuilding
/** A building as the simulation reads it: its frame, use, blocks and parts, and what the page works out of the whole record */
export interface TrafficBuilding {
  readonly id: BuildingId;
  readonly x: number;
  readonly y: number;
  readonly rotation: number;
  readonly use: BuildingUse;
  readonly function?: BuildingFunction;
  readonly volumes: readonly TrafficBlock[];
  /** Absent when the record has no parts at all (as `Building.elements`). */
  readonly elements?: readonly TrafficPart[];
  /** `storedBounds(b)`: its blocks and its parts, world. */
  readonly bounds: TrafficBox;
  /** `storedBounds(b, LOT_READS)`: the box its lot's bays read within. */
  readonly reach: TrafficBox;
  /** `solidFootprints(b)`: the world rings of its built ground blocks, what nobody drives or walks through. */
  readonly footprints: readonly (readonly Vec2[])[];
  /** `streetDoors(b)`: its doors straight onto the footway. */
  readonly doors: readonly StreetDoor[];
  /** `lorryDocks(b)`: its loading docks. */
  readonly docks: readonly LorryDock[];
  /** `entrancesOf(b)`: its car gates and the edges of its parking, where a kerb keeps no bay. */
  readonly entrances: readonly [Vec2, Vec2][];
}

// src/sim/agents/trafficBuildings.ts:306-309 method TrafficBuildingTable.get
get(id: BuildingId): TrafficBuilding | undefined;

// src/sim/agents/trafficBuildings.ts:414-416 method TrafficBuildingTable.near
near(minX: number, minY: number, maxX: number, maxY: number): TrafficBuilding[];
