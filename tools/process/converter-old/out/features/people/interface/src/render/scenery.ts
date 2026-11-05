// What the feature uses of src/render/scenery.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/scenery.ts:61-61 variable LAMP_POOL_RADIUS
/** Radius of the bright core under a street lamp (the cone's foot). */
export const LAMP_POOL_RADIUS = m(4.6);

// src/render/scenery.ts:63-63 variable POOL_ALONG
/** The light a street lamp lays on the road: not a spotlight's disc but the long patch of a road luminaire's spread */
export const POOL_ALONG = 1.6;

// src/render/scenery.ts:66-66 variable LAMP_BEAM_RADIUS
/** A lamp's visible cone of light, drawn as Volumetric Light Beam draws one (saladgamer.com/vlb-doc/comp-lightbeam-sd/) */
/** The cone's radius at the ground: the bright core of the pool. */
export const LAMP_BEAM_RADIUS = LAMP_POOL_RADIUS * 0.8;

// src/render/scenery.ts:373-376 function foliageTint
/** A near-white multiplier, so no two plants of one species are identical. */
export function foliageTint(rng: Rng): Color;
