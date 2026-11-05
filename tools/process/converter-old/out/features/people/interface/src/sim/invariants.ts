// What the feature uses of src/sim/invariants.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/invariants.ts:35-259 function runAudit
/** DO NOT walk every signal of the map each audit: `signals` (those stepped since, `pipeline.ts`) and those planned again are looked at; without it, every one */
export function runAudit(w: SimWorld, level: 'cheap' | 'full', signals?: readonly SignalController[]): AuditIssue[];
