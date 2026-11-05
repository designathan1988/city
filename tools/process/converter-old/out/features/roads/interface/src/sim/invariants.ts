// What the feature uses of src/sim/invariants.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/invariants.ts:30-30 variable CYCLE_FLOOR
/** The floor and the ceiling of the longest cycle the audit reads (`maxCycle`), s; a wedge is twice it, so never under twice the floor. */
const CYCLE_FLOOR = 30;

// src/sim/invariants.ts:32-32 variable MIN_WEDGE
const MIN_WEDGE = 2 * CYCLE_FLOOR;

// src/sim/invariants.ts:35-259 function runAudit
/** DO NOT walk every signal of the map each audit: `signals` (those stepped since, `pipeline.ts`) and those planned again are looked at; without it, every one */
export function runAudit(w: SimWorld, level: 'cheap' | 'full', signals?: readonly SignalController[]): AuditIssue[];

// src/sim/invariants.ts:262-267 function junctionIsStuck
/** True when a junction has admitted nobody for longer than `limit` seconds. */
function junctionIsStuck(w: SimWorld, node: number | undefined, limit: number): boolean;

// src/sim/invariants.ts:356-361 function maxSignalCycle
function maxSignalCycle(c: SignalController): number;
