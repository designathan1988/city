// What the feature uses of src/render/buildings/cellPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/cellPool.ts:34-38 interface CellEnv
/** THE PAGE'S SIDE OF THE CELLS PUT TOGETHER IN WORKERS (`cellWork.ts`, `cell.worker.ts`), through the job system's one queue (`jobPool.ts` `submitJob`) */

/** What a job reads on the page: the ground as drawn and as it was, and the paving (with `pack`: read by the worker itself). */
export interface CellEnv {
  readonly groundAt: GroundAt;
  readonly naturalAt: GroundAt;
  readonly pavedAt?: PavingSource;
}

// src/render/buildings/cellPool.ts:41-41 variable PAVING_REACH
/** How far past its cell a job's buildings read the paving off the roads handed to its worker (`PavingData`) */
const PAVING_REACH = m(64);

// src/render/buildings/cellPool.ts:72-77 function pavingRect
/** The rectangle a job's worker reads the paving in: its square and `PAVING_REACH` round it. */
function pavingRect(cell: string): [number, number, number, number] | null;

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

// src/render/buildings/cellPool.ts:106-106 interface Floor
/** A building's ground floor as a job measured it, and what it was measured on: its record, its ground (`CellEntry`) and whether the paving was read. */
interface Floor { readonly sig: number; readonly digest: number; readonly paved: boolean; readonly floor: number | null }

// src/render/buildings/cellPool.ts:112-156 class FloorColumns
/** THE FLOORS MEASURED, IN COLUMNS BY BUILDING ID (`core/idSlots.ts`): a plate's job measures every building of 2 km round, and a `Map` entry and an object each held ~100 B of JS heap a building of the city. A building cut open (its key `id|...`, a few at a time) is kept in a `Map` of its own. */
class FloorColumns { … }

// src/render/buildings/cellPool.ts:113-113 field FloorColumns.slots
private readonly slots = new IdSlots(1024);

// src/render/buildings/cellPool.ts:118-118 field FloorColumns.cut
private readonly cut = new Map<string, Floor>();

// src/render/buildings/cellPool.ts:159-159 field CellPool.asked
private readonly asked = new Map<number, Asked>();

// src/render/buildings/cellPool.ts:161-161 field CellPool.floors
/** DO NOT measure a building's floor in each job of it (its cell's masses, its tile's, its plate's, its detail, its far level): measured once, handed with the next jobs while its record and ground are the same (`CellEntry.floor`). */
  private readonly floors = new FloorColumns();

// src/render/buildings/cellPool.ts:187-191 constructor CellPool.constructor
/** `workers`: false does every job on the page (tests). `tone`: each finish's colour from afar, for the masses (`kit.ts`). */
  /** `text`: where an entry's record is read (`recordTable.ts`), taken when the job is sent. */
  constructor(private readonly boxes: () => KitBoxes, private readonly text: (entry: CellEntry) => () => RecordRef, private readonly workers = true,
    private readonly tone?: Readonly<Record<Finish, readonly [number, number, number]>>);

// src/render/buildings/cellPool.ts:200-229 method CellPool.run
/** A cell's job: its batches ('cell'), its far level ('facade'), its masses ('mass') or its buildings' summaries; `key`, what a newer job replaces */
  /** `hole`: nothing stands where it draws (a plate of the map with no masses yet): in the far levels' band (`FAR_TIER`), before the view's detail - a hole on the screen comes before any refinement of what is drawn. */
  run(kind: JobKind, cell: string, entries: readonly CellEntry[], env: CellEnv, done: (result: CellResult) => void, key: string | null = null, edited = false, hole = false): number;

// src/render/buildings/cellPool.ts:237-242 method CellPool.cancel
/** A job no longer wanted: never sent if waiting, its answer dropped if begun (a worker runs a job through, one message). */
  cancel(id: number): void;

// src/render/buildings/cellPool.ts:345-351 method CellPool.onPage
/** The job done here, every height read off the page's own ground. */
  private onPage(asked: Asked): void;
