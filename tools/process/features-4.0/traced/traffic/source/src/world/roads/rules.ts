// The code of src/world/roads/rules.ts that is the feature's (the rest of the file is shared).

// src/world/roads/rules.ts:114-118 function isHighwayClass: the feature's own
export const isHighwayClass = (doc: RoadDoc, segment: SegmentId): boolean => {
  const s = doc.segment(segment);
  const id = s ? roadType(s.type).id : '';
  return id === 'highway' || id === 'ramp';
};
