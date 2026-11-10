// The code of src/render/buildings/finishes.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/finishes.ts:139-223 slice of src/render/buildings/finishes.ts#shellShader (shared): the feature's statements; "…" marks code left out
function shellShader(shader: WebGLProgramParametersWithUniforms, maps: SurfaceBake | undefined, looks: { value: Float32Array }, weathered: boolean, carried: boolean, tones: Float32Array | null = null): void {
  // … 2 lines
  if (maps) {
    shader.uniforms['shellMaps'] = { value: maps.map };
    shader.uniforms['shellNormals'] = { value: maps.normalMap };
    shader.uniforms['shellRough'] = { value: maps.roughnessMap };
  }
  // … 47 lines
  shader.fragmentShader = shader.fragmentShader
    .replace('#include <common>', `#include <common>\n${FINISH_PARS}\n${maps ? MAP_PARS : ''}\n${weathered ? WEATHER_PARS : ''}\n${tones ? `uniform vec4 shellTones[${FINISHES.length * 2}];
vec4 shellFar = vec4(1.0);` : ''}`)
    .replace('#include <envmap_physical_pars_fragment>', ShaderChunk.envmap_physical_pars_fragment.replace(/\benvMapIntensity\b/g, '( envMapIntensity * shellEnvScale )'))
    .replace('#include <map_fragment>', `#include <map_fragment>
shellLook = shellLooks[vShellFinish * ${LOOK_ROWS}];
shellKind = shellLooks[vShellFinish * ${LOOK_ROWS} + 1];
shellEnvScale = shellLook.w;
${maps ? `diffuseColor *= texture(shellMaps, ${layer});` : ''}
${tones ? FAR_FINISH : weathered ? WEATHERING : ''}`)
    .replace('#include <roughnessmap_fragment>', `#include <roughnessmap_fragment>
${maps ? `shellRoughTexel = texture(shellRough, ${layer});\nroughnessFactor *= shellRoughTexel.g;` : ''}
${tones ? 'roughnessFactor *= shellFar.x;' : ''}`)
    // The glass's metal (its mullions) read from the roughness map's blue, as three reads a metalness map.
    .replace('#include <metalnessmap_fragment>', `#include <metalnessmap_fragment>
metalnessFactor *= shellLook.z;
${maps ? 'if (shellKind.x > 0.5) metalnessFactor *= shellRoughTexel.b;' : ''}
${tones ? 'if (shellKind.x > 0.5) metalnessFactor *= shellFar.y;' : ''}`)
    .replace('#include <normal_fragment_maps>', `#include <normal_fragment_maps>
${maps ? `{
  mat3 shellTbn = shellTangentFrame(- vViewPosition, normal, vShellUv);
  #ifdef DOUBLE_SIDED
    shellTbn[0] *= faceDirection;
    shellTbn[1] *= faceDirection;
  #endif
  vec3 mapN = texture(shellNormals, ${layer}).xyz * 2.0 - 1.0;
  mapN.xy *= shellLook.y;
  normal = normalize(shellTbn * mapN);
}` : ''}`);
}

// src/render/buildings/finishes.ts:230-247 slice of src/render/buildings/finishes.ts#createShellMaterial (shared): the feature's statements; "…" marks code left out
export function createShellMaterial(options: { readonly plain?: boolean; readonly carried?: boolean; readonly far?: boolean } = {}, anisotropy = 4): MeshStandardMaterial {
  // … 4 lines
  const maps = far ? undefined : surfaceMaps('building-finishes', anisotropy);
  // … 2 lines
  const material = new MeshStandardMaterial({ vertexColors: true, roughness: maps || far ? 1 : 0.9, metalness: 1, envMapIntensity: 1, shadowSide: BackSide });
  // … 1 lines
  const looks = { value: finishLookTable(maps !== undefined) };
  material.onBeforeCompile = (shader) => shellShader(shader, maps, looks, !plain, carried, tones);
  const key = `building-shell-v2-${plain ? 'plain' : 'weathered'}-${carried ? 'carried' : 'laid'}-${maps ? 'maps' : 'none'}${far ? '-far' : ''}`;
  material.customProgramCacheKey = () => key;
// … 3 lines
}
