// What the feature uses of src/desktopSaves.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/desktopSaves.ts:27-32 interface DesktopStores
/** The desktop's stores: the page's (`new Persistence`), a port of its own for each worker that reads or writes the tiles (`connect`, `editor/workerParts.ts`), and who to tell when the writer stopped (`onLost`: each such port is dead). */
export interface DesktopStores {
  readonly store: LargeStore;
  readonly parts: PartStore;
  connect(): Promise<MessagePort>;
  onLost(listener: () => void): void;
}

// src/desktopSaves.ts:35-114 function desktopSaves
/** The stores, or null where the page has no desktop shell. */
export function desktopSaves(): DesktopStores | null;
