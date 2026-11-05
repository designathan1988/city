// The code of src/render/people/crowdAnimation.ts that is the feature's (the rest of the file is shared).

// src/render/people/crowdAnimation.ts:335-337 method createPalettePass.setAtlas: the feature's own
    setAtlas(atlas) {
      uniforms.uAtlas.value = atlas as DataTexture;
    },

// src/render/people/crowdAnimation.ts:338-346 method createPalettePass.dispose: the feature's own
    dispose() {
      freeSlab(slab);
      blendTarget?.dispose();
      playTexture.dispose();
      overrideTexture.dispose();
      material.dispose();
      blendMaterial?.dispose();
      quad.dispose();
    },
