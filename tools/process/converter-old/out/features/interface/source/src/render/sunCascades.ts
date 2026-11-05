// The code of src/render/sunCascades.ts that is the feature's (the rest of the file is shared).

// src/render/sunCascades.ts:992-995 method createSunCascades.setFarEvery: the feature's own
    setFarEvery(n) {
      farEvery = Math.max(1, Math.floor(n));
      if (farEvery === 1) carried.fill(false);
    },

// src/render/sunCascades.ts:996-1004 method createSunCascades.remakeMaps: the feature's own
    remakeMaps() {
      lights.forEach((light, i) => {
        light.shadow.map?.dispose();
        light.shadow.map = null;
        storeValid[i] = false;
        refitted[i] = true;
        everDrawn[i] = false;
      });
    },
