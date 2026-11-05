// What the feature uses of src/actionsWiring.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/actionsWiring.ts:24-24 variable RAY_ABOVE
/** How high over the ground under the pointer a building can reach: the tallest stack of the tallest storeys, its roof and plinth. */
const RAY_ABOVE = MAX_STOREYS * MAX_STOREY_HEIGHT + m(40);

// src/actionsWiring.ts:26-26 variable RAY_BELOW
/** How far under the ground under the pointer a building can stand (one in a valley seen over a ridge). */
const RAY_BELOW = m(120);

// src/actionsWiring.ts:33-55 interface ActionsDeps
/** THE ACTIONS (the dock's Actions: the pistol and the bomb, and the city on fire after) */
export interface ActionsDeps {
  readonly doc: RoadDoc;
  readonly net: Network;
  readonly sim: SimClient;
  readonly scene: SceneHandle;
  /** The building drawn under a screen point (`buildingsWiring.ts` tool). */
  buildingAt(screen: Vec2): number | null;
  view(): Viewport;
  /** The canvas size, CSS pixels. */
  size(): { readonly w: number; readonly h: number };
  /** The height of what is drawn at a point: ground, road or plate. */
  heightAt(p: Vec2): number;
  /** The bomb's force (`ui/toolChoices.ts` `strikeChoice.strength`). */
  strength(): number;
  /** An edit of the document, one undo step. */
  mutate(fn: () => boolean): void;
  /** Runs `fn` with the diary's cause set (`main.ts` `caused`). */
  caused<T>(cause: string, fn: () => T): T;
  hint(key: string): void;
  redraw(): void;
  /** Nothing grows on zoned lots for this many seconds of the simulation's time (rubble lying). */
  holdGrowth(seconds: number): void;
}

// src/actionsWiring.ts:57-66 interface Actions
export interface Actions {
  /** The bomb at a screen point (`world` the ground under it). */
  strikeAt(sx: number, sy: number, world: Vec2): void;
  /** The pistol at a screen point; false when it hit nothing. */
  shootAt(sx: number, sy: number): boolean;
  /** An explosion at `world`, height `z` (on building `b` when it landed on one). */
  explodeAt(world: Vec2, z: number, b: Building | null, strength: number, quiet?: boolean): void;
  /** The fires, at the simulation's time `now` (seconds, `SimClock.time`) */
  tick(now: number): void;
}

// src/actionsWiring.ts:68-475 function createActions
export function createActions(deps: ActionsDeps): Actions;
