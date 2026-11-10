// What the feature uses of src/render/buildings/cellPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/cellPool.ts:34-38 interface CellEnv
/** THE PAGE'S SIDE OF THE CELLS PUT TOGETHER IN WORKERS (`cellWork.ts`, `cell.worker.ts`), through the job system's one queue (`jobPool.ts` `submitJob`) */

/** What a job reads on the page: the ground as drawn and as it was, and the paving (with `pack`: read by the worker itself). */
export interface CellEnv {
  readonly groundAt: GroundAt;
  readonly naturalAt: GroundAt;
  readonly pavedAt?: PavingSource;
}

// src/render/buildings/cellPool.ts:80-103 interface Asked
/** A job asked: what it is written from when a worker takes it (`write`), and where it stands. */
interface Asked {
  readonly id: number;
  readonly kind: JobKind;
  readonly cell: string;
  readonly entries: readonly CellEntry[];
  /** Where each record is read, taken when the job is sent (its tile's shared table: posted as a handle, never copied). */
  readonly writers: readonly (() => RecordRef)[];
  readonly env: CellEnv;
  readonly done: (result: CellResult) => void;
  readonly key: string | null;
  /** Its band now (`cellPriority`): asked again in another when the curtain lifts (`coarseFirst`). */
  priority: number;
  /** Cut open or edited by the player: always first. */
  readonly first: boolean;
  /** Taken by a worker (`write` called): it runs through as asked. */
  sent: boolean;
  readonly place: readonly [number, number] | null;
  /** Where its result is kept between sessions (`CELL_PREFIX`); null: not kept. */
  readonly stored: string | null;
  /** The roads handed with it, and the ground's plates to a job process (`groundAround`): the cell and its reach, wider once the worker said so. */
  rect: [number, number, number, number] | null;
  widened: number;
  ticket: JobTicket | null;
}

// src/render/buildings/cellPool.ts:345-351 method CellPool.onPage
/** The job done here, every height read off the page's own ground. */
  private onPage(asked: Asked): void;
