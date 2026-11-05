// What the feature uses of src/ui/v2/shell.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/ui/v2/shell.ts:323-335 interface ShellStatus
/** The city's numbers, as `main.ts` computes them for the top bar and the simulation menu. */
export interface ShellStatus {
  readonly clock: string;
  readonly vehicles: string;
  readonly people: string;
  readonly trips: string;
  readonly lost: string;
  readonly meanSpeed: string;
  readonly queued: string;
  /** Junctions that cannot be built (`Network.impossible`). */
  readonly impossible: number;
  /** The money in hand (`world/economy.ts`). */
  readonly balance: number;
}
