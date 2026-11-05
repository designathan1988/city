// What the feature uses of src/render/frustumGround.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/frustumGround.ts:20-42 function frustumGroundBox
/**
 * The ground box (world x, y = three's -z) of the frustum with each plane pushed out by `grow`, cut between the heights
 * `low` and `high`: every centre a sphere of radius `grow` may stand at and pass `Frustum.intersectsSphere` stands in it.
 * The cut's corners are the frustum's corners between the heights and its edges' crossings of them; an empty cut, an empty box.
 */
export function frustumGroundBox(frustum: Frustum, grow: number, low: number, high: number, out: GroundBox): GroundBox;
