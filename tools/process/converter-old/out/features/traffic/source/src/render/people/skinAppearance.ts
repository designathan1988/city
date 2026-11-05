// The code of src/render/people/skinAppearance.ts that is the feature's (the rest of the file is shared).

// src/render/people/skinAppearance.ts:281-283 function skinTextures: the feature's own
export function skinTextures(name: string, url: string, eyeFile: string): Promise<[Texture, Texture]> {
  return Promise.all([skinLeaseOf(name, url).texture, cardLeaseOf(eyeFile).texture]);
}

// src/render/people/skinAppearance.ts:286-289 function itemTexture: the feature's own
export function itemTexture(name: string, item: ProxyItem): Promise<Texture> | null {
  if (!item.textureFile) return null;
  return item.transparent ? cardLeaseOf(item.textureFile).texture : paddedGarment(name, item).texture;
}
