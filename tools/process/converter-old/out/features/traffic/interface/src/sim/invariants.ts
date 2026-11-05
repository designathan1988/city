// What the feature uses of src/sim/invariants.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/invariants.ts:14-26 interface AuditMemory
/** Runtime invariant checks. */
/** What a world's audits keep from one to the next (`runAudit`) */
interface AuditMemory {
  /** The audits run so far. */
  pass: number;
  /** The signals' log read so far (`SimWorld.signalChangesSince`). */
  serial: number;
  /** When demand was first seen waiting at each group's red, per controller (`groupStarved`), and the audit that last looked at it. */
  readonly onsets: WeakMap<SignalController, { audited: number; readonly since: Map<number, number> }>;
  /** Each junction's plan cycle, and the longest with how many have it (`maxCycle`); `stale`: the last of the longest went, found again when next asked. */
  readonly cycles: Map<NodeId, number>;
  longest: number;
  longestCount: number;
  stale: boolean;
}

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

// src/sim/invariants.ts:287-297 function auditedSignals
/** The signals an audit looks at: every one, or those given and those planned again since the last audit (`planned`, null: any may have been). */
function auditedSignals(w: SimWorld, signals: readonly SignalController[] | undefined, planned: readonly NodeId[] | null): Iterable<SignalController>;

// src/sim/invariants.ts:300-302 function everySignal
/** @rebuild Every signal: the audit of everything (doing everything, the specs), or the first after the signals' log was forgotten. */
function everySignal(w: SimWorld): Iterable<SignalController>;

// src/sim/invariants.ts:305-311 function onsetsOf
/** A signal's demand onsets for this audit: forgotten when the audit before did not look at it (asleep then, no demand waited at it) */
function onsetsOf(memory: AuditMemory, c: SignalController, pass: number): Map<number, number>;

// src/sim/invariants.ts:314-327 function followSignals
/** The plans' cycles brought up to the signals' log: the junctions planned again since the last audit, null when it cannot say which. */
function followSignals(w: SimWorld, memory: AuditMemory): readonly NodeId[] | null;

// src/sim/invariants.ts:330-336 function refileCycles
/** @rebuild Every junction's cycle filed again: the first audit of a world, or after the signals' log was forgotten. */
function refileCycles(w: SimWorld, memory: AuditMemory): void;

// src/sim/invariants.ts:338-341 function keepCycle
function keepCycle(memory: AuditMemory, node: NodeId, cycle: number): void;

// src/sim/invariants.ts:356-361 function maxSignalCycle
function maxSignalCycle(c: SignalController): number;
