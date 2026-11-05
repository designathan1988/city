// What the feature uses of src/render/people/skinAppearance.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/skinAppearance.ts:217-245 function skinChoice
/** Which skin texture and eye texture a person gets, and the tint matching the texture to their skin colour */
export function skinChoice(person: PersonSpec): { name: string; url: string; eyeFile: string; tint: Color };

// src/render/people/skinAppearance.ts:263-269 function skinLeaseOf
/** A skin texture by `skinChoice` name, shared while anyone holds it. */
function skinLeaseOf(name: string, url: string): TextureLease;

// src/render/people/skinAppearance.ts:272-278 function cardLeaseOf
/** An eye or card texture from the proxies folder, shared while anyone holds it. */
function cardLeaseOf(file: string): TextureLease;

// src/render/people/skinAppearance.ts:604-604 interface TextureLease
interface TextureLease { texture: Promise<Texture>; release(): void }

// src/render/people/skinAppearance.ts:648-689 function paddedGarment
/** A garment's texture with every UV island grown outward into the background */
function paddedGarment(name: string, item: ProxyItem): TextureLease;
