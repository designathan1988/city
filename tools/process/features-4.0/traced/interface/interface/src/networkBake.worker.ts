// What the feature uses of src/networkBake.worker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/networkBake.worker.ts:12-19 interface NetworkBakeAsk
/**
 * A CITY'S NETWORK SOLVED OFF THE PAGE WHILE THE CURTAIN IS UP (`main.ts` openCityNow): the document opened here as the page opens it
 * (`editor/history.ts` restoreInto), its network built whole and handed back as solved (`Network.exportBake`); the page's own build
 * takes every junction and ribbon whose inputs are the same (`NetworkBuild.takeBakes`) and solves only the others. DO NOT solve a large
 * city's every junction on the page: 1.6 s of its opening blocked in `Network.rebuild` for the city of 70,000 buildings (2026-10-22).
 */

export interface NetworkBakeAsk {
  readonly id: number;
  /** The document as the page restores it (its buildings in `table`, `prepareCity`). */
  readonly data: SerializedDoc;
  readonly table: ArrayBufferLike | null;
  /** Whether the page restores it unrepaired (`restoreInto`: a generated city). */
  readonly generated: boolean;
}
