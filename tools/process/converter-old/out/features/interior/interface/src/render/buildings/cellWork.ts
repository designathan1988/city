// What the feature uses of src/render/buildings/cellWork.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/cellWork.ts:25-25 type JobKind
/** What a job makes: a cell's detail (its coarse batch and its detail cells'), the same at the far level (`facade`: L1, `emitChunk` `far`), a cell's buildings faded */
/** 'massTile' and 'massPlate': the masses of a 1,024 m tile (L2) and a 2,048 m plate (L3) merged, simplified to their level (`massLevels.ts`); 'massBase': a plate's square at the top level, its blocks' prisms (`massBase.ts`). */
export type JobKind = 'cell' | 'mass' | 'summary' | 'facade' | 'massTile' | 'massPlate' | 'massBase';

// src/render/buildings/cellWork.ts:30-49 interface CellJob
export interface CellJob {
  readonly id: number;
  readonly kind: JobKind;
  /** The coarse cell the entries stand in (`BATCH_CELL`). */
  readonly cell: string;
  readonly entries: readonly CellJobEntry[];
  /** Whether the page has paving to read. */
  readonly paved: boolean;
  /** The roads near the cell, to read the paving here instead of asking the page (`world/paving.ts`). */
  readonly paving?: PavingData;
  /** The ground in shared memory, read here as the page reads it (`sharedGround.ts`); none when the job is done on the page. */
  readonly ground?: SharedGroundData;
  /** Where the ground was cut to its plates around the job (`groundAround`: a job for another process); a read past them asks the job again (`wider`). */
  readonly around?: readonly [number, number, number, number];
  /** The page reads the natural ground as the drawn one (`layer.ts` `update`, `naturalAt` left out). */
  readonly naturalIsGround?: boolean;
  /** The kit's part boxes and each finish's colour from afar (`kit.ts` `finishTone`): the job carries all it reads. */
  readonly boxes: KitBoxes;
  readonly tone?: Readonly<Record<Finish, readonly [number, number, number]>>;
}

// src/render/buildings/cellWork.ts:52-57 interface ChunkSummary
/** What the page keeps of a building emitted: its flags, how its lots' ground is made again, its signature floor. */
export interface ChunkSummary {
  readonly key: string;
  readonly flags?: readonly FlagInstance[];
  readonly lot: LotGroundRecipe | null;
  readonly signatureFloor?: number;
}

// src/render/buildings/cellWork.ts:59-72 interface CellResult
export interface CellResult {
  /** The coarse batch ('cell'); none for a summary or masses. */
  readonly batch: CellArrays | null;
  /** The cell's parts by band and detail cell, filed here (`partFiling.ts`): null for a summary or masses, or none. */
  readonly parts: PartFiling | null;
  /** The buildings this job emitted whole. */
  readonly summaries: readonly ChunkSummary[];
  /** The cell's masses ('mass'): null when none of its buildings has any. */
  readonly mass?: CellMass | null;
  /** A square's base ('massBase'): null with no building. */
  readonly base?: BaseMass | null;
  /** Each entry's ground floor as the job stood it (`CellEntry.floor`): null with no closed block, undefined where it read none; kept by the page for the next jobs of the same buildings (`cellPool.ts`). */
  readonly floors?: readonly (number | null | undefined)[];
}

// src/render/buildings/cellWork.ts:74-78 type WorkReply
export type WorkReply =
  /** The paving was read past the roads handed over, or the ground past the plates (`CellJob.around`): the job is asked again with them over `rect` (rare: a building reaching far out of its cell). */
  | { readonly kind: 'wider'; readonly job: number; readonly rect: readonly [number, number, number, number] }
  | { readonly kind: 'done'; readonly job: number; readonly result: CellResult }
  | { readonly kind: 'error'; readonly job: number; readonly message: string };

// src/render/buildings/cellWork.ts:91-97 interface CellReads
/** What a job reads: the ground (shared, or the page's own), and the paving, undefined where it cannot be read here. */
interface CellReads extends GroundReaders {
  readonly pavedAt?: (x: number, y: number) => number | undefined;
  /** The worker's own shell and rows (`ShellWriter`): DO NOT make them for each job, their arrays grown again from nothing. */
  readonly writer?: ShellWriter;
  /** The box of the reads on plates the job was not handed (`CellJob.around`); null when none. */
  readonly groundMissed?: () => readonly [number, number, number, number] | null;
}

// src/render/buildings/cellWork.ts:115-115 variable COARSE
const COARSE: Omit<CellSelection, 'ghost' | 'dim'> = { parts: PART_KINDS.filter((k) => COARSE_PARTS.has(k)), shells: true, furniture: true };

// src/render/buildings/cellWork.ts:116-116 variable DETAIL
const DETAIL: Omit<CellSelection, 'ghost' | 'dim'> = { parts: PART_KINDS.filter((k) => DETAIL_PARTS.has(k)), shells: false, furniture: false };

// src/render/buildings/cellWork.ts:119-227 function runCellJob
/** DO NOT ask the page for the ground in rounds: a cell's job reads every height itself (`sharedGround.ts`), in one pass, nothing kept once it is done */
export function runCellJob(job: CellJob, reads: CellReads): WorkReply;

// src/render/buildings/cellWork.ts:230-236 function wider
/** The roads and the ground to hand the job again: those it was handed and every point it read past them, a metre round. */
function wider(job: CellJob, x0: number, y0: number, x1: number, y1: number): WorkReply;
