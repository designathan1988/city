// The code of src/ui/inspector.ts that is the feature's (the rest of the file is shared).

// src/ui/inspector.ts:28-70 slice of src/ui/inspector.ts#InspectorActions (shared): the feature's statements; "…" marks code left out
export interface InspectorActions {
  // … 3 lines
  readonly onSetSection?: (id: SegmentId, section: RoadSection | undefined) => void;
  // … 1 lines
  readonly onApplyProfile?: (id: SegmentId, profile: RoadProfileSpec, type?: number) => void;
  // … 4 lines
  readonly onDrawWithProfile?: (held: DrawProfile) => void;
  // … 2 lines
  readonly onSetDirection?: (id: SegmentId, direction: SegmentDirection) => void;
  // … 15 lines
  readonly onSetLaneLinks?: (node: NodeId, links: LaneLink[] | undefined) => void;
  // … 1 lines
  readonly onSetApproachRules?: (node: NodeId, rules: ApproachRuleEntry[] | undefined) => void;
  // … 1 lines
  readonly onSetSignal?: (node: NodeId, settings: SignalSettings | undefined) => void;
  readonly onSetSignals?: (settings: ReadonlyMap<NodeId, SignalSettings>) => void;
// … 6 lines
}

// src/ui/inspector.ts:72-78 slice of src/ui/inspector.ts#InspectorContext (shared): the feature's statements; "…" marks code left out
interface InspectorContext {
  // … 1 lines
  readonly net: Network;
// … 3 lines
}

// src/ui/inspector.ts:86-96 slice of src/ui/inspector.ts#openInspector (shared): the feature's statements; "…" marks code left out
export function openInspector(
  // … 1 lines
  net: Network,
// … 3 lines
): void {
  current = { doc, net, sim, selection, actions };
// … 2 lines
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

// src/ui/inspector.ts:190-208 slice of src/ui/inspector.ts#segmentStats (shared): the feature's statements; "…" marks code left out
function segmentStats(doc: RoadDoc, net: Network, sim: SimClient, id: SegmentId): string | null {
  // … 2 lines
  const rt = roadProfile(seg.type, seg.lanes, seg.direction, seg.section, seg.parking);
  const length = net.polylines.get(doc, id).length;
  // … 1 lines
  return grid([
    [t('inspector.length'), `${meters(length)} m`],
    [t('inspector.lanes'), seg.direction === 'both'
      ? t('inspector.lanesTwoWay', { count: rt.lanes, perSide: travelLanes(rt, seg.direction) })
      : t('inspector.lanesOneWay', { count: rt.lanes })],
    [t('inspector.speed'), `${Math.round(rt.speedLimit * METERS_PER_UNIT * 3.6)} km/h`],
    [t('inspector.meanSpeed'), traffic?.speedCount ? `${Math.round((traffic.speedSum / traffic.speedCount) * METERS_PER_UNIT * 3.6)} km/h` : '—'],
    [t('inspector.vehicles'), String(traffic?.vehicles ?? 0)],
    [t('inspector.queue'), String(traffic?.queued ?? 0)],
    [t('inspector.capacity'), plural('inspector.capacityValue', Math.round(traffic?.capacity ?? 0))],
    [t('inspector.volume'), String(traffic?.volume ?? 0)],
  ]);
}

// src/ui/inspector.ts:210-343 slice of src/ui/inspector.ts#renderSegment (shared): the feature's statements; "…" marks code left out
function renderSegment(
  // … 1 lines
  net: Network,
// … 4 lines
): void {
  // … 1 lines
  const stats = segmentStats(doc, net, sim, id);
  if (!seg || stats === null) {
    closeInspector();
    return;
  }
  const rt = roadProfile(seg.type, seg.lanes, seg.direction, seg.section, seg.parking);
  const length = net.polylines.get(doc, id).length;
  // … 1 lines
  const streetName = streetNameOf(net, id);
  setTitle(`${streetName ?? t('inspector.road')} · ${roadTypeName(rt)}`);
  const numbers = streetNumbers(doc, net, id);
  // … 1 lines
  const maxCurve = Math.max(20, Math.min(280, length * 0.65));
  // … 7 lines
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
  // … 1 lines
  if (actions.onApplyProfile) {
    mountProfilePanel(body.querySelector<HTMLElement>('#inspectProfile')!, seg, (profile, type) => actions.onApplyProfile?.(id, profile, type), {
      chain: actions.streetLength?.(id) ?? 0,
      ...(actions.onProfileToStreet ? { toStreet: () => actions.onProfileToStreet?.(id) } : {}),
      ...(actions.onDrawWithProfile ? { drawWith: (held: DrawProfile) => actions.onDrawWithProfile?.(held) } : {}),
    });
  }
  if (freeRoadsEnabled() && actions.onSetSection) {
    mountRoadSectionEditor(body.querySelector<HTMLElement>('#inspectSection')!, rt, seg.direction, seg.section,
      (section) => actions.onSetSection?.(id, section));
  }
  // … 15 lines
  if (direction) direction.onchange = () => actions.onSetDirection?.(id, direction.value as SegmentDirection);
// … 48 lines
}

// src/ui/inspector.ts:384-444 slice of src/ui/inspector.ts#renderNode (shared): the feature's statements; "…" marks code left out
function renderNode(
  // … 1 lines
  net: Network,
// … 3 lines
): void {
  // … 8 lines
  if (node.smooth) {
    // … 1 lines
    mountJunctionPanel(body.querySelector<HTMLElement>('#inspectJunction')!, junctionHost(doc, sim, id));
  // … 1 lines
  }
  // … 1 lines
  const mouths = node.incident
    .map((seg) => `${meters(net.mouthDistance(seg, id))} m`)
    .join(' · ');
  // … 2 lines
  const alignedLegs =
    !node.smooth && !node.crossing && node.incident.length === 2 && surfaceMode(doc, net.polylines, id) === 'none';
  const joinOffer = alignedLegs
    ? `<div class="inspect-actions"><button type="button" id="inspectJoin">${t('inspector.joinAligned')}</button></div>`
    : '';
  // … 7 lines
  const gap = net.impossible.get(id);
  const impossible = gap === undefined ? ''
    : `<div class="inspect-warning" role="alert"><strong>${t('inspector.impossible')}</strong> · ${t('inspector.impossibleGap', { angle: Math.round((gap * 180) / Math.PI) })}<br>${t('inspector.impossibleBody')}</div>`;
  // … 1 lines
  body.innerHTML = impossible + `<div id="inspectStats">${stats}</div>` + crossingOffer +
    '<div id="inspectJunction"></div>' + joinOffer +
    '<div id="inspectConnectors"></div>' + `<p class="inspect-note">${t('inspector.mouths')}: ${mouths || '—'}</p>`;
  mountJunctionPanel(body.querySelector<HTMLElement>('#inspectJunction')!, junctionHost(doc, sim, id));
  // … 1 lines
  if (connectors && actionsForNode().onSetLaneLinks) {
    mountConnectorPanel(connectors, id, {
      lanes: () => sim.asked({ kind: 'nodeLanes', node: id }) as ReturnType<ConnectorPanelHost['lanes']>,
      links: () => doc.node(id)?.laneLinks,
      set: (links) => actionsForNode().onSetLaneLinks?.(id, links),
      ...(actionsForNode().project ? { project: actionsForNode().project! } : {}),
    });
  }
// … 5 lines
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

// src/ui/inspector.ts:467-472 slice of src/ui/inspector.ts#directionOptions (shared): the feature's statements; "…" marks code left out
function directionOptions(value: SegmentDirection): string {
  const entries: readonly SegmentDirection[] = ['both', 'aToB', 'bToA'];
  return entries
    .map((id) => `<option value="${id}"${id === value ? ' selected' : ''}>${t(`direction.${id}`)}</option>`)
    .join('');
}

// src/ui/inspector.ts:474-485 slice of src/ui/inspector.ts#laneOptions (shared): the feature's statements; "…" marks code left out
function laneOptions(direction: SegmentDirection, configured: number | null, resolved: number): string {
  const values = direction === 'both' ? [2, 4, 6, 8] : [1, 2, 3, 4, 5, 6, 7, 8];
  // … 3 lines
  for (const count of values) {
    options.push(
      `<option value="${count}"${configured === count ? ' selected' : ''}>${plural('inspector.laneOption', count)}</option>`,
    );
  }
// … 1 lines
}
