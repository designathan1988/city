// The code of src/render/utilities.ts that is the feature's (the rest of the file is shared).

// src/render/utilities.ts:150-152 function wireParts: the feature's own
export function wireParts(): { readonly material: MeshStandardMaterial; readonly depth: MeshDepthMaterial } {
  return { material: PARTS.wireMaterial, depth: PARTS.wireDepth };
}

// src/render/utilities.ts:241-247 function writePlacement: the feature's own
export function writePlacement(placement: Placement, array: Float32Array, offset: number): void {
  placing.position.set(placement.x, placement.z, -placement.y);
  placing.rotation.set(0, placement.yaw, placement.roll ?? 0);
  placing.scale.set(placement.sx, placement.sy, placement.sz);
  placing.updateMatrix();
  placing.matrix.toArray(array, offset);
}

// src/render/utilities.ts:331-341 function wireLevel: the feature's own
export function wireLevel(pixelsPerMetre: number, sag: number): number {
  const width = WIRE_WIDTH * pixelsPerMetre;
  if (width < WIRE_FADE[1]) return -1;
  if (width >= WIRE_TUBE_PIXELS) return 0;
  // A strip's chords stray from the parabola by sag / segments squared: the fewest chords that keep that under a pixel.
  for (let level = WIRE_TEMPLATES.length - 1; level > 1; level--) {
    const n = WIRE_TEMPLATES[level]!.segments;
    if ((sag * pixelsPerMetre) / (n * n) < 1) return level;
  }
  return 1;
}

// src/render/utilities.ts:344-354 function wireGeometry: the feature's own
export function wireGeometry(level: number, instances: InstancedInterleavedBuffer): InstancedBufferGeometry {
  const template = WIRE_TEMPLATES[level]!;
  const geometry = new InstancedBufferGeometry();
  geometry.setIndex(template.index);
  geometry.setAttribute('position', template.position);
  geometry.setAttribute('normal', template.normal);
  geometry.setAttribute('aStart', new InterleavedBufferAttribute(instances, 3, 0));
  geometry.setAttribute('aEnd', new InterleavedBufferAttribute(instances, 3, 3));
  geometry.setAttribute('aSagPhase', new InterleavedBufferAttribute(instances, 2, 6));
  return geometry;
}

// src/render/utilities.ts:357-362 function releaseWireGeometry: the feature's own
export function releaseWireGeometry(geometry: InstancedBufferGeometry): void {
  geometry.setIndex(null);
  geometry.deleteAttribute('position');
  geometry.deleteAttribute('normal');
  geometry.dispose();
}

// src/render/utilities.ts:419-449 slice of src/render/utilities.ts#wireMaterials (shared): the feature's statements; "…" marks code left out
function wireMaterials(): { readonly lit: MeshStandardMaterial; readonly depth: MeshDepthMaterial } {
  // … 2 lines
  lit.onBeforeCompile = (shader) => {
    Object.assign(shader.uniforms, windUniforms, { uWireAmp: { value: WIRE_SWING } }, wireScreen);
  // … 11 lines
  };
// … 13 lines
}
