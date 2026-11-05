// The code of src/world/roadSection.ts that is the feature's (the rest of the file is shared).

// src/world/roadSection.ts:267-270 function laneLineCrossable: the feature's own
export function laneLineCrossable(section: RoadSection | undefined, forward: boolean, index: number): boolean {
  const line = (forward ? section?.linesForward : section?.linesBackward)?.[index] ?? 'dashed';
  return line === 'dashed' && laneUse(section, forward, index) === laneUse(section, forward, index + 1);
}
