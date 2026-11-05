// What the feature uses of src/world/buildings/blueprints.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/blueprints.ts:21-21 type BlueprintBody
/** Reusable building definitions: everything a building is except where it stands. */
export type BlueprintBody = Omit<Building, 'id' | 'x' | 'y' | 'rotation'>;

// src/world/buildings/blueprints.ts:23-29 interface Blueprint
export interface Blueprint {
  readonly key: string;
  /** Translation key for a built-in preset; user blueprints carry `name`. */
  readonly nameKey?: string;
  readonly name?: string;
  readonly body: BlueprintBody;
}

// src/world/buildings/blueprints.ts:32-34 function instantiate
/** A building record for a blueprint, placed so the middle of its FRONT edge (the street side) is at `anchor`. */
export function instantiate(body: BlueprintBody, anchor: Vec2, rotation: number, blueprint?: string): Omit<Building, 'id'>;

// src/world/buildings/blueprints.ts:37-46 function placeBody
/** DO NOT copy the body for a ghost: every pointer move wrote the whole body to JSON and read it back to move it. */
export function placeBody(body: BlueprintBody, anchor: Vec2, rotation: number, blueprint?: string): Omit<Building, 'id'>;

// src/world/buildings/blueprints.ts:49-49 variable DEFAULT_STOREY_HEIGHT
export const DEFAULT_STOREY_HEIGHT = m(3.1);

// src/world/buildings/blueprints.ts:108-108 function bays
/** `n` default modules, world units: how the presets below are measured. */
const bays = (n: number): number => …;

// src/world/buildings/blueprints.ts:150-169 function upperStoreyFrom
/** A storey for above `source`, in the building's own style rather than a category's: the same facade, with every way in */
export function upperStoreyFrom(source: Storey): Storey;

// src/world/buildings/blueprints.ts:172-201 function generateBlock
/** The neutral starting block: no category, just a mass of `w x d` units and `storeys` storeys with windows all round */
export function generateBlock(
  w: number,
  d: number,
  storeys: number,
  options: { module?: number; groundHeight?: number; storeyHeight?: number; roof?: RoofKind; palette?: number } = {},
): BlueprintBody;

// src/world/buildings/blueprints.ts:208-210 function house
function house(): BlueprintBody;

// src/world/buildings/blueprints.ts:212-218 function rowhouse
function rowhouse(): BlueprintBody;

// src/world/buildings/blueprints.ts:220-230 function apartments
function apartments(): BlueprintBody;

// src/world/buildings/blueprints.ts:232-243 function tower
function tower(): BlueprintBody;

// src/world/buildings/blueprints.ts:245-249 function shop
function shop(): BlueprintBody;

// src/world/buildings/blueprints.ts:251-256 function office
function office(): BlueprintBody;

// src/world/buildings/blueprints.ts:258-261 function mixedBlock
function mixedBlock(): BlueprintBody;

// src/world/buildings/blueprints.ts:264-280 function courtyard
/** Four individually editable wings around an open, ground-level courtyard. */
function courtyard(): BlueprintBody;

// src/world/buildings/blueprints.ts:282-284 function warehouse
function warehouse(): BlueprintBody;

// src/world/buildings/blueprints.ts:286-303 function factory
function factory(): BlueprintBody;

// src/world/buildings/blueprints.ts:306-318 variable BLUEPRINTS
/** The built-in presets, in palette order. Names are `building.preset.<key>`. */
export const BLUEPRINTS: readonly Blueprint[] = [
  { key: 'block', nameKey: 'building.preset.block', body: generateBlock(bays(4), bays(3), 2) },
  { key: 'house', nameKey: 'building.preset.house', body: house() },
  { key: 'rowhouse', nameKey: 'building.preset.rowhouse', body: rowhouse() },
  { key: 'apartments', nameKey: 'building.preset.apartments', body: apartments() },
  { key: 'tower', nameKey: 'building.preset.tower', body: tower() },
  { key: 'shop', nameKey: 'building.preset.shop', body: shop() },
  { key: 'office', nameKey: 'building.preset.office', body: office() },
  { key: 'mixed', nameKey: 'building.preset.mixed', body: mixedBlock() },
  { key: 'courtyard', nameKey: 'building.preset.courtyard', body: courtyard() },
  { key: 'warehouse', nameKey: 'building.preset.warehouse', body: warehouse() },
  { key: 'factory', nameKey: 'building.preset.factory', body: factory() },
];
