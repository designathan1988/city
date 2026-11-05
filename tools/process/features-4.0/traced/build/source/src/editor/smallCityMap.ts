// The code of src/editor/smallCityMap.ts that is the feature's (the rest of the file is shared).

// src/editor/smallCityMap.ts:52-68 slice of src/editor/smallCityMap.ts#buildCityTable (shared): the feature's statements; "…" marks code left out
export function buildCityTable(city: CityPlan, phase?: (name: string) => void): PreparedCity {
  // … 1 lines
  const shared = new SharedMembers();
  const page = new PreparedCityWriter(shared);
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
  // … 2 lines
  doc.buildings.internShared = false;
  const items = spill ? doc.buildings.spillToTable(shared ?? undefined) : null;
  // … 4 lines
  applyLots(doc, planLots(doc, net));
  // … 1 lines
  forgetLotPlans(net);
  // … 7 lines
  growLots(doc, net, items && spill ? () => spill(items, false) : null);
  if (items && spill) spill(items, true);
// … 2 lines
}

// src/editor/smallCityMap.ts:97-113 slice of src/editor/smallCityMap.ts#growLots (shared): the feature's statements; "…" marks code left out
function growLots(doc: RoadDoc, net: Network, grew: (() => void) | null): void {
  // … 5 lines
  const open = doc.openLots.count(refused);
  for (let guard = 0; guard < open * 3 + 50; guard++) {
    const id = growOnLot({ doc, net, groundAt: ground, pavedAt }, refused, GROWTH_SEED);
    if (id !== null) {
      // Stamped as the page stamps a building grown (`main.ts` `growthAnswered`), at the city's first minute.
      const fresh = doc.buildings.get(id as BuildingId)!;
      doc.buildings.put({ ...fresh, builtAt: 0, decay: 0, lotPlan: LOT_PLAN_VERSION });
      grew?.();
    } else if (doc.openLots.count(refused) === 0) break;
  }
}
