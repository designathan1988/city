// The code of src/ui/labels.ts that is the feature's (the rest of the file is shared).

// src/ui/labels.ts:6-6 slice of src/ui/labels.ts#roadTypeName (shared): the feature's statements; "…" marks code left out
export const roadTypeName = (type: RoadType): string => t(type.nameKey);

// src/ui/labels.ts:9-15 slice of src/ui/labels.ts#roadTypeDescription (shared): the feature's statements; "…" marks code left out
export function roadTypeDescription(type: RoadType): string {
  if (type.subLanes === null) return t(type.subKey);
  return t(type.subKey, {
    count: type.subLanes,
    way: t(type.subOneWay ? 'road.way.one' : 'road.way.two'),
  });
}
