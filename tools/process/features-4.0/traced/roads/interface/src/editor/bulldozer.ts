// What the feature uses of src/editor/bulldozer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/bulldozer.ts:13-25 interface BulldozerHost
/** What the bulldozer reads of the game and does to it. */
export interface BulldozerHost {
  readonly doc: RoadDoc;
  readonly net: Network;
  /** The building under screen point `at` taken down; false when there is none. */
  bulldozeBuildingAt(at: Vec2): boolean;
  /** Pick radius of a pole and of a street item, world units at the current zoom. */
  poleReach(): number;
  itemReach(): number;
  /** An edit of the document, one undo step. */
  mutate(fn: () => boolean): void;
  hint(key: string): void;
  redraw(): void;
}

// src/editor/bulldozer.ts:28-36 function removeRoads
/** Roads taken away as the player demolishes them, by any tool: their power line and furniture with them, a share of their price back (`editor/roads/economy.ts`). */
export function removeRoads(doc: RoadDoc, net: Network, ids: readonly SegmentId[]): boolean;

// src/editor/bulldozer.ts:42-150 class Bulldozer
/** THE BULLDOZER: a click removes the one thing under the pointer */
export class Bulldozer { … }

// src/editor/bulldozer.ts:44-44 field Bulldozer.drag
/** The box being dragged (screen pixels in the canvas, and the ground under its corners), or the press while it stays a click. */
  private drag: { pointer: number; a: Vec2; b: Vec2; world: Vec2; to: Vec2; anchor: Anchor } | null = null;

// src/editor/bulldozer.ts:61-63 method Bulldozer.down
/** A press at screen point `screen` over `world`, `anchor` what is under it. */
  down(pointer: number, screen: Vec2, world: Vec2, anchor: Anchor): void;

// src/editor/bulldozer.ts:94-122 method Bulldozer.click
/** @perInput The bulldozer's click: the one thing under the pointer. */
  private click(screen: Vec2, world: Vec2, anchor: Anchor): void;

// src/editor/bulldozer.ts:125-149 method Bulldozer.boxed
/** Everything inside the box removed in one undo step: roads */
  private boxed(a: Vec2, b: Vec2): void;
