// The code of src/world/cityGen/field.ts that is the feature's (the rest of the file is shared).

// src/world/cityGen/field.ts:42-48 slice of src/world/cityGen/field.ts#TensorField.major (shared): the feature's statements; "…" marks code left out
  major(x: number, y: number): Vec2 | null {
    // … 2 lines
    const turn = this.noise.amount * valueNoise(x / this.noise.scale + this.noise.seed * 0.013, y / this.noise.scale - this.noise.seed * 0.007);
    const phi = Math.atan2(b, a) / 2 + turn;
    return { x: Math.cos(phi), y: Math.sin(phi) };
  }
