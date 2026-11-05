// What the feature uses of src/render/buildings/finishes.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/finishes.ts:13-13 variable LOOK_ROWS
/** The building finishes ("Materials"): their recipes in `bake/finishRecipes.ts`, baked at build time as one texture array per map, a layer per finish */

/** Four-number rows a finish takes in the look table: (uv scale, normal scale, metalness, envMapIntensity), then (metal read from the map, roof, glassy, 0). */
const LOOK_ROWS = 2;

// src/render/buildings/finishes.ts:16-32 function finishLookTable
/** Each finish's scalars, indexed by its id (its place in `FINISHES`, its layer in the arrays); `baked`: its maps are loaded. */
export function finishLookTable(baked: boolean): Float32Array;

// src/render/buildings/finishes.ts:35-41 variable FINISH_PARS
/** GLSL: the pixel's finish rows, and the sky's share it reflects (read by three's IBL in place of the material's one `envMapIntensity`). */
const FINISH_PARS = `
uniform vec4 shellLooks[${FINISHES.length * LOOK_ROWS}];
flat varying int vShellFinish;
varying vec2 vShellUv;
vec4 shellLook = vec4(0.0);
vec4 shellKind = vec4(0.0);
float shellEnvScale = 1.0;`;

// src/render/buildings/finishes.ts:44-62 variable MAP_PARS
/** GLSL: the finishes' maps as texture arrays, and the tangent frame three builds for a normal map without tangents (r186 `normalmap_pars_fragment`; C. Schueler, "Normal Mapping Without Precomputed Tangents"). */
const MAP_PARS = `
uniform sampler2DArray shellMaps;
uniform sampler2DArray shellNormals;
uniform sampler2DArray shellRough;
vec4 shellRoughTexel = vec4(1.0);
mat3 shellTangentFrame(vec3 eye_pos, vec3 surf_norm, vec2 uv) {
  vec3 q0 = dFdx(eye_pos.xyz);
  vec3 q1 = dFdy(eye_pos.xyz);
  vec2 st0 = dFdx(uv.st);
  vec2 st1 = dFdy(uv.st);
  vec3 N = surf_norm;
  vec3 q1perp = cross(q1, N);
  vec3 q0perp = cross(N, q0);
  vec3 T = q1perp * st0.x + q0perp * st1.x;
  vec3 B = q1perp * st0.y + q0perp * st1.y;
  float det = max(dot(T, T), dot(B, B));
  float scale = (det == 0.0) ? 0.0 : inversesqrt(det);
  return mat3(T * scale, B * scale, N);
}`;

// src/render/buildings/finishes.ts:65-75 variable WEATHER_PARS
/** GLSL: the weathering's noise. */
const WEATHER_PARS = `
varying float vDecay;
varying vec3 vDecayWorld;
varying vec3 vDecayNormal;
float dHash(vec3 p) { return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453); }
float dNoise(vec3 p) {
  vec3 i = floor(p), f = fract(p);
  f = f * f * (3.0 - 2.0 * f);
  return mix(mix(mix(dHash(i), dHash(i + vec3(1, 0, 0)), f.x), mix(dHash(i + vec3(0, 1, 0)), dHash(i + vec3(1, 1, 0)), f.x), f.y),
             mix(mix(dHash(i + vec3(0, 0, 1)), dHash(i + vec3(1, 0, 1)), f.x), mix(dHash(i + vec3(0, 1, 1)), dHash(i + vec3(1, 1, 1)), f.x), f.y), f.z);
}`;

// src/render/buildings/finishes.ts:78-122 variable WEATHERING
/** GLSL after the map: buildings that are not maintained run down (the player's order of 2026-10-05), each finish as its row says (roof, glassy) */
const WEATHERING = `
// The pixel's footprint on the wall, metres: each term below filtered by it (Inigo Quilez, "filterable procedurals"). DO NOT sample a pattern finer than
// the pixel at its centre: a wall seen along the street took one random streak a pixel at full contrast, stripes and speckle (the player, 2026-10-23).
// Taken here, in uniform control flow: derivatives in the branches below are undefined.
float shellFoot = max(length(dFdx(vDecayWorld)), length(dFdy(vDecayWorld)));
{
  vec3 w = vDecayWorld;
  // MACRO VARIATION, on every building; its finest octave its mean (0.497) once under 2 pixels.
  float fine = mix(dNoise(w * 0.6), 0.497, smoothstep(0.25, 0.5, shellFoot * 0.6));
  float macro = dNoise(w * 0.045) * 0.55 + dNoise(w * 0.17) * 0.3 + fine * 0.15;
  diffuseColor.rgb *= 0.9 + 0.2 * macro;
}
{
  // After the texture: the stains darken the finish as painted (streaks, grime), heavier as a building runs down (aDecay).
  float d = max(vDecay, shellKind.z > 0.5 ? 0.0 : 0.3);
  // DO NOT work the stains out where they weigh nothing (a glassy finish not run down: 16 sines a pixel multiplied by 0, PRD-03), nor the streaks off a wall.
  if (d > 0.0) {
    vec3 w = vDecayWorld;
    bool wall = abs(vDecayNormal.y) < 0.5;
    // Grime: darker towards the foot of the wall and in soft patches.
    float grime = 0.5 + 0.5 * dNoise(w * 0.22);
    // Rain streaks: thin, long, running down from sills and copings; their edges widened by the footprint, their mean (0.160) once a streak is under 2 pixels.
    float sa = shellFoot * 2.2;
    float darken = d * (0.18 + 0.12 * grime + (wall ? 0.28 * mix(smoothstep(0.55 - sa, 0.85 + sa, dNoise(vec3(w.x * 2.2, w.y * 0.08, w.z * 2.2))), 0.160, smoothstep(0.25, 0.5, sa)) : 0.1));
    diffuseColor.rgb *= 1.0 - darken;
  }
}
if (vDecay > 0.001 && shellKind.z < 0.5) {
  float d = vDecay;
  vec3 w = vDecayWorld;
  bool wall = abs(vDecayNormal.y) < 0.5;
  if (shellKind.y > 0.5) {
    // Dark moss in the low corners of the roof; its mean (0.127) once a patch is under 2 pixels.
    float ma = shellFoot * 0.3;
    float moss = mix(smoothstep(0.6 - ma, 0.85 + ma, dNoise(w * 0.3)), 0.127, smoothstep(0.25, 0.5, ma));
    diffuseColor.rgb = mix(diffuseColor.rgb, diffuseColor.rgb * vec3(0.55, 0.62, 0.45), d * 0.6 * moss);
  } else {
    // Paint gone in a few small patches, the darker render showing (worked out on a wall only, where it shows); the edge widened by the footprint,
    // its mean over the wear (measured: 0.0412 + 0.0757 d + 0.037 d^2) once a patch is under 2 pixels.
    if (wall) {
      float peel = mix(smoothstep(0.8 - d * 0.12 - shellFoot * 1.3, 0.83 - d * 0.12 + shellFoot * 1.3, dNoise(w * 1.3 + 7.0)), 0.0412 + d * (0.0757 + 0.037 * d), smoothstep(0.25, 0.5, shellFoot * 1.3));
      diffuseColor.rgb = mix(diffuseColor.rgb, diffuseColor.rgb * vec3(0.72, 0.68, 0.62), peel * d);
    }
  }
}`;

// src/render/buildings/finishes.ts:125-127 variable FAR_FINISH
/** GLSL after the map, the far program's (`createShellMaterial` far): its finish's colour from afar for its texture (`finishAlbedo`), the wear's mean for its stains; its roughness and its glass's metal the means of its maps' (`finishFar`). */
const FAR_FINISH = `
shellFar = shellTones[vShellFinish * 2 + 1];
diffuseColor.rgb *= shellTones[vShellFinish * 2].rgb * (1.0 - 0.3 * vDecay);`;

// src/render/buildings/finishes.ts:139-223 function shellShader
/** The shell's program: each pixel's finish picks its layer of the maps and its rows of the look table; `weathered` adds the wear. A shell's vertex carries its paint and finish (`PAINT_ATTRIBUTE`), its uv worked out here from its place and normal as `Shell.face` laid it; instanced, the shell's parts (`shellParts.ts`) their finish, wear and ground per instance; `carried`: a ruin's pieces, which move, their uv, colour and finish carried per vertex (`fracture.ts`). */
function shellShader(shader: WebGLProgramParametersWithUniforms, maps: SurfaceBake | undefined, looks: { value: Float32Array }, weathered: boolean, carried: boolean, tones: Float32Array | null = null): void;

// src/render/buildings/finishes.ts:230-247 function createShellMaterial
/**
 * THE ONE SHELL MATERIAL: every finish of a cell's shells in one draw, textured where the build's baked surfaces are loaded (`surfaceBakeClient.ts`; none in a worker or a test).
 * `plain`: the faded copy's look ("Ocultar outros"): the finishes' maps and scalars without the wear or the lots, as the per-finish clones drew it.
 * `carried`: a ruin's (`destruction.ts`): its pieces move, so each vertex carries its uv, colour and finish (`shellShader`).
 */
export function createShellMaterial(options: { readonly plain?: boolean; readonly carried?: boolean; readonly far?: boolean } = {}, anisotropy = 4): MeshStandardMaterial;
