// The code of src/world/junction/levels.ts that is the feature's (the rest of the file is shared).

// src/world/junction/levels.ts:252-254 method JunctionLevels.has: the feature's own
  has(level: SurfaceLevel): boolean {
    return (this.present & bit(level)) !== 0;
  }
