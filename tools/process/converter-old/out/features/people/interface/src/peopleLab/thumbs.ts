// What the feature uses of src/peopleLab/thumbs.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/peopleLab/thumbs.ts:13-13 type Framing
/** PICTURES OF CHOICES, as Create-a-Sim's tiles: each preset shown on the person themself, built and photographed off screen, one at a time, kept for the page's life */

/** What a picture shows: the whole body, the feet, the head, the face, or one feature of it up close. */
export type Framing = 'face' | 'head' | 'body' | 'feet' | 'eyes' | 'brows' | 'nose' | 'mouth' | 'cheeks' | 'jaw' | 'ears';

// src/peopleLab/thumbs.ts:25-145 class Thumbs
export class Thumbs { … }

// src/peopleLab/thumbs.ts:34-34 field Thumbs.layers
/** Its own texture arrays (`createPersonLayers`): the main view's are bound to the main renderer and must never be shared across WebGL contexts. */
  private readonly layers: PersonLayers = createPersonLayers();

// src/peopleLab/thumbs.ts:107-144 method Thumbs.shoot
private async shoot(person: LabPerson, framing: Framing): Promise<string>;
