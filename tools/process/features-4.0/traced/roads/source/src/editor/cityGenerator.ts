// The code of src/editor/cityGenerator.ts that is the feature's (the rest of the file is shared).

// src/editor/cityGenerator.ts:19-28 slice of src/editor/cityGenerator.ts#layCity (shared): the feature's statements; "…" marks code left out
export function layCity(plan: CityPlan): RoadDoc {
  // … 1 lines
  const types = LEVEL_ROAD.map((id) => Math.max(0, ROAD_TYPES.findIndex((t) => t.id === id)));
  const ids = plan.graph.nodes.map((p) => doc.addNode(p).id);
  for (const e of plan.graph.edges) {
    const chain = [ids[e.a]!, ...e.points.map((p) => doc.addNode(p).id), ids[e.b]!];
    for (let i = 0; i + 1 < chain.length; i++) doc.addSegment(chain[i]!, chain[i + 1]!, types[Math.min(e.level, types.length - 1)]!);
  }
// … 1 lines
}

// src/editor/cityGenerator.ts:34-43 slice of src/editor/cityGenerator.ts#widenCityFootways (shared): the feature's statements; "…" marks code left out
export function widenCityFootways(doc: RoadDoc): void {
  for (const s of [...doc.segments.values()]) {
    const rt = roadProfile(s.type, s.lanes, s.direction, s.section, s.parking);
    if (rt.id === 'highway' || rt.id === 'ramp') continue;
    // … 1 lines
    const median = rt.id === 'avenue' && s.direction === 'both' && rt.median < CITY_MEDIAN ? CITY_MEDIAN : rt.median;
    if (rt.sidewalk >= CITY_FOOTWAY && median === rt.median) continue;
    doc.setSegmentSection(s.id, { ...sectionFromProfile(rt), sidewalk: Math.max(rt.sidewalk, CITY_FOOTWAY), median });
  }
}

// src/editor/cityGenerator.ts:46-62 slice of src/editor/cityGenerator.ts#greenCity (shared): the feature's statements; "…" marks code left out
export function greenCity(doc: RoadDoc, net: Network, seed: number): { squares: number; pieces: number } {
  // … 6 lines
  if (net.revision !== doc.revision) net.rebuild();
  // … 1 lines
  const power = layPowerLines(doc, net, doc.segments.keys()) > 0;
  const pieces = furnitureFor(net, doc.segments.keys(), 'complete', doc.landscape.values(), power, false);
  // … 1 lines
  doc.addLandscapeMany(pieces.map((piece) => ({ kind: piece.kind, at: piece.at })));
  // … 1 lines
  doc.addLandscapeMany(nameFor(net, doc.segments.keys()).map((name) => ({ kind: 'streetname' as const, at: name.at, extra: { text: name.text } })));
  return { squares: squares.length, pieces: pieces.length };
}

// src/editor/cityGenerator.ts:65-95 slice of src/editor/cityGenerator.ts#zoneCity (shared): the feature's statements; "…" marks code left out
export function zoneCity(doc: RoadDoc, net: Network, plan: CityPlan): number {
  const main = new Set(['avenue', 'urban'].map((id) => ROAD_TYPES.findIndex((t) => t.id === id)));
  // … 13 lines
  for (const lot of doc.lots as readonly Lot[]) {
    // … 1 lines
    const zone = plan.zoneAt(anchor, nearAny(anchor, mainsNear(anchor), reach));
    if (zone === 'park') continue;
    const key = `${zone.use}:${zone.density}`;
    let list = groups.get(key);
    if (!list) groups.set(key, list = []);
    list.push(lot.id);
  }
// … 6 lines
}

// src/editor/cityGenerator.ts:69-78 slice of src/editor/cityGenerator.ts#zoneCity.mainsNear (shared): the feature's statements; "…" marks code left out
  const mainsNear = (p: Vec2): [Vec2, Vec2][] => {
    // … 1 lines
    for (const { id } of ribbonsNear(net, [p.x - reach, p.y - reach, p.x + reach, p.y + reach])) {
      const s = doc.segment(id);
      if (!s || !main.has(s.type)) continue;
      const a = doc.nodes.get(s.a), b = doc.nodes.get(s.b);
      if (a && b) out.push([a, b]);
    }
  // … 1 lines
  };
