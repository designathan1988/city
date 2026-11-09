// Tests of the three.js → Godot shader translation (src/render/godot-shader.ts), one rule each. The rules were
// measured against three.js and Godot drawing the same program (scripts/parity.mjs); these tests hold them.
import assert from 'node:assert/strict';
import { describe, it } from 'node:test';
import { loadGlsl, translateProgram } from '../src/render/godot-shader.ts';
import { glslDir } from './support.ts';

const glsl = loadGlsl(glslDir());
const t = (vertex: string, fragment: string, options = {}) => translateProgram(glsl, { vertex, fragment }, options);
const MVP = 'in vec3 position; uniform mat4 projectionMatrix; uniform mat4 modelViewMatrix;';
const POS = 'gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);';

describe('three.js program → Godot spatial shader', () => {
  it('one spatial shader, unshaded; three\'s matrices and attributes are Godot\'s built-ins; gl_Position is POSITION', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'out vec4 c; void main() { c = vec4(1.0); }');
    assert.match(s.code, /^shader_type spatial;$/m);
    assert.match(s.code, /render_mode unshaded/);
    assert.match(s.code, /POSITION = PROJECTION_MATRIX\s*\* MODELVIEW_MATRIX\s*\* vec4\(VERTEX, 1\.0\)/);
    assert.ok(!/uniform mat4 projectionMatrix/.test(s.code));
  });

  it('a raw color output is decoded (Godot encodes ALBEDO); an encoded one (linearToOutputTexel) is not', () => {
    const raw = t(`${MVP} void main() { ${POS} }`, 'out vec4 c; void main() { c = vec4(0.5); }');
    assert.match(raw.code, /ALBEDO = mix\(pow\(/);
    const enc = t(`${MVP} void main() { ${POS} }`, 'out vec4 c; vec4 linearToOutputTexel(vec4 v) { return v; } void main() { c = linearToOutputTexel(vec4(0.5)); }');
    assert.match(enc.code, /ALBEDO = _three_out\.rgb;/);
  });

  it('gl_FragCoord in three\'s convention: y from the bottom, depth 0 near (Godot: top-left, reversed Z)', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'out vec4 c; void main() { c = vec4(gl_FragCoord.xyz, 1.0); }');
    assert.match(s.code, /vec4\((_g_)?FRAGCOORD\.x, (_g_)?VIEWPORT_SIZE\.y - (_g_)?FRAGCOORD\.y, 1\.0 - (_g_)?FRAGCOORD\.z, (_g_)?FRAGCOORD\.w\)/);
    assert.match(s.code, /_three_fragment\(FRAGCOORD, VIEWPORT_SIZE, _three_out\);/);
  });

  it('gl_FragDepth is written to DEPTH reversed', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'out vec4 c; void main() { c = vec4(1.0); gl_FragDepth = 0.25; }');
    assert.match(s.code, /DEPTH = 1\.0 - _three_depth;/);
  });

  it('a struct uniform array is one uniform array per field; a member read is the field array', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'struct L { vec3 dir; vec3 color; }; uniform L lights[2]; out vec4 c; void main() { c = vec4(lights[1].color, 1.0); }');
    assert.match(s.code, /uniform vec3 lights__dir\[ ?2 ?\];/);
    assert.match(s.code, /lights__color\[1\]/);
    assert.ok(s.uniforms.some((u) => u.three === 'lights[].color' && u.godot === 'lights__color'));
  });

  it('a shadow sampler is a sampler2D compared by hand like the hardware: four texels, weights at 8 sub-texel bits', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'uniform sampler2DShadow sm; out vec4 c; void main() { c = vec4(texture(sm, vec3(0.5, 0.5, 0.3))); }');
    assert.match(s.code, /uniform sampler2D sm/);
    assert.match(s.code, /_three_shadow_lequal_linear\(sm, \(vec3\(0\.5, 0\.5, 0\.3\)\)\.xy, \(vec3\(0\.5, 0\.5, 0\.3\)\)\.z\)/);
    assert.match(s.code, /float _three_shadow_lequal_linear\(sampler2D t, vec2 uv, float r\)/);
    assert.match(s.code, /floor\(\(uv \* vec2\(s\) - 0\.5\) \* 256\.0\) \/ 256\.0/);
    const g = t(`${MVP} void main() { ${POS} }`, 'uniform sampler2DShadow sm; out vec4 c; void main() { c = vec4(texture(sm, vec3(0.5, 0.5, 0.3))); }', { shadowCompare: 'gequal', shadowFilter: 'nearest' });
    assert.match(g.code, /_three_shadow_gequal_nearest/);
    assert.match(g.code, /step\(\(texelFetch\(t, i, 0\)\.r\), r\)/);
  });

  it('three\'s colour, normal, tangent and skinWeight are CUSTOM floats (Godot stores ARRAY_COLOR in 8 bits); instanceColor is COLOR', () => {
    const s = t(`${MVP} in vec3 color; in vec3 normal; in vec4 tangent; in vec3 instanceColor; out vec3 v; void main() { v = color * instanceColor + normal + tangent.xyz; ${POS} }`, 'in vec3 v; out vec4 o; void main() { o = vec4(v, 1.0); }', { instancedAttributes: ['instanceColor'] });
    const at = Object.fromEntries(s.attributes.map((a) => [a.three, a.godot]));
    assert.match(at['color']!, /^CUSTOM\d\./);
    assert.match(at['normal']!, /^CUSTOM\d\./);
    assert.match(at['tangent']!, /^CUSTOM\d\./);
    assert.equal(at['instanceColor'], 'COLOR.rgb');
  });

  it('alpha: replace stores colour and alpha as written (blend_premul_alpha); none writes no ALPHA; blend keeps the mode', () => {
    const f = 'out vec4 o; void main() { o = vec4(0.2, 0.4, 0.6, 0.5); }';
    const r = t(`${MVP} void main() { ${POS} }`, f, { alphaOutput: 'replace', renderModes: ['blend_mix', 'cull_back'] });
    assert.match(r.code, /render_mode unshaded, cull_back, blend_premul_alpha;/);
    assert.match(r.code, /ALPHA = _three_out\.a;/);
    const n = t(`${MVP} void main() { ${POS} }`, f, { alphaOutput: 'none' });
    assert.doesNotMatch(n.code, /ALPHA =/);
    const b = t(`${MVP} void main() { ${POS} }`, f, { renderModes: ['blend_add'] });
    assert.match(b.code, /blend_add/);
    assert.match(b.code, /ALPHA = _three_out\.a;/);
  });

  it('orientation gl keeps three\'s clip y, gl_FragCoord.y is FRAGCOORD.y and dFdy keeps its sign; Godot\'s orientation flips all three', () => {
    const f = 'out vec4 o; void main() { o = vec4(gl_FragCoord.y, dFdy(gl_FragCoord.x), 0.0, 1.0); }';
    const gl = t(`${MVP} void main() { ${POS} }`, f, { matrices: 'three', orientation: 'gl' });
    assert.match(gl.code, /POSITION = vec4\(POSITION\.x, POSITION\.y,/);
    assert.match(gl.code, /vec4\(_g_FRAGCOORD\.x, _g_FRAGCOORD\.y,/);
    assert.doesNotMatch(gl.code, /\(-dFdy/);
    const gd = t(`${MVP} void main() { ${POS} }`, f, { matrices: 'three' });
    assert.match(gd.code, /POSITION = vec4\(POSITION\.x, -POSITION\.y,/);
    assert.match(gd.code, /VIEWPORT_SIZE\.y - _g_FRAGCOORD\.y/);
    assert.match(gd.code, /\(-dFdy/);
  });

  it('matN of N×N scalars is matN of columns; float(vector) is its first component; vector kinds are converted', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'out vec4 c; void main() { mat2 m = mat2(1.0, 0.0, 0.0, 1.0); ivec4 q = ivec4(2.5); float f = float(ivec2(3, 4)); c = vec4(m[0], float(q.x), f); }');
    assert.match(s.code, /mat2\(vec2\(1\.0, 0\.0\), vec2\(0\.0, 1\.0\)\)/);
    assert.match(s.code, /ivec4\(int\(2\.5\)\)/);
    assert.match(s.code, /float\(\(ivec2\( ?3, 4 ?\)\)\.x\)/);
  });

  it('an integer operator with a vector and a scalar: the scalar made the vector\'s type', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'uniform uint u; out vec4 c; void main() { uvec4 b = uvec4(u) & 255u; c = vec4(b) / 255.0; }');
    assert.match(s.code, /uvec4\(u\) & uvec4\(255u\)/);
  });

  it('a helper that reads a built-in receives it; the fragment main is a helper (no return in fragment())', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'uniform vec3 cameraPosition; out vec4 c; vec3 eye() { return cameraPosition; } void main() { if (eye().x > 1.0) { c = vec4(1.0); return; } c = vec4(eye(), 1.0); }');
    assert.match(s.code, /vec3 eye\(in vec3 _g_CAMERA_POSITION_WORLD\)/);
    assert.match(s.code, /_three_fragment\(CAMERA_POSITION_WORLD, _three_out\);/);
  });

  it('a vertex helper that assigns a varying is written inline (Godot assigns varyings in vertex() only)', () => {
    const s = t(`${MVP} out vec2 vUv; void set(vec2 p) { vUv = p; } void main() { set(position.xy); ${POS} }`, 'in vec2 vUv; out vec4 c; void main() { c = vec4(vUv, 0.0, 1.0); }');
    assert.ok(!/void set\(/.test(s.code));
    assert.match(s.code, /\{ vec2 p_i0 = VERTEX\.xy;\s+vUv = p_i0;\s+\}/);
  });

  it('attributes: custom ones in CUSTOM0–3, instanced ones in INSTANCE_CUSTOM, beyond the slots a data texture', () => {
    const decl = ['a', 'b', 'c', 'd', 'e'].map((n) => `in vec4 ${n};`).join(' ');
    const s = t(`${MVP} ${decl} in float k; out vec4 v; void main() { v = a + b + c + d + e + vec4(k); ${POS} }`, 'in vec4 v; out vec4 o; void main() { o = v; }', { instancedAttributes: ['k'] });
    assert.deepEqual(s.attributes.map((x) => x.godot.split('(')[0]), ['VERTEX', 'CUSTOM0.xyzw', 'CUSTOM1.xyzw', 'CUSTOM2.xyzw', 'CUSTOM3.xyzw', 'texelFetch', 'INSTANCE_CUSTOM.x']);
    assert.match(s.code, /uniform sampler2D _three_attr_e : filter_nearest, repeat_disable;/);
  });

  it('instanceMatrix is the identity (Godot puts the MultiMesh transform in MODEL_MATRIX); skin indices are bones, weights a CUSTOM float', () => {
    const s = t(`${MVP} in mat4 instanceMatrix; in vec4 skinIndex; in vec4 skinWeight; out vec4 w; void main() { w = skinIndex * skinWeight; gl_Position = projectionMatrix * modelViewMatrix * instanceMatrix * vec4(position, 1.0); }`, 'in vec4 w; out vec4 o; void main() { o = w; }');
    assert.match(s.code, /mat4 _three_instanceMatrix = mat4\(1\.0\);/);
    assert.match(s.code, /vec4\(BONE_INDICES\)\s*\* CUSTOM0\.xyzw/);
  });

  it('many varyings: the small ones share vec4s (Godot\'s 32 locations hold its own too)', () => {
    const names = Array.from({ length: 10 }, (_, i) => `v${i}`);
    const s = t(`${MVP} ${names.map((n) => `out float ${n};`).join(' ')} void main() { ${names.map((n, i) => `${n} = ${i}.0;`).join(' ')} ${POS} }`,
      `${names.map((n) => `in float ${n};`).join(' ')} out vec4 o; void main() { o = vec4(${names.join(' + ')}); }`);
    assert.equal((s.code.match(/^varying /gm) ?? []).length, 3);
    assert.match(s.code, /_three_pack0\.x = 0\.0;/);
  });

  it('a second color output (multiple render targets) is said, not dropped silently', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'layout(location = 0) out vec4 a; layout(location = 1) out vec4 b; void main() { a = vec4(1.0); b = vec4(0.5); }');
    // one pass per location (options.fragmentOutput): the other output is a note and a local, not dropped silently
    assert.ok(s.notes.some((u) => /multiple render targets/.test(u)));
  });

  it('the fragment output is the stage\'s: a function that writes it receives it (inout), as main does', () => {
    const s = t(`${MVP} void main() { ${POS} }`, 'out vec4 o; void paint(float k) { o = vec4(k); } void main() { paint(0.5); o.a = 1.0; }');
    assert.match(s.code, /void paint\(float k, inout vec4 _g__three_out\)/);
    assert.match(s.code, /void _three_fragment\(inout vec4 _g__three_out\)/);
    assert.match(s.code, /_three_fragment\(_three_out\);/);
  });

  it('channels alpha: the program\'s alpha in ALBEDO, no ALPHA (the opaque pipeline); alphaOutput given wins over OPAQUE', () => {
    const f = 'out vec4 o; void main() { o = vec4(0.2, 0.4, 0.6, 0.5); }';
    const a = t(`${MVP} void main() { ${POS} }`, f, { channels: 'alpha', alphaOutput: 'none' });
    assert.match(a.code, /ALBEDO = vec3\(_three_out\.a\);/);
    assert.doesNotMatch(a.code, /ALPHA =/);
    const o = t(`${MVP} void main() { ${POS} }`, `#define OPAQUE\n${f}`, { alphaOutput: 'blend' });
    assert.match(o.code, /ALPHA = _three_out\.a;/);
  });
});
