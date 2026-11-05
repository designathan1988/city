// What the feature uses of src/render/mesh/cylinder.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/cylinder.ts:6-9 interface Caps
/** A CYLINDER WITH ONLY THE CAPS THAT ARE EVER SEEN */

/** Which caps to keep: the top (+y) and the bottom (-y), as three's `CylinderGeometry` builds them. */
export interface Caps {
  readonly top: boolean;
  readonly bottom: boolean;
}

// src/render/mesh/cylinder.ts:16-45 function cylinderWith
/** `CylinderGeometry(radiusTop, radiusBottom, height, radialSegments)` with the caps `caps` leaves out removed, and their vertices with them:
 *  DO NOT draw a cap that stands buried, inside another solid or against a housing - a closed post was a quarter caps no one saw. */
export function cylinderWith(radiusTop: number, radiusBottom: number, height: number, radialSegments: number, caps: Caps): BufferGeometry;
