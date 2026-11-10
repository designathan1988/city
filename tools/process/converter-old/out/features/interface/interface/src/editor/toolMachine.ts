// What the feature uses of src/editor/toolMachine.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/toolMachine.ts:4-20 type Tool
export type Tool =
  | 'building'
  | 'zone'
  | 'road'
  | 'roundabout'
  | 'terrain'
  | 'upgrade'
  | 'move'
  | 'split'
  | 'bulldoze'
  | 'control'
  | 'inspect'
  | 'pole'
  | 'streetscape'
  | 'barrier'
  | 'transit'
  | 'person';

// src/editor/toolMachine.ts:28-30 function isTool
export function isTool(name: string): name is Tool;

// src/editor/toolMachine.ts:32-35 type ToolEvent
type ToolEvent =
  | { readonly type: 'pick'; readonly tool: Tool }
  | { readonly type: 'toggle'; readonly tool: Tool }
  | { readonly type: 'putDown' };

// src/editor/toolMachine.ts:38-43 interface ToolHands
/** What the machine does to the game: take a tool, let go of what Inspect picked. */
export interface ToolHands {
  /** Takes `tool` in hand: gestures cancelled, the store written, the Builder woken or put to sleep. */
  take(tool: Tool): void;
  /** Lets go of the road or junction Inspect picked, and its panel. */
  letGo(): void;
}

// src/editor/toolMachine.ts:45-49 interface ToolMachine
export interface ToolMachine {
  readonly tool: Tool;
  send(event: ToolEvent): void;
  latest(n: number): TransitionRecord[];
}

// src/editor/toolMachine.ts:52-85 function createToolMachine
/** A machine whose state is `start` (the store's tool). */
export function createToolMachine(start: Tool, hands: ToolHands, now?: () => number): ToolMachine;
