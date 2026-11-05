// What the feature uses of src/core/commands.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/commands.ts:3-3 type CommandResult
/** THE COMMAND REGISTRY: one table of what the player can do, behind the shortcuts, the dock, the drawers */

export type CommandResult = 'done' | 'cancelled' | 'pass';

// src/core/commands.ts:5-11 interface Command
export interface Command<Ctx> {
  readonly id: string;
  /** The i18n key of its name. */
  readonly label: string;
  readonly enabled?: (ctx: Ctx) => boolean;
  readonly run: (ctx: Ctx, args?: unknown) => CommandResult;
}

// src/core/commands.ts:13-28 interface KeyRule
export interface KeyRule<Ctx> {
  /** `KeyboardEvent.code` (the physical key: 'KeyZ', 'Digit1', 'Escape'). */
  readonly code?: string;
  /** `KeyboardEvent.key` (the character: 'z', '[', 'PageUp'), or several; without case. */
  readonly key?: string | readonly string[];
  /** Ctrl or Cmd held; left out: either. */
  readonly ctrl?: boolean;
  readonly shift?: boolean;
  readonly alt?: boolean;
  /** When the rule applies; omitted: always. */
  readonly when?: (ctx: Ctx) => boolean;
  readonly command: string;
  readonly args?: unknown;
  /** Whether the page's own use of the key is cancelled when the command acts (default: yes). */
  readonly prevent?: boolean;
}

// src/core/commands.ts:39-87 class CommandRegistry
export class CommandRegistry<Ctx> { … }

// src/core/commands.ts:40-40 field CommandRegistry.commands
private readonly commands = new Map<string, Command<Ctx>>();

// src/core/commands.ts:41-41 field CommandRegistry.keys
private readonly keys: KeyRule<Ctx>[] = [];
