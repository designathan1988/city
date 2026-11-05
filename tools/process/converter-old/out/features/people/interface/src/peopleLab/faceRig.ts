// What the feature uses of src/peopleLab/faceRig.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/faceRig.ts:40-65 function attachFace
/** The 52 shapes as morphs of the built person (through `BuiltPerson.morph`, the body and what is worn on the face), its own 13 channels
 *  replaced; the gaze kept. A few frames at a time, so the page keeps drawing. False when it cannot (the tailored shells). */
export async function attachFace(built: BuiltPerson, current: () => boolean): Promise<boolean>;

// src/peopleLab/faceRig.ts:68-81 function applyLabFace
/** A face written into a mesh's influences: the 52 shapes by name, the game's channels (from the street's life of the face) mixed into them. */
export function applyLabFace(influences: number[], dictionary: Record<string, number>, face: Readonly<Record<string, number>>, strength = 1): void;
