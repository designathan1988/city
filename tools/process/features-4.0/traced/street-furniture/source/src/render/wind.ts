// The code of src/render/wind.ts that is the feature's (the rest of the file is shared).

// src/render/wind.ts:151-158 function wireWindGlsl: the feature's own
export function wireWindGlsl(swing: string, phase: string): string {
  return `
        float front = dot(transformed.xz, uWindDir) * ${perM(0.03).toFixed(6)} - uWindTime * 0.42;
        float gust = 0.45 + 0.55 * smoothstep(-0.6, 1.0, sin(front) + 0.35 * sin(front * 2.3 + 1.7));
        float swing = sin(uWindTime * 1.9 + ${phase}) + 0.35 * sin(uWindTime * 3.7 + ${phase} * 2.3);
        transformed.xz += uWindDir * (0.55 + 0.45 * swing) * gust * ${swing} * uWireAmp * uWindStrength;
        transformed.y += 0.25 * cos(uWindTime * 1.9 + ${phase}) * gust * ${swing} * uWireAmp * uWindStrength;`;
}

// src/render/wind.ts:161-166 function applyStructureSway: the feature's own
export function applyStructureSway(material: Material, originHeight: number, tall: number, amplitude: number, _key: string): void {
  installStructureSway(material, { uSwayOrigin: { value: originHeight } }, `
        uniform float uSwayOrigin;`, `
           float swayGround = instanceMatrix[3].y - uSwayOrigin;
           vec2 swayRoot = instanceMatrix[3].xz;`, tall, amplitude, 'structure');
}

// src/render/wind.ts:170-176 function applyPartSway: the feature's own
export function applyPartSway(material: Material, tall: number, amplitude: number, _key: string): void {
  installStructureSway(material, {}, `
        attribute vec4 aSwayPart;`, `
           vec3 swayPartAt = (instanceMatrix * vec4(aSwayPart.xyz, 1.0)).xyz;
           float swayGround = swayPartAt.y - aSwayPart.w;
           vec2 swayRoot = swayPartAt.xz;`, tall, amplitude, 'part');
}

// src/render/wind.ts:179-209 function installStructureSway: the feature's own
function installStructureSway(material: Material, uniforms: Record<string, { value: unknown }>, declarations: string, foot: string,
  tall: number, amplitude: number, variant: 'structure' | 'part'): void {
  markSway(material);
  const previous = material.onBeforeCompile.bind(material);
  material.onBeforeCompile = (shader, renderer) => {
    previous(shader, renderer);
    Object.assign(shader.uniforms, windUniforms, uniforms, { uSwayTall: { value: tall }, uSwayAmp: { value: amplitude } });
    shader.vertexShader = shader.vertexShader
      .replace('#include <common>', `#include <common>
        uniform float uWindTime;
        uniform vec2 uWindDir;
        uniform float uWindStrength;${declarations}
        uniform float uSwayTall;
        uniform float uSwayAmp;`)
      .replace('#include <project_vertex>', ShaderChunk.project_vertex.replace(
        'mvPosition = modelViewMatrix * mvPosition;',
        `#ifdef USE_INSTANCING${foot}
           float swayH = clamp((mvPosition.y - swayGround) / uSwayTall, 0.0, 1.4);
           float swayFront = dot(swayRoot, uWindDir) * ${perM(0.03).toFixed(6)} - uWindTime * 0.42;
           float swayGust = 0.45 + 0.55 * smoothstep(-0.6, 1.0, sin(swayFront));
           float swayPhase = dot(floor(swayRoot / ${m(1.6).toFixed(6)}), vec2(1.7, 2.3));
           float swayK = uSwayAmp * swayH * swayH * swayGust * (0.55 + 0.45 * sin(uWindTime * 2.3 + swayPhase)) * uWindStrength;
           mvPosition.xz += uWindDir * swayK;
         #endif
         mvPosition = modelViewMatrix * mvPosition;`,
      ));
  };
  // DO NOT key the program by whose it is (VEG-14: a program per signal part of the same code, as `applyWind` was): by the patch (its foot, `variant`), after whatever keyed it before.
  const before = material.customProgramCacheKey.bind(material);
  material.customProgramCacheKey = () => `${before()}-sway-${variant}`;
}
