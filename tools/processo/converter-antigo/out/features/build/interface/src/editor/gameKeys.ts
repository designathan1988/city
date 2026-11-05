// What the feature uses of src/editor/gameKeys.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/gameKeys.ts:6-52 interface GameKeyDeps
/** What the keys act on: the game's commands, handed in by `main.ts`. */
export interface GameKeyDeps {
  tool(): Tool;
  /** The Builder's own keys; whether it used this one. */
  buildingKey(e: KeyboardEvent): boolean;
  transitKey(key: string): boolean;
  barrierKey(key: string): boolean;
  /** The road tool's rebindable keys (`ui/roads/keys.ts`); whether the key is one of them. */
  roadKey(key: string): boolean;
  /** Whether Inspect has a smooth node picked, which Page Up/Down raise and lower. */
  smoothNodePicked(): boolean;
  stepNodeHeight(metres: 1 | -1): void;
  camera: {
    /** A quarter turn, glided. */
    readonly turn: (radians: number) => void;
    /** A camera key held; whether it is one (`view/cameraMotion.ts`). */
    readonly press: (code: string, shift: boolean) => boolean;
    /** The view the game starts with, framing the map. */
    readonly home: () => void;
  };
  requestDraw(): void;
  saveMap(): void;
  openMap(): void;
  duplicateRoad(): void;
  undo(): void;
  redo(): void;
  /** Whether Inspect has a road picked, which Delete removes. */
  roadPicked(): boolean;
  deleteRoad(): void;
  /** Whether the focus sits on a button the player reached with the keyboard (Space is that button's). */
  keyboardOnButton(e: KeyboardEvent): boolean;
  togglePause(e: KeyboardEvent): void;
  /** The terrain brush: its operation by number (1 = the first), radius and strength steps; false: no such operation. */
  terrainMode(index: number): boolean;
  terrainRadius(step: 1 | -1): void;
  terrainStrength(step: 1 | -1): void;
  roadClassCount: number;
  pickRoadClass(index: number): void;
  toggleChanges(): void;
  /** The F9 monitor (`ui/healthPanel.ts`): toggled, or closed (false: it was not open). */
  health: { toggle(): void; close(): boolean };
  /** The F7 effects panel (`ui/effectsPanel.ts`): toggled, or closed (false: it was not open). */
  effects: { toggle(): void; close(): boolean };
  /** A tool's key: takes the tool or puts it down (`toolMachine` `toggle`), and the drawer follows. */
  toolKey(tool: Tool): void;
  /** Escape, one step back: the stroke, then what Inspect picked, then the tool; false: nothing to put down. */
  escape(): boolean;
}

// src/editor/gameKeys.ts:69-148 function createGameKeys
/** The registry with the game's commands and keys. */
export function createGameKeys(d: GameKeyDeps): CommandRegistry<KeyboardEvent>;
