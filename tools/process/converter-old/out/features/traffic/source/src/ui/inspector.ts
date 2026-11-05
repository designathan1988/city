// The code of src/ui/inspector.ts that is the feature's (the rest of the file is shared).

// src/ui/inspector.ts:28-70 slice of src/ui/inspector.ts#InspectorActions (shared): the feature's statements; "…" marks code left out
export interface InspectorActions {
  // … 11 lines
  readonly onSetParking?: (id: SegmentId, parking: SegmentParking) => void;
  // … 19 lines
  readonly onSetApproachRules?: (node: NodeId, rules: ApproachRuleEntry[] | undefined) => void;
  // … 1 lines
  readonly onSetSignal?: (node: NodeId, settings: SignalSettings | undefined) => void;
  readonly onSetSignals?: (settings: ReadonlyMap<NodeId, SignalSettings>) => void;
// … 6 lines
}

// src/ui/inspector.ts:103-145 slice of src/ui/inspector.ts#renderCurrent (shared): the feature's statements; "…" marks code left out
function renderCurrent(): void {
  // … 11 lines
  if (key === builtFor && stats && body.contains(stats)) {
    // … 9 lines
    if (selection.node !== null) { refreshConnectorPanel(); refreshJunctionFlows(sim, doc, selection.node); }
  // … 1 lines
  }
// … 17 lines
}

// src/ui/inspector.ts:183-187 slice of src/ui/inspector.ts#parkingSelect (shared): the feature's statements; "…" marks code left out
function parkingSelect(id: string, label: string, value: ParkingKind, rt: RoadType): string {
  const options = PARKING_KINDS.filter((kind) => kind === value || parkingAllowed(kind, rt))
    .map((kind) => `<option value="${kind}"${kind === value ? ' selected' : ''}>${t(`parking.${kind}`)}</option>`).join('');
  return `<label class="inspect-select">${t(label)} <select id="${id}">${options}</select></label>`;
}

// src/ui/inspector.ts:210-343 slice of src/ui/inspector.ts#renderSegment (shared): the feature's statements; "…" marks code left out
function renderSegment(
// … 6 lines
): void {
  // … 21 lines
  body.innerHTML =
    `<div id="inspectStats">${stats}</div>` +
    (numbers ? `<div class="rp-sub inspect-numbers">${t('inspector.numbers', { from: numbers.from, to: numbers.to })}</div>` : '') +
    (profiled ? '<div id="inspectProfile"></div>' : '') +
    (profiled ? '' : `<label class="inspect-select">${t('inspector.roadClass')} <select id="inspectClass">${ROAD_TYPES.map((type, index) => `<option value="${index}"${index === seg.type ? ' selected' : ''}>${roadTypeName(type)}</option>`).join('')}</select></label>` +
    `<label class="inspect-select">${t('inspector.direction')} <select id="inspectDirection">${directionOptions(seg.direction)}</select></label>`) +
    // The heights of its two ends as steppers of the interface (V5): no number box.
    `<div class="rp-junction">${heightStepper('A', t('inspector.heightStart'), (doc.node(seg.a)?.heightOffset ?? 0) / UNITS_PER_METER)}` +
    `${heightStepper('B', t('inspector.heightEnd'), (doc.node(seg.b)?.heightOffset ?? 0) / UNITS_PER_METER)}</div>` +
    (actions.onSetNodeEnd ? endSelect(doc, seg.a, 'A') + endSelect(doc, seg.b, 'B') : '') +
    (profiled ? '' : `<label class="inspect-select">${t('inspector.laneCount')} <select id="inspectLanes">${laneOptions(seg.direction, seg.lanes, rt.lanes)}</select></label>` +
      parkingSelect('inspectParkingLeft', 'inspector.parkingLeft', seg.parking?.left ?? 'none', rt) +
      parkingSelect('inspectParkingRight', 'inspector.parkingRight', seg.parking?.right ?? 'none', rt)) +
    `<div id="inspectSection"></div>` +
    `<label class="inspect-range"><span>${t('inspector.curvature')}</span><output id="inspectCurveValue">${curveText(curveValue)}</output><input id="inspectCurve" type="range" min="${-maxCurve}" max="${maxCurve}" step="1" value="${Math.max(-maxCurve, Math.min(maxCurve, curveValue))}" /></label>` +
    `<label class="inspect-range"${seg.curve ? '' : ' data-disabled'}><span>${t('inspector.curvePosition')}</span><output id="inspectCurvePositionValue">${percent(curvePosition)}</output><input id="inspectCurvePosition" type="range" min="0.15" max="0.85" step="0.01" value="${curvePosition}"${seg.curve ? '' : ' disabled'} /></label>` +
    `<div class="inspect-actions">` +
    `<button type="button" id="inspectUpgrade"${seg.type >= LAST_UPGRADE_CLASS ? ' disabled' : ''}>${t('inspector.upgrade')}</button>` +
    (oneWay ? `<button type="button" id="inspectReverse">${t('inspector.reverse')}</button>` : '') +
    `<button type="button" id="inspectDuplicate" title="${t('inspector.duplicateHint')}">${t('inspector.duplicate')}</button>` +
    `<button type="button" id="inspectSplit">${t('inspector.splitMiddle')}</button>` +
    `<button type="button" id="inspectZebra">${t('inspector.addZebra')}</button>` +
    `<button type="button" id="inspectSignalCrossing">${t('inspector.addSignalCrossing')}</button>` +
    (actions.onAddUturn && seg.direction === 'both' && rt.median > 0
      ? `<button type="button" data-uturn="b">${t('inspector.addUturnToEnd')}</button><button type="button" data-uturn="a">${t('inspector.addUturnToStart')}</button>` : '') +
    `<button type="button" id="inspectAddHeightPoint">${t('inspector.addHeightPoint')}</button>` +
    `<button type="button" id="inspectDelete" class="danger">${t('inspector.demolish')}</button>` +
    `</div>`;
// … 76 lines
}

// src/ui/inspector.ts:308-311 slice of src/ui/inspector.ts#renderSegment.setParking (shared): the feature's statements; "…" marks code left out
  const setParking = (): void => {
    // … 1 lines
    actions.onSetParking?.(id, { left: parkingLeft.value as ParkingKind, right: parkingRight.value as ParkingKind });
  };

// src/ui/inspector.ts:384-444 slice of src/ui/inspector.ts#renderNode (shared): the feature's statements; "…" marks code left out
function renderNode(
// … 5 lines
): void {
  // … 8 lines
  if (node.smooth) {
    // … 1 lines
    mountJunctionPanel(body.querySelector<HTMLElement>('#inspectJunction')!, junctionHost(doc, sim, id));
  // … 1 lines
  }
  // … 25 lines
  mountJunctionPanel(body.querySelector<HTMLElement>('#inspectJunction')!, junctionHost(doc, sim, id));
// … 14 lines
}

// src/ui/inspector.ts:447-458 slice of src/ui/inspector.ts#junctionHost (shared): the feature's statements; "…" marks code left out
function junctionHost(doc: RoadDoc, sim: SimClient, id: NodeId): Parameters<typeof mountJunctionPanel>[1] {
// … 10 lines
}

// src/ui/inspector.ts:450-450 method junctionHost.setHeight: the feature's own
    setHeight: (metres) => actionsForNode().onSetNodeHeight?.(id, metres),

// src/ui/inspector.ts:451-451 method junctionHost.setControl: the feature's own
    setControl: (control: JunctionControl) => actionsForNode().onSetControl?.(id, control),

// src/ui/inspector.ts:452-452 method junctionHost.setRules: the feature's own
    setRules: (rules) => actionsForNode().onSetApproachRules?.(id, rules),

// src/ui/inspector.ts:453-453 method junctionHost.setSignal: the feature's own
    setSignal: (settings) => actionsForNode().onSetSignal?.(id, settings),

// src/ui/inspector.ts:454-454 method junctionHost.setSignals: the feature's own
    setSignals: (settings) => actionsForNode().onSetSignals?.(settings),

// src/ui/inspector.ts:455-455 method junctionHost.setMovementBlocked: the feature's own
    setMovementBlocked: (from, to, blocked) => actionsForNode().onSetMovementBlocked?.(id, from, to, blocked),
