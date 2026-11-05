// The code of src/render/torusWindow.ts that is the feature's (the rest of the file is shared).

// src/render/torusWindow.ts:27-31 method TorusWindow.indexAt: the feature's own
  indexAt(x: number, y: number): number {
    const gx = Math.floor(x / this.cell), gy = Math.floor(y / this.cell);
    if (gx < this.ox || gy < this.oy || gx >= this.ox + this.res || gy >= this.oy + this.res) return -1;
    return this.texel(gy) * this.res + this.texel(gx);
  }
