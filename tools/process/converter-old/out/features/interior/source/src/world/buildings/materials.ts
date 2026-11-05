// The code of src/world/buildings/materials.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/materials.ts:118-122 type MaterialTarget: the feature's own
export type MaterialTarget =
  | { readonly scope: 'building'; readonly slot: MaterialSlot }
  | { readonly scope: 'volume'; readonly volume: number; readonly slot: 'wall' | 'roof' }
  | { readonly scope: 'side'; readonly volume: number; readonly side: FaceId }
  | { readonly scope: 'floor'; readonly volume: number; readonly floor: number; readonly face: FaceId };

// src/world/buildings/materials.ts:125-149 function materialAt: the feature's own
export function materialAt(b: Building, target: MaterialTarget): MaterialSpec | null {
  switch (target.scope) {
    case 'building':
      switch (target.slot) {
        case 'wall': return b.materials?.wall ?? paletteOf(b).wall;
        case 'trim': return trimMaterial(b);
        case 'plinth': return plinthMaterial(b);
        case 'roof': return b.materials?.roof ?? paletteOf(b).roof;
      }
      return null;
    case 'volume': {
      const v = b.volumes.find((x) => x.id === target.volume);
      if (!v) return null;
      return target.slot === 'roof' ? roofMaterial(b, v) : v.materials?.wall ?? b.materials?.wall ?? paletteOf(b).wall;
    }
    case 'side': {
      const v = b.volumes.find((x) => x.id === target.volume);
      return v ? wallMaterial(b, v, target.side) : null;
    }
    case 'floor': {
      const v = b.volumes.find((x) => x.id === target.volume);
      return v ? wallMaterial(b, v, target.face, target.floor) : null;
    }
  }
}

// src/world/buildings/materials.ts:152-223 function applyMaterial: the feature's own
export function applyMaterial(b: Building, target: MaterialTarget, value: MaterialSpec | null): boolean {
  const before = JSON.stringify([b.materials, b.volumes.map((v) => [v.materials, v.storeys.map((s) => s.materials)])]);
  const tidyVolume = (v: Volume): void => {
    const m = v.materials;
    if (!m) return;
    if (m.sides && Object.keys(m.sides).length === 0) delete m.sides;
    if (Object.keys(m).length === 0) delete v.materials;
  };
  switch (target.scope) {
    case 'building': {
      const next: BuildingMaterials = { ...(b.materials ?? {}) };
      if (value) next[target.slot] = { ...value };
      else delete next[target.slot];
      if (Object.keys(next).length > 0) b.materials = next;
      else delete b.materials;
      if (value && (target.slot === 'wall' || target.slot === 'roof')) {
        for (const v of b.volumes) {
          if (target.slot === 'wall') for (const storey of v.storeys) delete storey.materials;
          if (!v.materials) continue;
          delete v.materials[target.slot];
          if (target.slot === 'wall') delete v.materials.sides;
          tidyVolume(v);
        }
      }
      break;
    }
    case 'volume': {
      const v = b.volumes.find((x) => x.id === target.volume);
      if (!v) return false;
      const m: VolumeMaterials = { ...(v.materials ?? {}) };
      if (value) m[target.slot] = { ...value };
      else delete m[target.slot];
      if (value && target.slot === 'wall') delete m.sides;
      if (value && target.slot === 'wall') for (const storey of v.storeys) delete storey.materials;
      v.materials = m;
      tidyVolume(v);
      break;
    }
    case 'side': {
      const v = b.volumes.find((x) => x.id === target.volume);
      if (!v) return false;
      const m: VolumeMaterials = { ...(v.materials ?? {}) };
      const sides = { ...(m.sides ?? {}) };
      if (value) sides[target.side] = { ...value };
      else delete sides[target.side];
      m.sides = sides;
      if (value) for (const storey of v.storeys) {
        if (storey.materials) {
          delete storey.materials[target.side];
          if (Object.keys(storey.materials).length === 0) delete storey.materials;
        }
      }
      v.materials = m;
      tidyVolume(v);
      break;
    }
    case 'floor': {
      const v = b.volumes.find((x) => x.id === target.volume);
      const storey = v?.storeys[target.floor];
      if (!v || !storey) return false;
      const materials = { ...(storey.materials ?? {}) };
      for (const face of volumeSides(v)) {
        if (value) materials[face] = { ...value };
        else delete materials[face];
      }
      if (Object.keys(materials).length > 0) storey.materials = materials;
      else delete storey.materials;
      break;
    }
  }
  return JSON.stringify([b.materials, b.volumes.map((v) => [v.materials, v.storeys.map((s) => s.materials)])]) !== before;
}

// src/world/buildings/materials.ts:284-296 function applyStyle: the feature's own
export function applyStyle(b: Building, key: string): boolean {
  const style = STYLES.find((s) => s.key === key);
  if (!style) return false;
  // The storeys' own paint too (`wallMaterial` reads it first)
  const state = (): string => JSON.stringify([b.materials, b.volumes.map((v) => [v.materials, v.storeys.map((s) => s.materials)])]);
  const before = state();
  b.materials = { ...style.materials };
  for (const v of b.volumes) {
    delete v.materials;
    for (const storey of v.storeys) delete storey.materials;
  }
  return state() !== before;
}
