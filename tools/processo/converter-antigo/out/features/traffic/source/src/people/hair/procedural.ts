// The code of src/people/hair/procedural.ts that is the feature's (the rest of the file is shared).

// src/people/hair/procedural.ts:242-244 function generateHairStrands: the feature's own
export function generateHairStrands(style: HairStyle, base: HairBase, seed = 1): ProxyPack {
  return generateHair(style, base, seed, true);
}
