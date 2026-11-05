// The code of src/sim/invariants.ts that is the feature's (the rest of the file is shared).

// src/sim/invariants.ts:35-259 slice of src/sim/invariants.ts#runAudit (shared): the feature's statements; "…" marks code left out
export function runAudit(w: SimWorld, level: 'cheap' | 'full', signals?: readonly SignalController[]): AuditIssue[] {
  // … 172 lines
  w.pedEngine.audit(w, out);
  // … 20 lines
  for (const [crossing, state] of w.crossingStates) {
    if (!state.occupants.length) continue;
    // … 5 lines
    for (const occupant of state.occupants) {
      for (const { v, ids } of here) {
        for (const connectorId of ids) {
          const connector = w.connector(connectorId);
          if (connector?.node === node) {
            const span = w.crossingSpans.span(connector.id, crossing);
            if (span === null || (span && !pedestrianAffectsSpan(occupant, span))) continue;
            if (!reservationCoversCrossing(w, v, connector, segment, span)) continue;
            out.push(
              issue(
                'pedSignalContradiction',
                tick,
                `${occupant.id}/${v.id}`,
                `crossing ${crossing} overlaps reserved connector ${connector.id}`,
              ),
            );
          }
        }
      }
    }
  }
// … 2 lines
}
