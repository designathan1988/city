// The code of src/render/people/crowdAnimation.ts that is the feature's (the rest of the file is shared).

// src/render/people/crowdAnimation.ts:68-68 method buildClassAnimation.rowOf: the feature's own
    atlas, rowOf: (clip) => start.get(clip) ?? 0, bones, bytes: width * height * 16,

// src/render/people/crowdAnimation.ts:69-71 method buildClassAnimation.refill: the feature's own
    refill() {
      if (gpuOnly) atlas.needsUpdate = true;
    },

// src/render/people/crowdAnimation.ts:252-265 method createPalettePass.setPlays: the feature's own
    setPlays(row, list, variant) {
      const o = row * PLAY_TEXELS * 4;
      for (let k = 0; k < PLAYS; k++) {
        const p = list?.[k];
        plays[o + k * 4] = p ? p.row : 0;
        plays[o + k * 4 + 1] = p ? p.t : 0;
        plays[o + k * 4 + 2] = p ? p.weight : 0;
        plays[o + k * 4 + 3] = 0;
      }
      plays[o + PLAYS * 4] = variant;
      plays[o + PLAYS * 4 + 1] = list ? 0 : 1;
      plays[o + PLAYS * 4 + 2] = 0;
      dirty.add(row);
    },

// src/render/people/crowdAnimation.ts:266-271 method createPalettePass.rest: the feature's own
    rest(row) {
      const o = row * PLAY_TEXELS * 4 + PLAYS * 4 + 2;
      if (plays[o] === 1) return;
      plays[o] = 1;
      dirty.add(row);
    },

// src/render/people/crowdAnimation.ts:272-275 method createPalettePass.setOverride: the feature's own
    setOverride(row, palette) {
      override.set(palette.subarray(0, width * 4), row * width * 4);
      overrideRows.add(row);
    },

// src/render/people/crowdAnimation.ts:276-311 method createPalettePass.render: the feature's own
    render(renderer, used) {
      const n = Math.min(rows, Math.max(0, used));
      if (n === 0) return;
      uniforms.uJoints.value = joints();
      if (dirtyAll || dirty.size) {
        // Each update range is one row of the image in three (WebGLTextures.js)
        playTexture.clearUpdateRanges();
        if (!dirtyAll && dirty.size <= MOST_ROW_RANGES) for (const row of dirty) playTexture.addUpdateRange(row * PLAY_TEXELS * 4, PLAY_TEXELS * 4);
        playTexture.needsUpdate = true;
        dirty.clear();
        dirtyAll = false;
      }
      if (overrideRows.size) {
        overrideTexture.clearUpdateRanges();
        // A row a call for a few; past them the whole texture in one (AGT-06), as the plays above.
        if (overrideRows.size <= MOST_ROW_RANGES) for (const row of overrideRows) overrideTexture.addUpdateRange(row * width * 4, width * 4);
        overrideTexture.needsUpdate = true;
        overrideRows.clear();
      }
      const previous = renderer.getRenderTarget();
      const autoClear = renderer.autoClear;
      renderer.autoClear = false;
      if (blendScene && blendTarget) {
        blendTarget.viewport.set(0, 0, width, n);
        renderer.setRenderTarget(blendTarget);
        renderer.render(blendScene, camera);
      }
      // This pass's rows of the shared target, nothing else of it touched (no clear).
      const target = slab.page.target;
      uniforms.uBase.value = slab.base;
      target.viewport.set(0, slab.base, width, n);
      renderer.setRenderTarget(target);
      renderer.render(scene, camera);
      renderer.setRenderTarget(previous);
      renderer.autoClear = autoClear;
    },

// src/render/people/crowdAnimation.ts:312-334 method createPalettePass.grow: the feature's own
    grow(capacity) {
      if (capacity <= rows) return;
      const nextPlays = new Float32Array(PLAY_TEXELS * 4 * capacity);
      nextPlays.set(plays);
      plays = nextPlays;
      playTexture.dispose();
      playTexture = rowsTexture(plays, PLAY_TEXELS, capacity);
      const nextOverride = new Float32Array(width * 4 * capacity);
      nextOverride.set(override);
      override = nextOverride;
      overrideTexture.dispose();
      overrideTexture = rowsTexture(override, width, capacity);
      growSlab(slab, capacity);
      if (blendTarget) {
        blendTarget.dispose();
        blendTarget = makeTarget(width, capacity);
        uniforms.uBlend.value = blendTarget.texture;
      }
      uniforms.uPlays.value = playTexture;
      uniforms.uOverride.value = overrideTexture;
      rows = capacity;
      dirtyAll = true;
    },

// src/render/people/crowdAnimation.ts:335-337 method createPalettePass.setAtlas: the feature's own
    setAtlas(atlas) {
      uniforms.uAtlas.value = atlas as DataTexture;
    },
