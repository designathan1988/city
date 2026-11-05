// The code of src/render/utilities.ts that is the feature's (the rest of the file is shared).

// src/render/utilities.ts:123-147 slice of src/render/utilities.ts#polePartSpecs (shared): the feature's statements; "…" marks code left out
export function polePartSpecs(kit: SceneryKit): readonly PolePartSpec[] {
  // … 6 lines
  const lens: PolePartSpec = {
    name: 'utility-lamp-lenses', geometry: lensGeometry, material: kit.glow, castShadow: false, receiveShadow: true, renderOrder: 0,
    glows: false, brightness: () => Math.max(1, kit.glow.color.r, kit.glow.color.g, kit.glow.color.b), levels: [{ geometry: lensGeometry, error: 0 }],
  };
  return [
    solid('utility-poles', mastLevels[0]!.geometry, timber, mastLevels),
    solid('utility-arms', boxGeometry, timber),
    solid('utility-caps', capGeometry, steel),
    solid('utility-braces', boxGeometry, steel),
    solid('utility-insulators', insulatorGeometry, porcelain),
    solid('utility-transformers', canGeometry, steel),
    solid('utility-lamp-arms', boxGeometry, metal),
    solid('utility-lamp-heads', boxGeometry, housing),
    lens,
    light('utility-lamp-pools', kit.pool, kit.poolGlow, 3),
    light('utility-lamp-beams', kit.beam, kit.beamGlow, 4),
  ];
}

// src/render/utilities.ts:155-237 slice of src/render/utilities.ts#placePole (shared): the feature's statements; "…" marks code left out
export function placePole(
// … 6 lines
): void {
  // … 71 lines
  out[POLE_PART.pool]!.push({ x: headX, y: headY, z: below + m(0.012), yaw: 0, sx: LAMP_POOL_RADIUS, sy: 1, sz: LAMP_POOL_RADIUS });
  // … 1 lines
  out[POLE_PART.beam]!.push({ x: headX, y: headY, z: lensZ, yaw: 0, sx: LAMP_BEAM_RADIUS, sy: Math.max(m(0.004), lensZ - below), sz: LAMP_BEAM_RADIUS });
}

// src/render/utilities.ts:419-449 slice of src/render/utilities.ts#wireMaterials (shared): the feature's statements; "…" marks code left out
function wireMaterials(): { readonly lit: MeshStandardMaterial; readonly depth: MeshDepthMaterial } {
  // … 2 lines
  lit.onBeforeCompile = (shader) => {
    Object.assign(shader.uniforms, windUniforms, { uWireAmp: { value: WIRE_SWING } }, wireScreen);
    shader.vertexShader = shader.vertexShader
      .replace('#include <common>', `#include <common>${WIRE_WIND_UNIFORMS_GLSL}
        uniform float uWirePixels;${WIRE_GLSL}`)
      .replace('#include <beginnormal_vertex>', `
        vec3 wireNormal;
        vec3 wirePoint = wirePosition(wireNormal, uWirePixels);
        vec3 objectNormal = wireNormal;`)
      .replace('#include <begin_vertex>', `
        vec3 transformed = wirePoint;
        float wireT = position.x;
        float wireSwing = 4.0 * wireT * (1.0 - wireT);${wireWindGlsl('wireSwing', 'aSagPhase.y')}`);
  };
// … 13 lines
}

// src/render/utilities.ts:524-537 slice of src/render/utilities.ts#poleGroundAt (shared): the feature's statements; "…" marks code left out
export function poleGroundAt(
  elevation: RoadElevation,
// … 3 lines
): (x: number, y: number) => number {
  return (x, y) => {
    const road = elevation.roadAt(x, y, GROUND_ONLY);
    // … 1 lines
    const onFootway = road.type >= 0 && Math.abs(road.across) <= road.half;
    if (!onFootway) return terrainAt(x, y);
    return elevation.at(x, y, GROUND_ONLY) + (net ? footwayRiseAt(net, x, y) : FOOTWAY_RISE);
  };
}
