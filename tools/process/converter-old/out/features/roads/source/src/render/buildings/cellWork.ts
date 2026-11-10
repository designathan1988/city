// The code of src/render/buildings/cellWork.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/cellWork.ts:30-49 slice of src/render/buildings/cellWork.ts#CellJob (shared): the feature's statements; "…" marks code left out
export interface CellJob {
  // … 8 lines
  readonly paving?: PavingData;
// … 9 lines
}

// src/render/buildings/cellWork.ts:100-113 slice of src/render/buildings/cellWork.ts#workerReads (shared): the feature's statements; "…" marks code left out
export function workerReads(job: CellJob, writer?: ShellWriter): CellReads {
  // … 9 lines
  const pavedAt = job.paving ? pavingReader(job.paving, naturalAt) : undefined;
  // … 1 lines
  return { groundAt: shared.groundAt, naturalAt, groundMissed, ...(pavedAt ? { pavedAt } : {}), ...(writer ? { writer } : {}) };
}

// src/render/buildings/cellWork.ts:230-236 slice of src/render/buildings/cellWork.ts#wider (shared): the feature's statements; "…" marks code left out
function wider(job: CellJob, x0: number, y0: number, x1: number, y1: number): WorkReply {
  // … 1 lines
  for (const was of [job.paving?.rect, job.around]) {
    if (was) { rect[0] = Math.min(rect[0], was[0]); rect[1] = Math.min(rect[1], was[1]); rect[2] = Math.max(rect[2], was[2]); rect[3] = Math.max(rect[3], was[3]); }
  }
// … 1 lines
}
