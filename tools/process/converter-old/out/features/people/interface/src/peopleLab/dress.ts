// What the feature uses of src/peopleLab/dress.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/dress.ts:11-40 function dressPerson
/** A BUILT PERSON DRESSED AS THE LAB DRAWS THEM: the lab's finish (`polish.ts`), then the game's textures (skin, irises, fabrics, `skinAppearance.ts`) with the lab's changes */

/** `painted`: painted skins by their source texture, kept by the caller (each made once). */
export async function dressPerson(person: BuiltPerson, spec: PersonSpec, layers: PersonLayers, painted: WeakMap<Texture, Texture>, sculpted = false): Promise<SkinAppearance>;
