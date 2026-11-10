// The code of src/ui/labels.ts that is the feature's (the rest of the file is shared).

// src/ui/labels.ts:17-17 function roadCountLabel: the feature's own
export const roadCountLabel = (count: number): string => plural('status.roads', count);

// src/ui/labels.ts:18-18 function nodeCountLabel: the feature's own
export const nodeCountLabel = (count: number): string => plural('status.nodes', count);
