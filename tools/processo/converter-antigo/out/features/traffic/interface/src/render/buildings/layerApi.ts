// What the feature uses of src/render/buildings/layerApi.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/layerApi.ts:11-18 interface BuildingPreviewInput
/** DO NOT put the detail of every cell together again, and DO NOT hold a far cell's detail for a while */
export interface BuildingPreviewInput {
  readonly building: Building;
  readonly valid: boolean;
  readonly hides: BuildingId | null;
  readonly serial: number;
  /** Drawn in the building's own materials, not as a ghost (the interior view). */
  readonly solid?: boolean;
}

// src/render/buildings/layerApi.ts:102-111 interface CutawaySpec
export interface CutawaySpec {
  readonly level: number;
  /** The way the camera looks, world and horizontal: the walls facing it come down. */
  readonly view: { readonly x: number; readonly y: number };
  readonly x: number;
  readonly y: number;
  readonly radius: number;
  /** Only this building is cut open (the one the player clicked); the rest stand whole. */
  readonly only?: BuildingId;
}
