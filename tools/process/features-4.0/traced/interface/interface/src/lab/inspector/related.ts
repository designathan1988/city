// What the feature uses of src/lab/inspector/related.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/inspector/related.ts:8-22 variable LAYER_BY_NAME
/** WHAT ELSE TOUCHES THE SELECTION: the effects panel's switches that hide it (by the layer groups `renderer.ts` `switchedOff` names and the buildings' parts and pieces, `buildingSwitches.ts`), the lab's optimisations of its system, its draws in the GPU tab's last capture, its memory, and the code that makes it (the dev server's source search, `scripts/vite/labSource.ts`). */

/** Each layer switch by the names of the groups it hides (`renderer.ts` `switchedOff`, the scene as built: `world`, `agent-parts`, `buildings-layer`...). */
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

// src/lab/inspector/related.ts:25-31 variable BUILDING_ROOTS
/** The buildings layer's roots (`BuildingLayer.roots`, named in `buildings/layer.ts`) and the switch of each. */
const BUILDING_ROOTS: readonly (readonly [RegExp, (typeof BUILDING_SWITCHES)[number]])[] = [
  [/^buildings-far$/, 'bMasses'],
  [/^signature-buildings$/, 'bSignatures'],
  [/^building-part-cells$|^building-parts$/, 'bParts'],
  [/^flags$/, 'bFlags'],
  [/^buildings$/, 'bDetail'],
];

// src/lab/inspector/related.ts:34-43 function buildingKindSwitch
/** A piece of a building by its mesh's name (`buildingSwitches.ts` KIND_SWITCH). */
function buildingKindSwitch(name: string): (typeof BUILDING_SWITCHES)[number] | null;
