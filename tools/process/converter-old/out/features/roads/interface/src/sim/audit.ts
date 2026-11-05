// What the feature uses of src/sim/audit.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/audit.ts:2-27 type AuditCode
/** Invariant checker. */
export type AuditCode =
  // vehicles
  | 'greenBlocked'
  /** Held at a green past what any single interaction explains. */
  | 'greenHeld'
  | 'redEntry'
  | 'overlap'
  | 'nonMonotoneS'
  | 'routeless'
  | 'spillbackWedge'
  // claims
  | 'staleClaim'
  | 'orphanClaim'
  // signals
  | 'allRedTooLong'
  | 'groupStarved'
  | 'planCoverageGap'
  | 'degradedPlan'
  // pedestrians
  | 'pedFrozen'
  | 'pedInRoadStalled'
  | 'pedOutsideSidewalk'
  | 'pedSignalContradiction'
  // geometry
  | 'shortLink'
  | 'degenerateGeometry';

// src/sim/audit.ts:29-34 interface AuditIssue
export interface AuditIssue {
  readonly code: AuditCode;
  readonly tick: number;
  readonly subject: string;
  readonly detail: string;
}

// src/sim/audit.ts:36-41 function issue
export const issue = (
  code: AuditCode,
  tick: number,
  subject: string | number,
  detail: string,
): AuditIssue => …;
