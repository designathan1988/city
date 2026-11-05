// The code of src/render/buildings/massingMaterial.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/massingMaterial.ts:59-126 slice of src/render/buildings/massingMaterial.ts#createMassingMaterial (shared): the feature's statements; "…" marks code left out
export function createMassingMaterial(uniforms: MassingUniforms, base = false): MeshStandardMaterial {
  // … 3 lines
  material.onBeforeCompile = (shader) => {
    // … 15 lines
    shader.fragmentShader = shader.fragmentShader
      .replace('#include <common>', `#include <common>
uniform float roomsAwake;
uniform float massingNight;
uniform float roomDaylight;
varying vec4 vFacade;
varying vec2 vGlazing;
varying float vHeight;
float massGlass = 0.0;
float massLit = 0.0;
vec3 massLamp = vec3(1.0);
${ROOM_LAMP_GLSL}
${PULSE_GLSL('mass')}`)
      .replace('#include <color_fragment>', `#include <color_fragment>
if (vFacade.y > 0.0) {
  // Bays along the face and storeys up it.
  vec2 cell = vec2(vFacade.x / vFacade.y, (vHeight - vFacade.w) / max(vFacade.z, 0.01));
  vec2 foot = massFootprint(cell);
  float share = vGlazing.x;
  // A curtain wall: its panes between mullions (the kit's frame), nine tenths glass each way.
  float across = share > 0.99 ? 0.9 : massPulse(cell.x, foot.x, 0.5 - 0.5 * share, 0.5 + 0.5 * share);
  float up = share > 0.99 ? 0.9 : massPulse(cell.y, foot.y, ${SILL.toFixed(2)}, ${HEAD.toFixed(2)});
  // A window under a pixel leaves its mean: kept at full strength the glass the detail shows, not washed into the wall (the town at 1–2 km read as pastel boxes, 2026-10-20).
  massGlass = clamp(across * up * 1.45, 0.0, 1.0) * step(0.0, cell.y);
  // Which room is lit: its own lot near, the share awake once a window is under a pixel; by day (\`massingNight\` 0, a uniform: one branch for every pixel) none glows, so none is worked out.
  if (massingNight > 0.0) {
    vec2 room = floor(cell);
    float seed = fract(vGlazing.y), office = step(1.5, vGlazing.y);
    float lot = fract(sin(dot(room, vec2(12.9898, 78.233)) + seed * 437.585) * 43758.5453);
    float near = step(1.0 - roomsAwake, lot);
    float sub = smoothstep(0.5, 1.5, max(foot.x, foot.y));
    massLit = mix(near, roomsAwake, sub);
    // Its lamp from the mix the detailed panes draw from (\`roomLamps.ts\`); under a pixel, the mix's mean, so a far block holds its colour.
    massLamp = mix(roomLamp(fract(lot * 17.31), office), roomLampMean(office), sub);
  }
  // The panes as the detail draws them, a curtain wall's too (its mullions the rest): dark glass, the sky's reflection and the room behind it (emissive below).
  diffuseColor.rgb = mix(diffuseColor.rgb, ${glass}, massGlass);
  // The foot of the wall in the street's shade: a block stands on the ground, not on a card (the town from afar, 2026-10-20).
  diffuseColor.rgb *= mix(0.62, 1.0, smoothstep(0.0, ${m(1.4).toFixed(4)}, vHeight - vFacade.w));
}`)
      .replace('#include <roughnessmap_fragment>', '#include <roughnessmap_fragment>\nroughnessFactor = mix(roughnessFactor, 0.06, massGlass);')
      .replace('#include <emissivemap_fragment>', `#include <emissivemap_fragment>\ntotalEmissiveRadiance += (massLamp * massingNight * massLit + ${ROOM_SEEN_GLSL} * roomDaylight) * massGlass;`);
  };
// … 4 lines
}
