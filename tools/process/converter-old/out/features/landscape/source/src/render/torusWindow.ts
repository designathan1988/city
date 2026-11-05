// The code of src/render/torusWindow.ts that is the feature's (the rest of the file is shared).

// src/render/torusWindow.ts:34-38 method TorusWindow.bandOf: the feature's own
  bandOf(rect: readonly [number, number, number, number]): TexelBand | null {
    const gx0 = Math.max(this.ox, Math.floor(rect[0] / this.cell)), gx1 = Math.min(this.ox + this.res - 1, Math.floor(rect[2] / this.cell));
    const gy0 = Math.max(this.oy, Math.floor(rect[1] / this.cell)), gy1 = Math.min(this.oy + this.res - 1, Math.floor(rect[3] / this.cell));
    return gx0 > gx1 || gy0 > gy1 ? null : [gx0, gx1, gy0, gy1];
  }
