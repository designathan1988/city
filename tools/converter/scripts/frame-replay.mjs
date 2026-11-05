// REPLAY of a captured frame (scripts/frame-capture.mjs): each draw alone, by WebGL (the reference: three's own GL
// commands, re-issued) and by Godot (the program translated by src/render/godot-shader.ts with the options the
// draw's GL state gives, the mesh built the way the translation places its attributes, the camera from three's
// matrices, the textures three's GPU held, every uniform three set), then compared pixel by pixel.
// Stages, each skipped when its output is newer than its input (fast reruns): select → translate (worker pool,
// cached) → webgl → godot (watched live, one item per draw) → compare.
// `node scripts/frame-replay.mjs <capture dir> <out dir> <playwright dir> <glsl dir> [--per-program <n>] [--size <px>]`
import crypto from 'node:crypto';
import fs from 'node:fs';
import http from 'node:http';
import path from 'node:path';
import { execFileSync } from 'node:child_process';
import { createRequire } from 'node:module';
import { judge } from './replay-judge.mjs';

const [capDir, outDir, pwDir, glslDir] = process.argv.slice(2);
const flag = (n, d) => { const i = process.argv.indexOf(n); return i >= 0 ? process.argv[i + 1] : d; };
const perProgram = Number(flag('--per-program', '2'));
// the replay size: the draw's own viewport by default (no resampling: every fragment at three's own position);
// --size <px> scales the longest side down, for a quick look
const SIZE = Number(flag('--size', '0'));
const only = flag('--only', null);
// a probe: the fragment main's last statement writes vec4(<expr>, 1.0) in both engines (bisecting a difference)
const probe = flag('--probe', null);
// the reference's graphics API: ANGLE on Vulkan, Godot's own (the same GPU then filters textures the same way:
// D3D11, Chrome's default on Windows, filters anisotropic mips differently); --angle d3d11 for the browser default
const ANGLE = flag('--angle', 'vulkan');
const here = path.resolve(import.meta.dirname, '..');
const t0 = Date.now();
const say = (m) => console.log(`[${((Date.now() - t0) / 1000).toFixed(1)} s] ${m}`);
const index = JSON.parse(fs.readFileSync(path.join(capDir, 'index.json'), 'utf8'));
// the fragment's colour output: a GLSL 3 `out vec4` (location 0, or the only one), else gl_FragColor
const outputOf = (f) => (/layout\s*\(\s*location\s*=\s*0\s*\)\s*out\s+(?:(?:highp|mediump|lowp)\s+)?vec4\s+(\w+)\s*;/.exec(f) ?? /(?:^|\n)\s*out\s+(?:(?:highp|mediump|lowp)\s+)?vec4\s+(\w+)\s*;/.exec(f))?.[1] ?? 'gl_FragColor';
// code run after the whole fragment main, early returns included: main renamed, a new main calls it, then the code
const afterMain = (f, code) => {
  const re = /\bvoid\s+main\s*\(\s*(?:void)?\s*\)/;
  if (!re.test(f)) return f;
  return `${f.replace(re, 'void _three_replay_main()')}\nvoid main() {\n\t_three_replay_main();\n\t${code}\n}\n`;
};
// a probe reads main's locals, so it stays inside main, before its closing brace (an early return skips it)
if (probe) for (const p of Object.values(index.programs)) { const k = p.fragment.lastIndexOf('}'); p.fragment = `${p.fragment.slice(0, k)}\t${outputOf(p.fragment)} = vec4(${probe}, 1.0);\n}\n`; }
// a program whose every draw writes no colour (colorMask off: a shadow map's depth pass) is judged by its depth:
// both engines write gl_FragCoord.z as a 24-bit fixed-point code in RGB (exact), decoded and compared numerically
const depthOnly = new Set();
for (const [id, p] of Object.entries(index.programs)) {
  const ds = index.draws.filter((d) => String(d.program) === String(id));
  if (!probe && ds.length > 0 && ds.every((d) => !d.state.colorMask.some(Boolean))) {
    depthOnly.add(Number(id));
    p.fragment = afterMain(p.fragment, `{ float e = floor(clamp(gl_FragCoord.z, 0.0, 1.0) * 16777215.0 + 0.5); ${outputOf(p.fragment)} = vec4(floor(e / 65536.0), mod(floor(e / 256.0), 256.0), mod(e, 256.0), 255.0) / 255.0; }`);
  }
}
// the target's kind: 'color8' (bytes), 'half' (a half-float target, three's HalfFloatType: the precision of Godot's
// HDR viewport, RGBAH, so compared as half floats directly), 'bits32' (a float32 target: Godot's viewports store at
// most half floats, so the 16 bytes of each pixel's four words are written three bytes a pass, six passes, through the
// opaque pipeline — a byte k/255 survives a half float — and the reference reads the float target itself)
const RGBA32F = 34836, RGB32F = 34837, R32F = 33326, RG32F = 33328;
const kindOf = (d) => (d.target?.componentType !== 5126 ? 'color8' : [RGBA32F, RGB32F, R32F, RG32F].includes(d.target.format) && !d.state.blend ? 'bits32' : 'half');
const bytePass = (f, j) => {
  const o = outputOf(f);
  // byte b of the pixel: word b >> 2 (the component), from its top byte down
  const byte = (b) => (b < 16 ? `float((floatBitsToUint((${o})[${b >> 2}]) >> ${24 - 8 * (b & 3)}u) & 255u)` : '0.0');
  return afterMain(f, `${o} = vec4(${byte(3 * j)}, ${byte(3 * j + 1)}, ${byte(3 * j + 2)}, 255.0) / 255.0;`);
};
fs.mkdirSync(outDir, { recursive: true });
const blob = (name) => fs.readFileSync(path.join(capDir, 'blobs', name));
const GL = { TRIANGLES: 4, TRIANGLE_STRIP: 5, TRIANGLE_FAN: 6, LINES: 1, LINE_STRIP: 3, POINTS: 0, BACK: 1029, FRONT: 1028, CCW: 2305, CW: 2304,
  NEAREST: 0x2600, LINEAR: 0x2601, NMN: 0x2700, LMN: 0x2701, NML: 0x2702, LML: 0x2703, REPEAT: 0x2901, CLAMP: 0x812f, MIRROR: 0x8370,
  ZERO: 0, ONE: 1, SRC_ALPHA: 770, ONE_MINUS_SRC_ALPHA: 771, SRC_COLOR: 768, DST_COLOR: 774, UNSIGNED_BYTE: 5121, UNSIGNED_SHORT: 5123, UNSIGNED_INT: 5125 };

// ---- select: every program, up to --per-program of its draws (the first ones), triangles, lines and points
const byProgram = new Map();
for (const d of index.draws) {
  if (only && !only.split(',').includes(String(d.index))) continue;
  if (!only && (byProgram.get(d.program) ?? []).length >= perProgram) continue;
  (byProgram.get(d.program) ?? byProgram.set(d.program, []).get(d.program)).push(d);
}
const draws = [...byProgram.values()].flat().sort((a, b) => a.index - b.index);
say(`${draws.length} draws of ${byProgram.size} programs selected (of ${index.draws.length} draws)`);

// ---- what the draw's GL state means for the Godot shader
const renderModes = (d) => {
  const s = d.state; const modes = [];
  if (!s.cull) modes.push('cull_disabled');
  else { const backIsBack = (s.cullFace === GL.BACK) === (s.frontFace === GL.CCW); modes.push(backIsBack ? 'cull_back' : 'cull_front'); }
  if (!s.depthTest) modes.push('depth_test_disabled');
  modes.push(s.depthMask ? 'depth_draw_always' : 'depth_draw_never');
  if (s.blend) {
    const key = `${s.blendSrc},${s.blendDst}`;
    const b = { [`${GL.SRC_ALPHA},${GL.ONE_MINUS_SRC_ALPHA}`]: 'blend_mix', [`${GL.ONE},${GL.ONE}`]: 'blend_add', [`${GL.SRC_ALPHA},${GL.ONE}`]: 'blend_add', [`${GL.ONE},${GL.ONE_MINUS_SRC_ALPHA}`]: 'blend_premul_alpha', [`${GL.ZERO},${GL.SRC_COLOR}`]: 'blend_mul', [`${GL.DST_COLOR},${GL.ZERO}`]: 'blend_mul' }[key];
    if (b) modes.push(b);
  }
  return modes;
};
const samplerHints = (d) => {
  const h = {};
  for (const sm of d.samplers) {
    const t = index.textures[sm.texture] ?? {};
    const min = sm.params?.min ?? GL.LINEAR;
    // anisotropy (three's texture.anisotropy, EXT_texture_filter_anisotropic): Godot's *_anisotropic filters, at
    // the level of the project setting (scripts/replay.gd sets it from anisotropyOf)
    const aniso = (sm.params?.anisotropy ?? 1) > 1 && min !== GL.NEAREST && min !== GL.LINEAR ? '_anisotropic' : '';
    const filter = min === GL.NEAREST ? 'filter_nearest' : min === GL.LINEAR ? 'filter_linear' : (min === GL.NMN || min === GL.NML) ? `filter_nearest_mipmap${aniso}` : `filter_linear_mipmap${aniso}`;
    const repeat = (sm.params?.wrapS ?? GL.REPEAT) === GL.CLAMP ? 'repeat_disable' : 'repeat_enable';
    const base = sm.name.replace(/\[\d+\]$/, '');
    // source_color for an sRGB texture: Godot then samples the sRGB view of its storage (decoded by the hardware,
    // like WebGL's SRGB8_ALPHA8; scripts/replay.gd makes the view shareable)
    h[base] = [...(t.srgb && !t.decoded ? ['source_color'] : []), filter, repeat];
  }
  return h;
};
// three's reversed depth buffer (EXT_clip_control zero to one): the depth test is GEQUAL/GREATER
const reversedOf = (d) => d.state.depthFunc === 518 || d.state.depthFunc === 516;
// an integer texture in Godot 4.7: RGBA16 uint when every value fits 16 bits (exact), else its 32-bit words in a
// float texture read back with floatBitsToUint/Int (exact: nearest sampling returns the bits)
const intRoutes = new Map();
const intRoute = (id) => {
  if (intRoutes.has(id)) return intRoutes.get(id);
  const t = index.textures[id];
  let route = 'bits32';
  if (t.kind === 'uint') {
    let fits = true;
    for (const b of t.blobs) { const B = blob(b.name); const U = new Uint32Array(B.buffer, B.byteOffset, B.length / 4); for (const x of U) if (x > 0xffff) { fits = false; break; } if (!fits) break; }
    if (fits) route = 'uint16';
  }
  intRoutes.set(id, route);
  return route;
};
const instancedOf = (d) => d.attributes.filter((a) => a.parts.some((p) => p.enabled && p.divisor > 0)).map((a) => a.name);

// ---- translate (the worker pool and its cache): one shader per program and options
const tDir = path.join(outDir, 'translate');
const jobs = []; const shaderOf = new Map();
for (const d of draws) {
  const p = index.programs[d.program];
  const shadow = d.samplers.find((sm) => sm.params && sm.params.compareMode === 34894);
  const cmp = shadow ? { 515: 'lequal', 518: 'gequal', 513: 'less', 516: 'greater' }[shadow.params.compareFunc] ?? 'lequal' : undefined;
  // three's matrices and its GL framebuffer orientation (the same rasterization as WebGL's offscreen framebuffers)
  const options = { renderModes: renderModes(d), samplerHints: samplerHints(d), matrices: 'three', orientation: 'gl', output: 'raw', depth: reversedOf(d) ? 'reversed' : 'standard',
    floatBitsSamplers: d.samplers.filter((sm) => ['uint', 'int'].includes(index.textures[sm.texture]?.kind) && intRoute(sm.texture) === 'bits32').map((sm) => sm.name.replace(/\[\d+\]$/, '')),
    ...(shadow ? { shadowCompare: cmp, shadowFilter: shadow.params.min === GL.NEAREST ? 'nearest' : 'linear' } : {}) };
  // the passes Godot draws for it. Blending on: one pass, ALPHA with three's blend mode (Godot's transparent pipeline
  // blends as three does). Blending off: Godot's transparent pipeline would still blend (it has no mode without), so
  // the colour goes through the opaque pipeline (it overwrites, as three) and the alpha through a second opaque pass
  // that stores it in the colour channels. A float32 target: six opaque passes of its bytes.
  const opaque = { ...options, alphaOutput: 'none', renderModes: options.renderModes.filter((m) => !m.startsWith('blend_')) };
  const kind = kindOf(d);
  const passes = kind === 'bits32' ? [0, 1, 2, 3, 4, 5].map((j) => ({ part: `b${j}`, fragment: bytePass(p.fragment, j), opts: opaque }))
    : d.state.blend ? [{ part: '', fragment: p.fragment, opts: { ...options, alphaOutput: 'blend' } }]
      : [{ part: '', fragment: p.fragment, opts: opaque }, { part: 'a', fragment: p.fragment, opts: { ...opaque, channels: 'alpha' } }];
  d.kind = kind;
  d.variants = [];
  for (const pass of passes) {
    const key = JSON.stringify([d.program, pass.part.startsWith('b') ? pass.part : '', pass.opts, instancedOf(d)]);
    if (!shaderOf.has(key)) { shaderOf.set(key, `p${d.program}_${shaderOf.size}`); jobs.push({ vertex: p.vertex, fragment: pass.fragment, instanced: instancedOf(d), options: pass.opts, name: shaderOf.get(key) }); }
    d.variants.push({ part: pass.part, shaderName: shaderOf.get(key) });
  }
  d.shaderName = d.variants[0].shaderName;
}
fs.writeFileSync(path.join(outDir, 'programs.json'), JSON.stringify(jobs));
say(`translating ${jobs.length} program variants (cached where already done)`);
execFileSync(process.execPath, [path.join(here, 'scripts', 'shaders.ts'), path.join(outDir, 'programs.json'), tDir, '--glsl', glslDir], { stdio: ['ignore', 'ignore', 'inherit'] });
const translation = new Map(JSON.parse(fs.readFileSync(path.join(tDir, 'translation.json'), 'utf8')).map((e, i) => [jobs[i].name, e]));
say('translated');

// ---- decode the captured attributes (every GL type), per vertex or per instance
const bpt = { 5126: 4, 5131: 2, 5121: 1, 5120: 1, 5123: 2, 5122: 2, 5125: 4, 5124: 4 };
const buffers = new Map();
const bufferOf = (id) => buffers.get(id) ?? buffers.set(id, blob(`buffer_${id}.bin`)).get(id);
const half = (h) => { const s = (h & 0x8000) ? -1 : 1, e = (h >> 10) & 0x1f, m = h & 0x3ff; return e === 0 ? s * 2 ** -14 * (m / 1024) : e === 31 ? (m ? NaN : s * Infinity) : s * 2 ** (e - 15) * (1 + m / 1024); };
const readPart = (part, count) => {
  if (!part.enabled) return Array.from({ length: count }, () => (part.current ?? [0, 0, 0, 1]).slice(0, part.size));
  const buf = bufferOf(part.buffer); const size = part.size; const stride = part.stride || size * bpt[part.type];
  const out = [];
  for (let i = 0; i < count; i++) {
    const v = [];
    for (let c = 0; c < size; c++) {
      const o = part.offset + i * stride + c * bpt[part.type];
      let x = 0;
      switch (part.type) {
        case 5126: x = buf.readFloatLE(o); break;
        case 5131: x = half(buf.readUInt16LE(o)); break;
        case 5121: x = buf.readUInt8(o); if (part.normalized && !part.integer) x /= 255; break;
        case 5120: x = buf.readInt8(o); if (part.normalized && !part.integer) x = Math.max(-1, x / 127); break;
        case 5123: x = buf.readUInt16LE(o); if (part.normalized && !part.integer) x /= 65535; break;
        case 5122: x = buf.readInt16LE(o); if (part.normalized && !part.integer) x = Math.max(-1, x / 32767); break;
        case 5125: x = buf.readUInt32LE(o); break;
        case 5124: x = buf.readInt32LE(o); break;
      }
      v.push(x);
    }
    out.push(v);
  }
  return out;
};

// ---- the Godot cases: a small JSON and float arrays in binary
const gDir = path.join(outDir, 'godot');
fs.rmSync(gDir, { recursive: true, force: true });
fs.mkdirSync(path.join(gDir, 'data'), { recursive: true });
fs.cpSync(path.join(tDir, 'shaders'), path.join(gDir, 'shaders'), { recursive: true });
const f32 = (name, arr) => { fs.writeFileSync(path.join(gDir, 'data', name), Buffer.from(new Float32Array(arr).buffer)); return name; };
const i32 = (name, arr) => { fs.writeFileSync(path.join(gDir, 'data', name), Buffer.from(new Int32Array(arr).buffer)); return name; };
const cases = [];
const skipped = [];
const inv4 = (m) => { // column-major 4x4 inverse
  const a = m, inv = new Array(16);
  inv[0] = a[5]*a[10]*a[15]-a[5]*a[11]*a[14]-a[9]*a[6]*a[15]+a[9]*a[7]*a[14]+a[13]*a[6]*a[11]-a[13]*a[7]*a[10];
  inv[4] = -a[4]*a[10]*a[15]+a[4]*a[11]*a[14]+a[8]*a[6]*a[15]-a[8]*a[7]*a[14]-a[12]*a[6]*a[11]+a[12]*a[7]*a[10];
  inv[8] = a[4]*a[9]*a[15]-a[4]*a[11]*a[13]-a[8]*a[5]*a[15]+a[8]*a[7]*a[13]+a[12]*a[5]*a[11]-a[12]*a[7]*a[9];
  inv[12] = -a[4]*a[9]*a[14]+a[4]*a[10]*a[13]+a[8]*a[5]*a[14]-a[8]*a[6]*a[13]-a[12]*a[5]*a[10]+a[12]*a[6]*a[9];
  inv[1] = -a[1]*a[10]*a[15]+a[1]*a[11]*a[14]+a[9]*a[2]*a[15]-a[9]*a[3]*a[14]-a[13]*a[2]*a[11]+a[13]*a[3]*a[10];
  inv[5] = a[0]*a[10]*a[15]-a[0]*a[11]*a[14]-a[8]*a[2]*a[15]+a[8]*a[3]*a[14]+a[12]*a[2]*a[11]-a[12]*a[3]*a[10];
  inv[9] = -a[0]*a[9]*a[15]+a[0]*a[11]*a[13]+a[8]*a[1]*a[15]-a[8]*a[3]*a[13]-a[12]*a[1]*a[11]+a[12]*a[3]*a[9];
  inv[13] = a[0]*a[9]*a[14]-a[0]*a[10]*a[13]-a[8]*a[1]*a[14]+a[8]*a[2]*a[13]+a[12]*a[1]*a[10]-a[12]*a[2]*a[9];
  inv[2] = a[1]*a[6]*a[15]-a[1]*a[7]*a[14]-a[5]*a[2]*a[15]+a[5]*a[3]*a[14]+a[13]*a[2]*a[7]-a[13]*a[3]*a[6];
  inv[6] = -a[0]*a[6]*a[15]+a[0]*a[7]*a[14]+a[4]*a[2]*a[15]-a[4]*a[3]*a[14]-a[12]*a[2]*a[7]+a[12]*a[3]*a[6];
  inv[10] = a[0]*a[5]*a[15]-a[0]*a[7]*a[13]-a[4]*a[1]*a[15]+a[4]*a[3]*a[13]+a[12]*a[1]*a[7]-a[12]*a[3]*a[5];
  inv[14] = -a[0]*a[5]*a[14]+a[0]*a[6]*a[13]+a[4]*a[1]*a[14]-a[4]*a[2]*a[13]-a[12]*a[1]*a[6]+a[12]*a[2]*a[5];
  inv[3] = -a[1]*a[6]*a[11]+a[1]*a[7]*a[10]+a[5]*a[2]*a[11]-a[5]*a[3]*a[10]-a[9]*a[2]*a[7]+a[9]*a[3]*a[6];
  inv[7] = a[0]*a[6]*a[11]-a[0]*a[7]*a[10]-a[4]*a[2]*a[11]+a[4]*a[3]*a[10]+a[8]*a[2]*a[7]-a[8]*a[3]*a[6];
  inv[11] = -a[0]*a[5]*a[11]+a[0]*a[7]*a[9]+a[4]*a[1]*a[11]-a[4]*a[3]*a[9]-a[8]*a[1]*a[7]+a[8]*a[3]*a[5];
  inv[15] = a[0]*a[5]*a[10]-a[0]*a[6]*a[9]-a[4]*a[1]*a[10]+a[4]*a[2]*a[9]+a[8]*a[1]*a[6]-a[8]*a[2]*a[5];
  const det = a[0]*inv[0]+a[1]*inv[4]+a[2]*inv[8]+a[3]*inv[12];
  return inv.map((x) => x / det);
};
const mul4 = (a, b) => { const r = new Array(16).fill(0); for (let c = 0; c < 4; c++) for (let rr = 0; rr < 4; rr++) for (let k = 0; k < 4; k++) r[c * 4 + rr] += a[k * 4 + rr] * b[c * 4 + k]; return r; };
for (const d of draws) {
  const tr = translation.get(d.shaderName);
  if (!tr || tr.error) { skipped.push({ index: d.index, why: `translation: ${tr?.error ?? 'missing'}` }); continue; }
  if (![GL.TRIANGLES, GL.TRIANGLE_STRIP, GL.TRIANGLE_FAN, GL.LINES, GL.POINTS].includes(d.mode)) { skipped.push({ index: d.index, why: `primitive ${d.mode}` }); continue; }
  // indices
  let indices;
  if (d.elements) {
    const buf = bufferOf(d.elements); const n = d.count; const sz = d.type === GL.UNSIGNED_INT ? 4 : d.type === GL.UNSIGNED_SHORT ? 2 : 1;
    indices = Array.from({ length: n }, (_, i) => (sz === 4 ? buf.readUInt32LE(d.offset + i * 4) : sz === 2 ? buf.readUInt16LE(d.offset + i * 2) : buf.readUInt8(d.offset + i)));
  } else indices = Array.from({ length: d.count }, (_, i) => d.offset + i);
  // strips and fans as lists (exact), then every triangle reversed (three's front faces are CCW, Godot's CW)
  let tris = indices;
  if (d.mode === GL.TRIANGLE_STRIP) { tris = []; for (let i = 0; i + 2 < indices.length; i++) tris.push(...(i % 2 === 0 ? [indices[i], indices[i + 1], indices[i + 2]] : [indices[i + 1], indices[i], indices[i + 2]])); }
  if (d.mode === GL.TRIANGLE_FAN) { tris = []; for (let i = 1; i + 1 < indices.length; i++) tris.push(indices[0], indices[i], indices[i + 1]); }
  const triangles = d.mode !== GL.LINES && d.mode !== GL.POINTS;
  const godotIndex = [];
  // in GL orientation (the translation keeps three's clip y) the triangles keep three's order: the y flip and the
  // index reversal of Godot's orientation cancel each other, so neither is done
  const glOrientation = translation.get(d.shaderName)?.notes?.some((n) => n.startsWith('orientation gl')) ?? false;
  if (triangles && !glOrientation) for (let i = 0; i + 2 < tris.length; i += 3) godotIndex.push(tris[i], tris[i + 2], tris[i + 1]); else for (const x of tris) godotIndex.push(x);
  let vertexCount = 0; for (const x of indices) if (x + 1 > vertexCount) vertexCount = x + 1;
  const instanceCount = d.instances;
  const arrays = {}; const customs = [null, null, null, null]; const textures = []; let bones = null, weights = null; let instanceXf = null; let instanceColor = null; let instanceCustom = null;
  const attrByName = new Map(d.attributes.map((a) => [a.name, a]));
  const perInstance = (a) => a.parts.some((p) => p.enabled && p.divisor > 0);
  const valuesOf = (a) => { const n = perInstance(a) ? instanceCount : vertexCount; const cols = a.parts.map((p) => readPart(p, n)); return Array.from({ length: n }, (_, i) => cols.flatMap((c) => c[i])); };
  for (const m of tr.attributes) {
    const a = attrByName.get(m.three);
    if (!a) continue;
    const vals = valuesOf(a);
    let mm;
    if (m.godot === 'VERTEX') arrays.vertex = f32(`${d.index}_vertex.bin`, vals.flatMap((v) => v.slice(0, 3)));
    else if (m.godot === 'NORMAL') arrays.normal = f32(`${d.index}_normal.bin`, vals.flatMap((v) => v.slice(0, 3)));
    else if (m.godot === 'UV') arrays.uv = f32(`${d.index}_uv.bin`, vals.flatMap((v) => v.slice(0, 2)));
    else if (m.godot === 'UV2') arrays.uv2 = f32(`${d.index}_uv2.bin`, vals.flatMap((v) => v.slice(0, 2)));
    // three's instanceColor with a vertex color too: the translation reads the product from COLOR (Godot's MultiMesh
    // multiplies the instance colour into it, as three's shader multiplies the two): the instance colours still go in
    else if (m.three === 'instanceColor' && m.godot === 'vec3(1.0)') instanceColor = vals.map((v) => [v[0], v[1], v[2], 1]);
    else if (m.godot.startsWith('COLOR')) { if (perInstance(a)) instanceColor = vals.map((v) => [v[0], v[1], v[2], v[3] ?? 1]); else arrays.color = f32(`${d.index}_color.bin`, vals.flatMap((v) => [v[0], v[1], v[2], v[3] ?? 1])); }
    else if (m.godot.startsWith('vec4(TANGENT')) arrays.tangent = f32(`${d.index}_tangent.bin`, vals.flatMap((v) => v.slice(0, 4)));
    else if (m.godot === '_three_instanceMatrix') instanceXf = vals;
    else if (m.godot === 'vec4(BONE_INDICES)') bones = i32(`${d.index}_bones.bin`, vals.flatMap((v) => v.slice(0, 4)));
    else if (m.godot === 'BONE_WEIGHTS') weights = f32(`${d.index}_weights.bin`, vals.flatMap((v) => v.slice(0, 4)));
    else if ((mm = /^CUSTOM(\d)\.([xyzw]+)$/.exec(m.godot))) {
      const k = Number(mm[1]); const at = 'xyzw'.indexOf(mm[2][0]);
      customs[k] ??= new Array(vertexCount * 4).fill(0);
      vals.forEach((v, i) => v.forEach((x, j) => { customs[k][i * 4 + at + j] = x; }));
    } else if ((mm = /^INSTANCE_CUSTOM\.([xyzw]+)$/.exec(m.godot))) {
      const at = 'xyzw'.indexOf(mm[1][0]);
      instanceCustom ??= Array.from({ length: instanceCount }, () => [0, 0, 0, 0]);
      vals.forEach((v, i) => v.forEach((x, j) => { instanceCustom[i][at + j] = x; }));
    } else if ((mm = /texelFetch\((_three_attr_\w+), ivec2\((VERTEX_ID|INSTANCE_ID)/.exec(m.godot))) {
      const count = vals.length; const w = Math.min(4096, count), h = Math.ceil(count / 4096);
      const data = new Float32Array(w * h * 4); vals.forEach((v, i) => v.forEach((x, j) => { data[i * 4 + j] = x; }));
      textures.push({ uniform: mm[1], kind: 'float', layers: 1, levels: [{ width: w, height: h, file: f32(`${d.index}_${mm[1]}.bin`, Array.from(data)) }] });
    }
  }
  const customFiles = customs.map((c, k) => (c ? f32(`${d.index}_custom${k}.bin`, c) : null));
  // textures three's samplers held
  const byThree = new Map(tr.uniforms.map((u) => [u.three, u]));
  for (const sm of d.samplers) {
    const t = index.textures[sm.texture];
    if (!t || t.error || !t.blobs) { skipped.push({ index: d.index, why: `texture ${sm.name}: ${t?.error ?? 'not read'}` }); continue; }
    const base = sm.name.replace(/\[\d+\]$/, '');
    const u = byThree.get(base);
    const levels = [];
    for (const b of t.blobs.filter((x) => x.layer === 0)) levels.push({ width: b.width, height: b.height, files: t.blobs.filter((x) => x.level === b.level).sort((x, y) => x.layer - y.layer).map((x) => x.name) });
    const element = /\[(\d+)\]$/.exec(sm.name);
    let kind = t.decoded || t.kind === 'depth' ? 'float' : t.kind;
    let capture = true;
    if (t.kind === 'uint' || t.kind === 'int') {
      if (intRoute(sm.texture) === 'bits32') kind = 'float'; // the same bytes, as float words
      else {
        kind = 'uint16'; capture = false;
        for (const l of levels) l.files = l.files.map((name) => {
          const to = `${name}.u16`; const B = blob(name); const U = new Uint32Array(B.buffer, B.byteOffset, B.length / 4);
          if (!fs.existsSync(path.join(gDir, 'data', to))) fs.writeFileSync(path.join(gDir, 'data', to), Buffer.from(Uint16Array.from(U).buffer));
          return to;
        });
      }
    }
    textures.push({ uniform: u?.godot ?? base, kind, srgb: !!t.srgb && !t.decoded, layers: t.layers, levels, capture, array: sm.target === 35866, ...(element ? { element: Number(element[1]) } : {}) });
  }
  // uniforms three set, under the Godot names
  const params = new Map();
  const width = { float: 1, int: 1, uint: 1, bool: 1, vec2: 2, vec3: 3, vec4: 4, ivec2: 2, ivec3: 3, ivec4: 4, uvec2: 2, uvec3: 3, uvec4: 4, mat2: 4, mat3: 9, mat4: 16 };
  for (const [name, v] of Object.entries(d.uniforms)) {
    if (!v.data) continue;
    const m = /^(\w+)(?:\[(\d+)\])?(?:\.(\w+))?$/.exec(name);
    if (!m) continue;
    const [, root, idx, field] = m;
    const u = field ? byThree.get(`${root}[].${field}`) ?? byThree.get(`${root}.${field}`) : byThree.get(root);
    if (!u || /sampler/.test(u.type)) continue;
    const e = params.get(u.godot) ?? params.set(u.godot, { godot: u.godot, type: u.type, array: u.array !== '', elements: [] }).get(u.godot);
    const w = width[u.type] ?? 1;
    for (let k = 0; k * w < v.data.length; k++) e.elements[Number(idx ?? 0) + k] = v.data.slice(k * w, k * w + w);
  }
  for (const e of params.values()) for (let k = 0; k < e.elements.length; k++) e.elements[k] ??= new Array(width[e.type] ?? 1).fill(0);
  // the camera and the object, from three's matrices (Godot's built-ins then equal them)
  const U = (n) => d.uniforms[n]?.data ?? null;
  const proj = U('projectionMatrix'); const view = U('viewMatrix'); const model = U('modelMatrix') ?? (view && U('modelViewMatrix') ? mul4(inv4(view), U('modelViewMatrix')) : null);
  const vw = d.viewport[2], vh = d.viewport[3];
  const scale = SIZE > 0 ? Math.min(1, SIZE / Math.max(vw, vh)) : 1;
  const theCase = {
    // the viewport anisotropy (Viewport.AnisotropicFiltering: 0 off, 1 2x .. 4 16x): its textures' level
    anisotropy: Math.round(Math.log2(Math.min(16, Math.max(1, ...d.samplers.map((sm) => sm.params?.anisotropy ?? 1))))),
    index: d.index, id: String(d.index), orientation: glOrientation ? 'gl' : 'godot', shader: `res://shaders/${tr.file}`, size: [Math.max(1, Math.round(vw * scale)), Math.max(1, Math.round(vh * scale))],
    primitive: triangles ? 'triangles' : d.mode === GL.LINES ? 'lines' : 'points',
    arrays, customs: customFiles, bones, weights, indices: i32(`${d.index}_index.bin`, godotIndex), vertexCount,
    // every instanced draw is a MultiMesh (instance count and per-instance data), with three's instanceMatrix when
    // it has one, else the identity (its MODEL_MATRIX is then what three's shader expects: nothing)
    instances: instanceXf || instanceCount > 1 || instanceColor || instanceCustom
      ? Array.from({ length: Math.max(1, instanceCount) }, (_, i) => ({ matrix: instanceXf?.[i] ?? [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1], color: instanceColor?.[i] ?? [1, 1, 1, 1], custom: instanceCustom?.[i] ?? [0, 0, 0, 0] }))
      : null,
    textures, params: [...params.values()], projection: proj, view, model, target: d.target,
  };
  // one case per pass (id <draw><part>: '' the colour, 'a' the alpha, 'b0'..'b5' the bytes of a float32 target);
  // `bits`: Godot writes the image as floats (half targets and byte passes), else as RGBA8
  for (const v of d.variants) {
    const tv = translation.get(v.shaderName);
    if (!tv || tv.error) { skipped.push({ index: d.index, why: `translation of pass '${v.part}': ${tv?.error ?? 'missing'}` }); continue; }
    cases.push({ ...theCase, id: `${d.index}${v.part}`, part: v.part, kind: d.kind, shader: `res://shaders/${tv.file}`, bits: d.kind !== 'color8' });
  }
}
fs.writeFileSync(path.join(gDir, 'cases.json'), JSON.stringify(cases));
// what the judge needs beyond the cases (scripts/replay-compare.mjs judges the C++ port's images the same way)
fs.writeFileSync(path.join(outDir, 'judge.json'), JSON.stringify({ depthOnly: [...depthOnly], programs: Object.fromEntries(draws.map((d) => [d.index, d.program])) }));
say(`${cases.length} Godot cases written; ${skipped.length} skipped`);
fs.writeFileSync(path.join(outDir, 'skipped.json'), JSON.stringify(skipped, null, 1));

// ---- the WebGL reference: each draw alone, from the captured state, read back
const server = http.createServer((req, res) => {
  const u = decodeURIComponent((req.url ?? '/').split('?')[0]);
  if (u === '/') { res.setHeader('content-type', 'text/html'); res.end('<!doctype html><canvas id=c></canvas>'); return; }
  if (u === '/cap/index.json') { res.setHeader('content-type', 'application/json'); res.end(JSON.stringify(index)); return; }
  if (u.startsWith('/cap/')) { const f = path.join(capDir, u.slice(5)); if (fs.existsSync(f)) { res.end(fs.readFileSync(f)); return; } }
  if (u.startsWith('/out/') && req.method === 'POST') { const chunks = []; req.on('data', (c) => chunks.push(c)); req.on('end', () => { fs.writeFileSync(path.join(outDir, 'webgl', u.slice(5)), Buffer.concat(chunks)); res.end('ok'); }); return; }
  res.statusCode = 404; res.end();
});
// the reference is cached: it depends only on the capture (as probed), the draws, their sizes and this page code
const refCode = fs.readFileSync(new URL(import.meta.url), 'utf8');
const refKey = crypto.createHash('sha256').update(ANGLE).update(refCode.slice(refCode.indexOf('// ---- the WebGL reference'), refCode.indexOf('// ---- Godot: the cases replayed'))).update(fs.readFileSync(path.join(capDir, 'index.json'))).update(JSON.stringify(index.programs)).update(JSON.stringify(cases.map((c) => [c.index, c.size]))).digest('hex');
const refKeyFile = path.join(outDir, 'webgl.key');
let webgl;
if (fs.existsSync(refKeyFile) && fs.readFileSync(refKeyFile, 'utf8') === refKey && fs.existsSync(path.join(outDir, 'webgl.json'))) {
  webgl = JSON.parse(fs.readFileSync(path.join(outDir, 'webgl.json'), 'utf8'));
  say(`WebGL reference: cached (${webgl.filter((r) => r.ok).length}/${webgl.length} valid)`);
} else {
  fs.rmSync(path.join(outDir, 'webgl'), { recursive: true, force: true });
  fs.mkdirSync(path.join(outDir, 'webgl'), { recursive: true });
  await new Promise((r) => server.listen(0, '127.0.0.1', r));
  const base = `http://127.0.0.1:${server.address().port}/`;
  const { chromium } = createRequire(path.join(pwDir, 'package.json'))('playwright');
  const browser = await chromium.launch({ channel: 'chrome', headless: true, args: ['--enable-gpu', '--ignore-gpu-blocklist', `--use-angle=${ANGLE}`, ...(ANGLE === 'vulkan' ? ['--enable-features=Vulkan'] : [])] });
  const page = await browser.newPage();
  await page.goto(base);
  webgl = await page.evaluate(async ({ selected, sizes }) => {
    const index = await (await fetch('/cap/index.json')).json();
    const canvas = document.getElementById('c'); const gl = canvas.getContext('webgl2', { antialias: false, preserveDrawingBuffer: true });
    gl.getExtension('EXT_color_buffer_float'); gl.getExtension('OES_texture_float_linear'); const clip = gl.getExtension('EXT_clip_control'); const aniso = gl.getExtension('EXT_texture_filter_anisotropic');
    // every texture is created on a unit no sampler uses (a bindTexture on a sampler's unit would replace its texture)
    const SCRATCH = gl.TEXTURE0 + gl.getParameter(gl.MAX_COMBINED_TEXTURE_IMAGE_UNITS) - 1;
    const blobs = new Map(); const get = async (n) => blobs.get(n) ?? blobs.set(n, new Uint8Array(await (await fetch('/cap/blobs/' + n)).arrayBuffer())).get(n);
    const programs = new Map(); const buffers = new Map(); const textures = new Map();
    const results = [];
    for (const di of selected) {
      const d = index.draws[di];
      while (gl.getError() !== gl.NO_ERROR) { /* the previous draw's, already reported */ }
      try {
        let p = programs.get(d.program);
        if (!p) {
          const s = index.programs[d.program];
          const mk = (t, src) => { const x = gl.createShader(t); gl.shaderSource(x, src); gl.compileShader(x); if (!gl.getShaderParameter(x, gl.COMPILE_STATUS)) throw new Error(gl.getShaderInfoLog(x)); return x; };
          p = gl.createProgram(); gl.attachShader(p, mk(gl.VERTEX_SHADER, s.vertex)); gl.attachShader(p, mk(gl.FRAGMENT_SHADER, s.fragment));
          // the captured attribute locations
          for (const a of d.attributes) gl.bindAttribLocation(p, a.parts[0].location, a.name);
          gl.linkProgram(p); if (!gl.getProgramParameter(p, gl.LINK_STATUS)) throw new Error(gl.getProgramInfoLog(p));
          programs.set(d.program, p);
        }
        gl.useProgram(p);
        // uniforms by type
        for (const [name, v] of Object.entries(d.uniforms)) {
          const loc = gl.getUniformLocation(p, name); if (!loc || !v.data) continue;
          const T = v.type, x = v.data;
          if (T === gl.FLOAT) gl.uniform1fv(loc, x); else if (T === gl.FLOAT_VEC2) gl.uniform2fv(loc, x); else if (T === gl.FLOAT_VEC3) gl.uniform3fv(loc, x); else if (T === gl.FLOAT_VEC4) gl.uniform4fv(loc, x);
          else if (T === gl.FLOAT_MAT2) gl.uniformMatrix2fv(loc, false, x); else if (T === gl.FLOAT_MAT3) gl.uniformMatrix3fv(loc, false, x); else if (T === gl.FLOAT_MAT4) gl.uniformMatrix4fv(loc, false, x);
          else if (T === gl.UNSIGNED_INT) gl.uniform1uiv(loc, x); else if (T === gl.UNSIGNED_INT_VEC2) gl.uniform2uiv(loc, x); else if (T === gl.UNSIGNED_INT_VEC3) gl.uniform3uiv(loc, x); else if (T === gl.UNSIGNED_INT_VEC4) gl.uniform4uiv(loc, x);
          else if (T === gl.INT_VEC2 || T === gl.BOOL_VEC2) gl.uniform2iv(loc, x); else if (T === gl.INT_VEC3 || T === gl.BOOL_VEC3) gl.uniform3iv(loc, x); else if (T === gl.INT_VEC4 || T === gl.BOOL_VEC4) gl.uniform4iv(loc, x);
          else gl.uniform1iv(loc, x); // int, bool, samplers (their units)
        }
        // textures, as captured (level by level)
        const bind = [];
        for (const sm of d.samplers) {
          const t = index.textures[sm.texture];
          let tex = textures.get(sm.texture);
          if (!tex && t && t.blobs) {
            tex = gl.createTexture(); const target = sm.target; gl.activeTexture(SCRATCH); gl.bindTexture(target, tex);
            // a depth texture is a depth texture again (a shadow sampler on any other is INVALID_OPERATION at the draw):
            // DEPTH_COMPONENT32F holding the captured values (the R of the decoded copy)
            const isDepth = t.kind === 'depth';
            const f = isDepth ? [gl.DEPTH_COMPONENT32F, gl.DEPTH_COMPONENT, gl.FLOAT]
              : t.decoded || t.kind === 'float' || t.kind === 'compressed' ? [gl.RGBA32F, gl.RGBA, gl.FLOAT] : t.kind === 'uint' ? [gl.RGBA32UI, gl.RGBA_INTEGER, gl.UNSIGNED_INT] : t.kind === 'int' ? [gl.RGBA32I, gl.RGBA_INTEGER, gl.INT] : [t.srgb ? gl.SRGB8_ALPHA8 : gl.RGBA8, gl.RGBA, gl.UNSIGNED_BYTE];
            const levels = Math.max(...t.blobs.map((b) => b.level)) + 1;
            if (target === gl.TEXTURE_2D_ARRAY) gl.texStorage3D(target, levels, f[0], t.width, t.height, t.layers); else gl.texStorage2D(target, levels, f[0], t.width, t.height);
            for (const b of t.blobs) {
              const raw = await get(b.name);
              let view = f[2] === gl.FLOAT ? new Float32Array(raw.buffer) : f[2] === gl.UNSIGNED_INT ? new Uint32Array(raw.buffer) : f[2] === gl.INT ? new Int32Array(raw.buffer) : raw;
              if (isDepth) { const r = new Float32Array(b.width * b.height); for (let k = 0; k < r.length; k++) r[k] = view[k * 4]; view = r; }
              if (target === gl.TEXTURE_2D_ARRAY) gl.texSubImage3D(target, b.level, 0, 0, b.layer, b.width, b.height, 1, f[1], f[2], view); else gl.texSubImage2D(target, b.level, 0, 0, b.width, b.height, f[1], f[2], view);
            }
            textures.set(sm.texture, tex);
          }
          bind.push([sm, tex ?? null, t]);
        }
        // vertex arrays from the captured buffers
        const vao = gl.createVertexArray(); gl.bindVertexArray(vao);
        const glBuf = async (id) => { let b = buffers.get(id); if (!b) { b = gl.createBuffer(); gl.bindBuffer(gl.ARRAY_BUFFER, b); gl.bufferData(gl.ARRAY_BUFFER, await get(`buffer_${id}.bin`), gl.STATIC_DRAW); buffers.set(id, b); } return b; };
        for (const a of d.attributes) for (const pt of a.parts) {
          if (!pt.enabled) { gl.disableVertexAttribArray(pt.location); if (pt.current) gl.vertexAttrib4fv(pt.location, pt.current); continue; }
          gl.bindBuffer(gl.ARRAY_BUFFER, await glBuf(pt.buffer));
          gl.enableVertexAttribArray(pt.location);
          if (pt.integer) gl.vertexAttribIPointer(pt.location, pt.size, pt.type, pt.stride, pt.offset); else gl.vertexAttribPointer(pt.location, pt.size, pt.type, pt.normalized, pt.stride, pt.offset);
          gl.vertexAttribDivisor(pt.location, pt.divisor);
        }
        if (d.elements) { const eb = gl.createBuffer(); gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, eb); gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, await get(`buffer_${d.elements}.bin`), gl.STATIC_DRAW); }
        // the target: colour RGBA8 and a depth texture, cleared, at the replay size
        const [w, h] = sizes[di];
        canvas.width = w; canvas.height = h;
        gl.activeTexture(SCRATCH);
        const fb = gl.createFramebuffer(); gl.bindFramebuffer(gl.FRAMEBUFFER, fb);
        // the target's kind, as three's was: a float target (its values unclamped, in float32) or a byte target
        const isFloat = d.target?.componentType === gl.FLOAT;
        // a float target at three's precision: half (RGBA16F and the other 16-bit float formats) or float32
        const half = isFloat && ![gl.RGBA32F, gl.RGB32F, gl.R32F, gl.RG32F].includes(d.target.format);
        const color = gl.createTexture(); gl.bindTexture(gl.TEXTURE_2D, color); gl.texStorage2D(gl.TEXTURE_2D, 1, isFloat ? (half ? gl.RGBA16F : gl.RGBA32F) : gl.RGBA8, w, h); gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, color, 0);
        const depth = gl.createTexture(); gl.bindTexture(gl.TEXTURE_2D, depth); gl.texStorage2D(gl.TEXTURE_2D, 1, gl.DEPTH_COMPONENT32F, w, h); gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.TEXTURE_2D, depth, 0);
        if (d.target.outputs === 2) { const c1 = gl.createTexture(); gl.bindTexture(gl.TEXTURE_2D, c1); gl.texStorage2D(gl.TEXTURE_2D, 1, gl.RGBA8, w, h); gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT1, gl.TEXTURE_2D, c1, 0); gl.drawBuffers([gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1]); }
        gl.viewport(0, 0, w, h); gl.disable(gl.SCISSOR_TEST); gl.colorMask(true, true, true, true); gl.depthMask(true);
        const rev = d.state.depthFunc === gl.GEQUAL || d.state.depthFunc === gl.GREATER;
        if (clip) clip.clipControlEXT(clip.LOWER_LEFT_EXT, rev ? clip.ZERO_TO_ONE_EXT : clip.NEGATIVE_ONE_TO_ONE_EXT);
        gl.clearColor(0, 0, 0, 0); gl.clearDepth(rev ? 0 : 1); gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        const s = d.state; const en = (cap, on) => (on ? gl.enable(cap) : gl.disable(cap));
        en(gl.BLEND, s.blend); gl.blendFuncSeparate(s.blendSrc, s.blendDst, s.blendSrcA, s.blendDstA); gl.blendEquation(s.blendEq);
        en(gl.DEPTH_TEST, s.depthTest); gl.depthFunc(s.depthFunc); gl.depthMask(s.depthMask); en(gl.CULL_FACE, s.cull); gl.cullFace(s.cullFace); gl.frontFace(s.frontFace);
        gl.colorMask(true, true, true, true); en(gl.POLYGON_OFFSET_FILL, false);
        for (const [sm, tex, t] of bind) {
          gl.activeTexture(gl.TEXTURE0 + sm.unit); gl.bindTexture(sm.target, tex);
          if (tex && sm.params) {
            gl.texParameteri(sm.target, gl.TEXTURE_MIN_FILTER, sm.params.min); gl.texParameteri(sm.target, gl.TEXTURE_MAG_FILTER, sm.params.mag);
            gl.texParameteri(sm.target, gl.TEXTURE_WRAP_S, sm.params.wrapS); gl.texParameteri(sm.target, gl.TEXTURE_WRAP_T, sm.params.wrapT);
            gl.texParameteri(sm.target, gl.TEXTURE_COMPARE_MODE, sm.params.compareMode); gl.texParameteri(sm.target, gl.TEXTURE_COMPARE_FUNC, sm.params.compareFunc);
            // the rest of three's sampler state, when the capture has it
            if (sm.params.anisotropy !== undefined && aniso) gl.texParameterf(sm.target, aniso.TEXTURE_MAX_ANISOTROPY_EXT, sm.params.anisotropy);
            if (sm.params.minLod !== undefined) { gl.texParameterf(sm.target, gl.TEXTURE_MIN_LOD, sm.params.minLod); gl.texParameterf(sm.target, gl.TEXTURE_MAX_LOD, sm.params.maxLod); }
            if (sm.params.wrapR !== undefined) gl.texParameteri(sm.target, gl.TEXTURE_WRAP_R, sm.params.wrapR);
          }
        }
        // GL errors before the draw are the replay's own (named), at the draw the reference is void
        const setupError = gl.getError();
        if (d.elements) { if (d.instances > 1) gl.drawElementsInstanced(d.mode, d.count, d.type, d.offset, d.instances); else gl.drawElements(d.mode, d.count, d.type, d.offset); }
        else if (d.instances > 1) gl.drawArraysInstanced(d.mode, d.offset, d.count, d.instances); else gl.drawArrays(d.mode, d.offset, d.count);
        const drawError = gl.getError();
        const px = isFloat ? new Float32Array(w * h * 4) : new Uint8Array(w * h * 4);
        gl.readPixels(0, 0, w, h, gl.RGBA, isFloat ? gl.FLOAT : gl.UNSIGNED_BYTE, px);
        const readError = gl.getError();
        const valid = setupError === 0 && drawError === 0 && readError === 0;
        if (valid) await fetch(`/out/${di}.${isFloat ? 'f32' : 'rgba'}`, { method: 'POST', body: px });
        results.push({ index: di, ok: valid, setupError, drawError, readError, clipControl: !!clip });
        gl.bindVertexArray(null); gl.deleteVertexArray(vao); gl.deleteFramebuffer(fb); gl.deleteTexture(color); gl.deleteTexture(depth);
      } catch (e) { results.push({ index: di, ok: false, error: String(e.message ?? e).slice(0, 300) }); }
    }
    return results;
  }, { selected: [...new Set(cases.map((c) => c.index))], sizes: Object.fromEntries(cases.map((c) => [c.index, c.size])) });
  await browser.close(); server.close();
  fs.writeFileSync(path.join(outDir, 'webgl.json'), JSON.stringify(webgl, null, 1));
  say(`WebGL reference: ${webgl.filter((r) => r.ok).length}/${webgl.length} drawn`);
fs.writeFileSync(refKeyFile, refKey);
}

// ---- Godot: the cases replayed, watched live (one item per draw)
// anisotropy: Godot 4.7's level is per viewport (Viewport.anisotropic_filtering_level, 2^n×), each draw's viewport
// at the level of its textures (three's texture.anisotropy). Only a draw mixing two levels could not be exact: flagged.
const anisoMismatch = new Set(draws.filter((d) => new Set(d.samplers.map((sm) => sm.params?.anisotropy ?? 1).filter((a) => a > 1)).size > 1).map((d) => d.index));
if (anisoMismatch.size) say(`${anisoMismatch.size} draws mix anisotropy levels (flagged)`);
fs.writeFileSync(path.join(gDir, 'project.godot'), ['config_version=5', '', '[application]', 'config/name="three replay"', '', '[rendering]', 'renderer/rendering_method="forward_plus"', ''].join('\n'));
fs.copyFileSync(path.join(here, 'scripts', 'replay.gd'), path.join(gDir, 'replay.gd'));
// the captured textures linked, not copied (hundreds of MB)
for (const c of cases) for (const t of c.textures) if (t.capture) for (const l of t.levels) for (const f of l.files) { const to = path.join(gDir, 'data', f); if (!fs.existsSync(to)) { try { fs.linkSync(path.join(capDir, 'blobs', f), to); } catch { fs.copyFileSync(path.join(capDir, 'blobs', f), to); } } }
const win = path.resolve(gDir);
fs.writeFileSync(path.join(gDir, 'godot.log'), '');
execFileSync(process.execPath, [path.join(here, 'scripts', 'live-run.mjs'), '--name', 'godot-replay', '--status', path.join(outDir, 'godot-replay.jsonl'), '--log', path.join(gDir, 'godot.log'),
  '--begin', '^REPLAY BEGIN (\\S+)', '--end', '^REPLAY END (\\S+)', '--fail', 'SHADER ERROR|^ERROR', '--pid', '^LIVE PID (\\d+)', '--skip-file', path.join(gDir, 'live-skip.txt'), '--min-idle', '20', '--factor', '10', '--',
  'powershell', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', 'C:\\Users\\Jonathan\\Desktop\\city\\tools\\build\\hidden_run.ps1',
  '-Command', `"C:\\Users\\Jonathan\\AppData\\Local\\Microsoft\\WinGet\\Links\\godot_console.exe" --path "${win}" --script res://replay.gd --rendering-driver vulkan >> "${win}\\godot.log" 2>&1`, '-WorkDir', win, '-TimeoutS', '86400'], { stdio: 'inherit' });

// ---- compare (scripts/replay-judge.mjs: orientation, alpha, float targets, depth-only draws)
const report = judge(cases, path.join(outDir, 'webgl'), path.join(gDir, 'out'), { depthOnly, programOf: (i) => index.draws[i].program })
  .map((r) => ({ ...r, colorMask: index.draws[r.index].state.colorMask.some(Boolean), ...(anisoMismatch.has(r.index) ? { anisoMismatch: true } : {}) }));
fs.writeFileSync(path.join(outDir, 'replay.json'), JSON.stringify(report, null, 1));
const count = (t) => report.filter((r) => r.tier === t).length;
const progs = new Map(); for (const r of report) progs.set(r.program, (progs.get(r.program) ?? true) && r.pass);
say(`REPLAY of the real frame (${report.length} draws): ${count('equal')} equal (every pixel ≤2/255), ${count('near')} near (≤1% of pixels differ), ${count('different')} different, ${count('empty')} empty (both blank: not counted), ${count('void')} void (no valid image); programs all-equal ${[...progs.values()].filter(Boolean).length}/${progs.size}; ${skipped.length} draws skipped`);
