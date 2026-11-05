// The code of src/render/buildings/cellWork.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/cellWork.ts:115-115 slice of src/render/buildings/cellWork.ts#COARSE (shared): the feature's statements; "…" marks code left out
const COARSE: Omit<CellSelection, 'ghost' | 'dim'> = { parts: PART_KINDS.filter((k) => COARSE_PARTS.has(k)), shells: true, furniture: true };

// src/render/buildings/cellWork.ts:116-116 slice of src/render/buildings/cellWork.ts#DETAIL (shared): the feature's statements; "…" marks code left out
const DETAIL: Omit<CellSelection, 'ghost' | 'dim'> = { parts: PART_KINDS.filter((k) => DETAIL_PARTS.has(k)), shells: false, furniture: false };

// src/render/buildings/cellWork.ts:119-227 slice of src/render/buildings/cellWork.ts#runCellJob (shared): the feature's statements; "…" marks code left out
export function runCellJob(job: CellJob, reads: CellReads): WorkReply {
  // … 70 lines
  if (job.kind === 'summary') {
    const floorsOf = new PartyWalls(buildings, naturalAt, paved, known);
    const summaries = buildings.map((b, i) => summaryOf(chunkSummary(b, groundAt, paved, naturalAt, floorsOf), i));
    return missed() ? wider(job, missX0, missY0, missX1, missY1) : { kind: 'done', job: job.id, result: { batch: null, parts: null, summaries, floors: floorsOf.floorsKnown() } };
  }
  // … 1 lines
  const party = new PartyWalls(buildings, naturalAt, paved, known);
  // … 2 lines
  const chunks = buildings.map((b) => {
    const chunk = emitChunk(b, groundAt, paved, naturalAt, undefined, job.kind === 'facade', true, party, writer);
    // … 2 lines
    return chunk;
  });
  // … 21 lines
  return { kind: 'done', job: job.id, result: { batch, parts, summaries, floors: party.floorsKnown() } };
}
