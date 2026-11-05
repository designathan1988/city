// What the feature uses of src/render/instanceRuns.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/instanceRuns.ts:16-16 type PoseFormat
/** The same design as the buildings' gardens (`gardens.ts`, which keeps a run per garden list), on the same cells (`cellInstances.ts`) */

/** DO NOT keep a matrix per thing, on the CPU or the GPU: a pose of `stride` floats, made three's matrix by the vertex shader (`cellInstances.ts` `posedMaterial`). */
export type PoseFormat = SlotFormat;

// src/render/instanceRuns.ts:19-27 function yawMatrixInto
/** DO NOT go through an `Object3D` for each instance: three's `compose` of a turn about the up axis (r186 `Matrix4.compose`, `Quaternion.setFromEuler` with x = z = 0) */
export function yawMatrixInto(out: Float32Array | number[], at: number, px: number, py: number, pz: number, yaw: number, sx: number, sy: number, sz: number): void;

// src/render/instanceRuns.ts:33-33 variable NEAR_SQUARE
/**
 * THE NEAR SQUARE: the world cells round the eye whose things are drawn by the layers' near cells (`propCells.ts`), x and y the eye's
 * cell, z how many cells it reaches each way (-1: none). A thing of a far batch standing in it is not drawn (its pose's eighth float).
 */
export const NEAR_SQUARE = { value: new Vector3(0, 0, -1) };

// src/render/instanceRuns.ts:36-36 variable NEAR_CODE_SPAN
/** A far batch's cell (`propCells.ts`) as a pose's eighth float: exact in float32 for |column|, |row| < 2048 (524 km of 256 m cells). */
export const NEAR_CODE_SPAN = 4096;

// src/render/instanceRuns.ts:39-39 variable NEVER_NEAR
/** The eighth float of a thing drawn wherever it stands (never by a far batch). */
export const NEVER_NEAR = -1;

// src/render/instanceRuns.ts:42-66 variable YAW_POSE
/** A thing turned about the up axis: its place in the scene's frame (x, y, z), its yaw, its scale on three axes, and the cell whose near drawing hides it (8 floats, not 12). */
export const YAW_POSE: PoseFormat = {
  stride: 8,
  key: 'yaw',
  pars: /* glsl */ 'uniform vec3 uNearSquare;',
  uniforms: { uNearSquare: NEAR_SQUARE },
  // A far batch's thing in the near square (`nearCode`, -1 none) is drawn by the near cells, not here: every vertex at one point.
  guard: /* glsl */ `
  if (b.w >= 0.0) {
    float column = floor(b.w / ${NEAR_CODE_SPAN.toFixed(1)});
    vec2 cell = vec2(column, b.w - column * ${NEAR_CODE_SPAN.toFixed(1)}) - ${(NEAR_CODE_SPAN / 2).toFixed(1)};
    vec2 off = abs(cell - uNearSquare.xy);
    if (max(off.x, off.y) <= uNearSquare.z) poseHidden = true;
  }`,
  // `yawMatrixInto`: a = x, y, z, yaw; b = the scale on three axes.
  glsl: /* glsl */ `
  float y = sin(a.w / 2.0);
  float w = cos(a.w / 2.0);
  float y2 = y + y;
  float yy = y * y2;
  float wy = w * y2;
  return mat4((1.0 - yy) * b.x, 0.0, -wy * b.x, 0.0, 0.0, b.y, 0.0, 0.0, wy * b.z, 0.0, (1.0 - yy) * b.z, 0.0, a.x, a.y, a.z, 1.0);`,
  matrixInto(data, at, out, outAt) {
    yawMatrixInto(out, outAt, data[at]!, data[at + 1]!, data[at + 2]!, data[at + 3]!, data[at + 4]!, data[at + 5]!, data[at + 6]!);
  },
};
