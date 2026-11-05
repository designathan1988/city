// What the feature uses of src/render/people/personBuild.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/personBuild.ts:20-53 function buildPersonScene
/** The person's scene, its levels made: body, garments worn (the tailored shells where they fail to load) and, with `faces`, the live face's shapes. */
export async function buildPersonScene(person: PersonSpec, gender: 'f' | 'm', style: CartoonStyle, faces: boolean,
  beforeRig: () => Promise<void> = () => Promise.resolve()): Promise<Object3D>;
