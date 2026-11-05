// The code of src/render/fogLayer.ts that is the feature's (the rest of the file is shared).

// src/render/fogLayer.ts:52-204 slice of src/render/fogLayer.ts#createFogField (shared): the feature's statements; "…" marks code left out
export function createFogField(texture: DataTexture): FogField {
  // … 1 lines
  const view = new TorusWindow(FOG_RES, FOG_CELL);
// … 149 lines
}

// src/render/fogLayer.ts:97-167 slice of src/render/fogLayer.ts#createFogField.raster (shared): the feature's statements; "…" marks code left out
  const raster = (dabs: readonly FogDab[], groundAt: (x: number, y: number) => number, [gx0, gx1, gy0, gy1]: TexelBand): void => {
    const x0 = (gx0 + 0.5) * cell, x1 = (gx1 + 0.5) * cell;
    reach.length = 0;
    for (const d of dabs) if (d.x + d.radius > x0 && d.x - d.radius < x1) reach.push(d);
    const width = gx1 - gx0 + 1, first = view.texel(gx0);
    // A band over more than a quarter of the rows (the view moved along x: a strip of every row) sent whole, in one call, never a call a row (TER-14).
    if (!sendWhole && uploaded && gy1 - gy0 + 1 > FOG_RES / 4) {
      sendWhole = true;
      texture.clearUpdateRanges();
    }
    for (let gy = gy0; gy <= gy1; gy++) {
      const y = (gy + 0.5) * cell;
      const j = view.texel(gy);
      row.length = 0;
      for (const d of reach) if (Math.abs(d.y - y) < d.radius) row.push(d);
      let low = Infinity, high = -Infinity;
      for (let gx = gx0; gx <= gx1; gx++) {
        const x = (gx + 0.5) * cell;
        const k = (j * FOG_RES + view.texel(gx)) * 4;
        near.length = 0;
        for (const d of row) if (Math.abs(d.x - x) < d.radius) near.push(d);
        if (near.length) fogAt(near, x, y, sample); else sample.density = 0;
        const density = sample.density;
        // The ground everywhere a little fog may spread to by filtering.
        const ground = groundAt(x, y);
        data[k] = density > 0.002 ? DataUtils.toHalfFloat(density) : zero;
        data[k + 1] = DataUtils.toHalfFloat(ground);
        data[k + 2] = density > 0.002 ? DataUtils.toHalfFloat(sample.height) : zero;
        data[k + 3] = density > 0.002 ? DataUtils.toHalfFloat(sample.speed) : zero;
        if (density > 0.002) {
          low = Math.min(low, ground);
          // Thin to under a twentieth three heights up.
          high = Math.max(high, ground + sample.height * 3);
        }
      }
      // Part of the row: its slab from every other texel of it with fog, read back from the texture: a block none of whose texels were
      // written from its slab kept, a block written in part from its texels outside the band (TER-14: never the whole row decoded).
      for (let b = 0; b < FOG_BLOCKS; b++) {
        const i0 = b * FOG_BLOCK, at = j * FOG_BLOCKS + b;
        let inside = 0;
        for (let i = i0; i < i0 + FOG_BLOCK; i++) if ((i - first + FOG_RES) % FOG_RES < width) inside++;
        // Written: read back again when next read.
        if (inside > 0) blockStale[at] = 1;
        if (width < FOG_RES) {
          if (inside === FOG_BLOCK) continue;
          if (inside === 0) {
            if (blockStale[at]) {
              slab.low = Infinity; slab.high = -Infinity;
              readBack(j, i0, i0 + FOG_BLOCK);
              blockLow[at] = slab.low; blockHigh[at] = slab.high; blockStale[at] = 0;
            }
            low = Math.min(low, blockLow[at]!);
            high = Math.max(high, blockHigh[at]!);
            continue;
          }
          slab.low = low; slab.high = high;
          for (let i = i0; i < i0 + FOG_BLOCK; i++) if ((i - first + FOG_RES) % FOG_RES >= width) readBack(j, i, i + 1);
          low = slab.low; high = slab.high;
        }
      }
      rowLow[j] = low;
      rowHigh[j] = high;
      // Only the band's columns of the row, in two where it wraps round the texture.
      if (uploaded && !sendWhole) {
        const run = Math.min(width, FOG_RES - first);
        texture.addUpdateRange((j * FOG_RES + first) * 4, run * 4);
        if (run < width) texture.addUpdateRange(j * FOG_RES * 4, (width - run) * 4);
      }
    }
    texture.needsUpdate = true;
  };

// src/render/fogLayer.ts:168-173 slice of src/render/fogLayer.ts#createFogField.layer (shared): the feature's statements; "…" marks code left out
  const layer = (): FogLayer => {
    // … 3 lines
    return { texture, low: any ? low : 0, high: any ? high : 0, any, window: [view.ox * cell, view.oy * cell, view.span] };
  };

// src/render/fogLayer.ts:174-182 slice of src/render/fogLayer.ts#createFogField.whole (shared): the feature's statements; "…" marks code left out
  const whole = (dabs: readonly FogDab[], groundAt: (x: number, y: number) => number): FogLayer => {
    // … 2 lines
    raster(dabs, groundAt, view.whole);
  // … 4 lines
  };

// src/render/fogLayer.ts:185-194 slice of src/render/fogLayer.ts#createFogField.update (shared): the feature's statements; "…" marks code left out
    update(dabs, groundAt, rects) {
      // … 3 lines
      for (const r of rects) {
        const band = view.bandOf(r);
        if (band) raster(dabs, groundAt, band);
      }
    // … 1 lines
    },

// src/render/fogLayer.ts:195-202 slice of src/render/fogLayer.ts#createFogField.follow (shared): the feature's statements; "…" marks code left out
    follow(x, y, dabs, groundAt) {
      const bands = view.follow(x, y, FOG_HOLD);
      // … 1 lines
      if (!bands) return null;
      // … 1 lines
      for (const band of bands) raster(dabs, groundAt, band);
    // … 1 lines
    },
