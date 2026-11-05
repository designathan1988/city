// The code of src/world/roads/connectors.ts that is the feature's (the rest of the file is shared).

// src/world/roads/connectors.ts:62-77 function toggleLaneLink: the feature's own
export function toggleLaneLink(
  current: readonly LaneLink[] | undefined, lanes: NodeLanes, link: LaneLink,
): LaneLink[] | undefined | null {
  const arrives = (l: LaneLink): boolean => lanes.arriving.some((a) => a.segment === l.from && a.lane === l.fromLane);
  const leaves = (l: LaneLink): boolean => lanes.leaving.some((a) => a.segment === l.to && a.lane === l.toLane);
  if (!arrives(link) || !leaves(link)) return null;
  const valid = (current ?? []).filter((l) => arrives(l) && leaves(l));
  const own = linksOf(valid, link.from, link.fromLane);
  const start = own.length ? own : linksOf(lanes.built, link.from, link.fromLane);
  const key = linkKey(link);
  const next = start.some((l) => linkKey(l) === key) ? start.filter((l) => linkKey(l) !== key) : [...start, link];
  if (!next.length) return null;
  const rest = valid.filter((l) => !(l.from === link.from && l.fromLane === link.fromLane));
  const all = [...rest, ...next];
  return all.length ? sortLinks(all) : undefined;
}
