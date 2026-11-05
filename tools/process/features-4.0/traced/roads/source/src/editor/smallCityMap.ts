// The code of src/editor/smallCityMap.ts that is the feature's (the rest of the file is shared).

// src/editor/smallCityMap.ts:31-33 slice of src/editor/smallCityMap.ts#buildCityMap (shared): the feature's statements; "…" marks code left out
export function buildCityMap(city: CityPlan): SerializedDoc {
  return growCity(city, null, null).toJSON();
}

// src/editor/smallCityMap.ts:52-68 slice of src/editor/smallCityMap.ts#buildCityTable (shared): the feature's statements; "…" marks code left out
export function buildCityTable(city: CityPlan, phase?: (name: string) => void): PreparedCity {
  // … 3 lines
  const doc = growCity(city, shared, (items, last) => {
    if (!last && items.heldCount < SPILL_AT) return;
    page.add(items.spill(last ? 0 : SPILL_KEEP));
    shared.end();
    items.endBatch();
    page.endBatch();
    phase?.('spilled');
  }, phase);
  // … 1 lines
  const data = doc.toJSONBuildingsApart();
  // … 1 lines
  return { data, buildings: page.finish() };
}

// src/editor/smallCityMap.ts:71-94 slice of src/editor/smallCityMap.ts#growCity (shared): the feature's statements; "…" marks code left out
function growCity(city: CityPlan, shared: SharedMembers | null, spill: ((items: SpilledItems, last: boolean) => void) | null, phase?: (name: string) => void): RoadDoc {
  const doc = layCity(city);
  // … 1 lines
  doc.buildings.internShared = false;
  const items = spill ? doc.buildings.spillToTable(shared ?? undefined) : null;
  widenCityFootways(doc);
  const net = new Network(doc);
  net.rebuild();
  // … 1 lines
  applyLots(doc, planLots(doc, net));
  // … 1 lines
  forgetLotPlans(net);
  // … 1 lines
  greenCity(doc, net, (city.options.seed * 0x711e) >>> 0);
  // … 2 lines
  net.rebuild();
  zoneCity(doc, net, city);
  // … 1 lines
  growLots(doc, net, items && spill ? () => spill(items, false) : null);
  if (items && spill) spill(items, true);
  // … 1 lines
  return doc;
}

// src/editor/smallCityMap.ts:97-113 slice of src/editor/smallCityMap.ts#growLots (shared): the feature's statements; "…" marks code left out
function growLots(doc: RoadDoc, net: Network, grew: (() => void) | null): void {
  // … 1 lines
  const ground = FLAT_GROUND;
  const elevation = buildRoadElevation(net, FLAT_GROUND);
  // … 3 lines
  for (let guard = 0; guard < open * 3 + 50; guard++) {
    const id = growOnLot({ doc, net, groundAt: ground, pavedAt }, refused, GROWTH_SEED);
  // … 6 lines
  }
}

// src/editor/smallCityMap.ts:101-101 slice of src/editor/smallCityMap.ts#growLots.pavedAt (shared): the feature's statements; "…" marks code left out
  const pavedAt = (x: number, y: number): number => (elevation.has(GROUND_ONLY) ? pavedHeightFrom(elevation, GROUND_ONLY, (px, py) => footwayRiseAt(net, px, py), x, y) : NaN);
