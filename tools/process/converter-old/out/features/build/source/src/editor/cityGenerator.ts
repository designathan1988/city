// The code of src/editor/cityGenerator.ts that is the feature's (the rest of the file is shared).

// src/editor/cityGenerator.ts:46-62 slice of src/editor/cityGenerator.ts#greenCity (shared): the feature's statements; "…" marks code left out
export function greenCity(doc: RoadDoc, net: Network, seed: number): { squares: number; pieces: number } {
  // … 1 lines
  for (const square of squares) {
    for (const id of square.lots) deleteLot(doc, id);
  // … 2 lines
  }
// … 9 lines
}

// src/editor/cityGenerator.ts:65-95 slice of src/editor/cityGenerator.ts#zoneCity (shared): the feature's statements; "…" marks code left out
export function zoneCity(doc: RoadDoc, net: Network, plan: CityPlan): number {
  // … 14 lines
  for (const lot of doc.lots as readonly Lot[]) {
    const anchor = lotFrame(lot).anchor;
    const zone = plan.zoneAt(anchor, nearAny(anchor, mainsNear(anchor), reach));
    if (zone === 'park') continue;
    const key = `${zone.use}:${zone.density}`;
    let list = groups.get(key);
    if (!list) groups.set(key, list = []);
    list.push(lot.id);
  }
  // … 1 lines
  for (const [key, ids] of groups) {
    const [use, density] = key.split(':') as [Lot['use'] & string, Lot['density'] & string];
    if (zoneLots(doc, ids, { use, density })) zoned += ids.length;
  }
  return zoned;
}
