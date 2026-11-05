// The code of src/render/citizenPlays.ts that is the feature's (the rest of the file is shared).

// src/render/citizenPlays.ts:12-12 interface Play: the feature's own
export interface Play { row: number; t: number; weight: number }

// src/render/citizenPlays.ts:15-21 interface CitizenAtlas: the feature's own
export interface CitizenAtlas {
  /** Its texture swapped only when it grows (`appendClip`): the pass reads `anim.atlas`. */
  anim: ClassAnimation;
  readonly rowOf: ReadonlyMap<ClipFrames, number>;
  /** Rows written; the texture's height is its room. */
  rows: number;
}

// src/render/citizenPlays.ts:29-36 function atlasTexture: the feature's own
function atlasTexture(pixels: Float32Array, width: number, height: number): DataTexture {
  const texture = new DataTexture(pixels, width, height, RGBAFormat, FloatType);
  texture.minFilter = texture.magFilter = NearestFilter;
  texture.needsUpdate = true;
  // Up whole once (`appendClip` adds ranges only after: three sends only the ranges of a texture that has some, its first upload too, AGT-16).
  texture.onUpdate = () => { texture.userData['uploaded'] = true; };
  return texture;
}

// src/render/citizenPlays.ts:39-41 function animationOf: the feature's own
function animationOf(texture: DataTexture, bones: number, width: number, height: number): ClassAnimation {
  return { atlas: texture, rowOf: () => 0, bones, bytes: width * height * 16, refill() {} };
}

// src/render/citizenPlays.ts:44-58 function citizenAtlas: the feature's own
export function citizenAtlas(clips: readonly (ClipFrames | undefined)[], bones: number): CitizenAtlas {
  const packed = bones * PACKED_BONE_FLOATS;
  const width = bones * ATLAS_TEXELS;
  const rowOf = new Map<ClipFrames, number>();
  let rows = 0;
  for (const clip of clips) {
    if (!clip || rowOf.has(clip)) continue;
    rowOf.set(clip, rows);
    rows += Math.round(clip.data.length / packed);
  }
  const height = Math.max(1, Math.ceil(rows * (1 + ATLAS_SLACK)));
  const pixels = new Float32Array(width * 4 * height);
  for (const [clip, row] of rowOf) pixels.set(clip.data.subarray(0, Math.round(clip.data.length / packed) * packed), row * width * 4);
  return { anim: animationOf(atlasTexture(pixels, width, height), bones, width, height), rowOf, rows };
}

// src/render/citizenPlays.ts:61-87 function appendClip: the feature's own
export function appendClip(atlas: CitizenAtlas, clip: ClipFrames): boolean {
  if (atlas.rowOf.has(clip)) return false;
  const bones = atlas.anim.bones;
  const packed = bones * PACKED_BONE_FLOATS;
  const width = bones * ATLAS_TEXELS;
  const count = Math.round(clip.data.length / packed);
  const at = atlas.rows;
  let texture = atlas.anim.atlas;
  const height = texture.image.height;
  let grown = false;
  if (at + count > height) {
    const next = Math.max(at + count, height * 2);
    const pixels = new Float32Array(width * 4 * next);
    pixels.set((texture.image.data as Float32Array).subarray(0, at * width * 4));
    texture = atlasTexture(pixels, width, next);
    atlas.anim = animationOf(texture, bones, width, next);
    grown = true;
  }
  (texture.image.data as Float32Array).set(clip.data.subarray(0, count * packed), at * width * 4);
  // Each update range is one row of the image (three's WebGLTextures): the new rows only.
  // DO NOT put ranges on a texture not yet sent: grown by an earlier clip this frame, it would go up with these rows alone, the rest zero (bodies collapsed to a point, AGT-16).
  if (!grown && texture.userData['uploaded'] === true) for (let r = 0; r < count; r++) texture.addUpdateRange((at + r) * width * 4, width * 4);
  texture.needsUpdate = true;
  (atlas.rowOf as Map<ClipFrames, number>).set(clip, at);
  atlas.rows = at + count;
  return grown;
}

// src/render/citizenPlays.ts:90-124 function gpuPlays: the feature's own
export function gpuPlays(out: Play[], atlas: CitizenAtlas, clips: readonly ClipFrames[], phases: readonly number[],
  weights: readonly number[], single: boolean): number {
  if (single) {
    // The frame of the clip that weighs most, as it is: the row before it carried all the way (no row past the clip is read).
    let best = -1;
    for (let c = 0; c < clips.length; c++) if (best < 0 || weights[c]! > weights[best]!) best = c;
    if (best < 0) return 0;
    const clip = clips[best]!;
    const start = atlas.rowOf.get(clip);
    if (start === undefined) return -1;
    const frame = Math.round(Math.min(clip.frames, Math.max(0, phases[best]!)));
    const play = out[0]!;
    if (frame > 0) { play.row = start + frame - 1; play.t = 1; } else { play.row = start; play.t = 0; }
    play.weight = 1;
    return 1;
  }
  let total = 0;
  for (let c = 0; c < clips.length; c++) total += Math.max(0, weights[c]!);
  let n = 0;
  for (let c = 0; c < clips.length; c++) {
    const weight = Math.max(0, weights[c]!) / (total || 1);
    if (weight < 0.001) continue;
    if (n >= GPU_PLAYS) return -1;
    const clip = clips[c]!;
    const start = atlas.rowOf.get(clip);
    if (start === undefined) return -1;
    const f = Math.min(clip.frames, Math.max(0, phases[c]!));
    const whole = Math.floor(f);
    const play = out[n++]!;
    play.row = start + whole;
    play.t = f % 1;
    play.weight = weight;
  }
  return n;
}

// src/render/citizenPlays.ts:127-154 function cpuPose: the feature's own
export function cpuPose(out: Float32Array, offset: number, clips: readonly ClipFrames[], phases: readonly number[],
  weights: readonly number[], width: number, single: boolean): void {
  const bones = width / SKIN_BONE_FLOATS;
  const packedWidth = bones * PACKED_BONE_FLOATS;
  // Far off (a body a few pixels tall), the pose is the frame of the clip that weighs most, copied as it is
  if (single) {
    let best = -1;
    for (let c = 0; c < clips.length; c++) if (best < 0 || weights[c]! > weights[best]!) best = c;
    if (best >= 0) {
      const clip = clips[best]!;
      const start = Math.round(Math.min(clip.frames, Math.max(0, phases[best]!))) * packedWidth;
      copyPackedFrame(out, offset, clip.data, start, bones);
    } else out.fill(0, offset, offset + width);
    return;
  }
  out.fill(0, offset, offset + width);
  let total = 0;
  for (let c = 0; c < clips.length; c++) total += Math.max(0, weights[c]!);
  for (let c = 0; c < clips.length; c++) {
    const weight = Math.max(0, weights[c]!) / (total || 1);
    if (weight < 0.001) continue;
    const clip = clips[c]!;
    const f = Math.min(clip.frames, Math.max(0, phases[c]!));
    const fraction = f % 1;
    const start = Math.floor(f) * packedWidth;
    blendPackedFrames(out, offset, clip.data, start, packedWidth, bones, weight * (1 - fraction), weight * fraction);
  }
}
