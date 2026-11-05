// The code of src/lab/inspector/related.ts that is the feature's (the rest of the file is shared).

// src/lab/inspector/related.ts:8-22 slice of src/lab/inspector/related.ts#LAYER_BY_NAME (shared): the feature's statements; "…" marks code left out
const LAYER_BY_NAME: readonly (readonly [RegExp, (typeof LAYER_SWITCHES)[number]])[] = [
  [/^terrain-water|water/, 'water'],
  [/^terrain-|^flat-world/, 'terrain'],
  [/^road-network$|^road-structure-details$|^road-stand-ins$/, 'roads'],
  [/^buildings-layer$/, 'buildings'],
  [/^ground-cover$|^nature-cards$|^nature-forest$|^painted-forest$|^planted-forest$|^garden-|^far-forest/, 'vegetation'],
  [/^street-furniture$|^grass$/, 'furniture'],
  [/^signs$|^barriers$|^utilities$|^elements$/, 'props'],
  [/^transit$/, 'transit'],
  [/^agent-parts$/, 'people'],
  [/^densityCars$/, 'farTraffic'],
  [/^traffic-signals$/, 'signals'],
  [/^exhaust$/, 'particles'],
  [/^rain$|^lightning$/, 'weather'],
];

// src/lab/inspector/related.ts:25-31 slice of src/lab/inspector/related.ts#BUILDING_ROOTS (shared): the feature's statements; "…" marks code left out
const BUILDING_ROOTS: readonly (readonly [RegExp, (typeof BUILDING_SWITCHES)[number]])[] = [
  [/^buildings-far$/, 'bMasses'],
  [/^signature-buildings$/, 'bSignatures'],
  [/^building-part-cells$|^building-parts$/, 'bParts'],
  [/^flags$/, 'bFlags'],
  [/^buildings$/, 'bDetail'],
];

// src/lab/inspector/related.ts:34-43 slice of src/lab/inspector/related.ts#buildingKindSwitch (shared): the feature's statements; "…" marks code left out
function buildingKindSwitch(name: string): (typeof BUILDING_SWITCHES)[number] | null {
// … 8 lines
}
