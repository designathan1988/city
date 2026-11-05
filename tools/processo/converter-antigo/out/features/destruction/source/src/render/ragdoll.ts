// The code of src/render/ragdoll.ts that is the feature's (the rest of the file is shared).

// src/render/ragdoll.ts:368-399 slice of src/render/ragdoll.ts#Ragdolls (shared): the feature's statements; "…" marks code left out
export interface Ragdolls {
  // … 19 lines
  readonly decals: BloodDecal[];
// … 10 lines
}

// src/render/ragdoll.ts:401-1956 slice of src/render/ragdoll.ts#createRagdolls (shared): the feature's statements; "…" marks code left out
export function createRagdolls(exhaust: Exhaust, getUp: GetUp, gore: Gore | null = null): Ragdolls {
  // … 5 lines
  const decals: BloodDecal[] = [];
  // … 445 lines
  return {
    decals,
  // … 249 lines
  };
// … 851 lines
}

// src/render/ragdoll.ts:1028-1078 slice of src/render/ragdoll.ts#createRagdolls.update (shared): the feature's statements; "…" marks code left out
    update(dt, world) {
      // … 4 lines
      for (const d of decals) d.age += wall;
    // … 44 lines
    },
