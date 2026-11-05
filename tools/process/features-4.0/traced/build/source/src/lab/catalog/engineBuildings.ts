// The code of src/lab/catalog/engineBuildings.ts that is the feature's (the rest of the file is shared).

// src/lab/catalog/engineBuildings.ts:28-28 variable LOTS: the feature's own
const LOTS = [['small', m(12), m(16)], ['medium', m(24), m(26)], ['large', m(40), m(36)]] as const;

// src/lab/catalog/engineBuildings.ts:29-29 variable USES: the feature's own
const USES = ['residential', 'commercial', 'industrial'] as const;

// src/lab/catalog/engineBuildings.ts:30-30 variable DENSITIES: the feature's own
const DENSITIES = ['low', 'medium', 'high'] as const;

// src/lab/catalog/engineBuildings.ts:41-41 function engineBuildingFarItems.ground: the feature's own
  const ground = (): number => 0;
