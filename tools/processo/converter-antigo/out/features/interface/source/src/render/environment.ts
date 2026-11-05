// The code of src/render/environment.ts that is the feature's (the rest of the file is shared).

// src/render/environment.ts:295-295 method createEnvironment.setFarCascadeEvery: the feature's own
    setFarCascadeEvery: (n) => cascades.setFarEvery(n),

// src/render/environment.ts:298-298 method createEnvironment.remakeShadowMaps: the feature's own
    remakeShadowMaps: () => cascades.remakeMaps(),
