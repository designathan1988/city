// What the feature uses of src/sim/agents/lotNav.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lotNav.ts:36-36 type LotPart
/** What a lot's grid reads of a part of the lot (`buildLotGrid`): all a worker is sent of it (`lotBays.ts` `shareOf`). */
export type LotPart = Pick<BuildingElement, 'kind' | 'x' | 'y' | 'w' | 'd' | 'facing' | 'angle' | 'z'>;
