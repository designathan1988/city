// SEMANTIC PARITY of the three.js → Godot shader translation: the same scene drawn by three.js (r186, WebGL2 in
// Chrome) and by Godot (Forward+, Vulkan) from the program three linked, translated by src/render/godot-shader.ts,
// with the uniform values three uploaded (every gl.uniform* call captured), the same mesh (indices reversed: three's
// front faces are counter-clockwise, Godot's clockwise), the same camera; then compared pixel by pixel.
// `node scripts/parity.mjs <out dir> <three build dir> <playwright dir> <glsl parser dir>`
import fs from 'node:fs';
import http from 'node:http';
import path from 'node:path';
import { machine } from '../src/common/machine.ts';
import { filesKey, folderKey, keyOf, logOf, mustRun, runTasksSync, toolKey } from '../src/common/tasks.ts';

/** Godot drawing every parity case and reading each back; past this its process tree is stopped. */
const PARITY_TIMEOUT_S = 600;
import { createRequire } from 'node:module';
import { pathToFileURL } from 'node:url';

const [outDir, threeDir, pwDir, glslDir] = process.argv.slice(2);
const { chromium } = createRequire(path.join(pwDir, 'package.json'))('playwright');
const { loadGlsl, translateProgram } = await import(pathToFileURL(path.resolve(import.meta.dirname, '../src/render/godot-shader.ts')).href);
const glsl = loadGlsl(glslDir);
const SIZE = 64;

// ---- the cases: each exercises translation rules; three builds them, Godot rebuilds them from what three ran
const PLANE = { kind: 'plane' };
const cases = [
  { id: 'matrices-uv', geometry: PLANE, material: 'shader', side: 'front',
    vertex: 'varying vec2 vUv; void main() { vUv = uv; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'varying vec2 vUv; void main() { gl_FragColor = vec4(vUv, 0.25, 1.0); }' },
  { id: 'world-camera', geometry: PLANE, material: 'shader', side: 'double', rotation: [0.4, 0.7, 0.0], position: [0.1, -0.2, 0.0],
    vertex: 'varying vec3 vWorld; void main() { vec4 w = modelMatrix * vec4(position, 1.0); vWorld = w.xyz; gl_Position = projectionMatrix * viewMatrix * w; }',
    fragment: 'varying vec3 vWorld; void main() { gl_FragColor = vec4(normalize(vWorld - cameraPosition) * 0.5 + 0.5, 1.0); }' },
  { id: 'vertex-color-custom', geometry: { kind: 'plane', colors: true, custom: 'aShade' }, material: 'shader', side: 'front', vertexColors: true,
    vertex: 'attribute float aShade; varying vec3 vColor; varying float vShade; void main() { vColor = color; vShade = aShade; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'varying vec3 vColor; varying float vShade; void main() { gl_FragColor = vec4(vColor * vShade, 1.0); }' },
  { id: 'struct-uniform-array', geometry: PLANE, material: 'shader', side: 'front', rotation: [0.3, -0.5, 0.0],
    uniforms: { lights: [{ dir: [0.3, 0.8, 0.5], color: [0.9, 0.4, 0.1] }, { dir: [-0.7, 0.1, 0.7], color: [0.1, 0.5, 0.8] }] },
    vertex: 'varying vec3 vNormal; void main() { vNormal = normalize(normalMatrix * normal); gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'struct L { vec3 dir; vec3 color; }; uniform L lights[2]; varying vec3 vNormal; void main() { vec3 c = vec3(0.0); for (int i = 0; i < 2; i++) { c += lights[i].color * max(dot(vNormal, normalize(lights[i].dir)), 0.0); } gl_FragColor = vec4(c, 1.0); }' },
  { id: 'integer-ops', geometry: PLANE, material: 'shader', side: 'front', uniforms: { uBits: { uint: 0x40c08020 } }, glsl3: true,
    vertex: 'out vec2 vUv; void main() { vUv = uv; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'uniform uint uBits; in vec2 vUv; out vec4 outColor; void main() { uvec4 b = uvec4(uBits, uBits >> 8u, uBits >> 16u, uBits >> 24u) & 255u; ivec2 cell = ivec2(vUv * 8.0) & 1; outColor = vec4(vec3(b.rgb) / 255.0 * (0.5 + 0.5 * float(cell.x ^ cell.y)), 1.0); }' },
  { id: 'matrix-constructors', geometry: PLANE, material: 'shader', side: 'front',
    vertex: 'varying vec2 vUv; void main() { vUv = uv; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'varying vec2 vUv; const mat2 TURN = mat2(0.8, 0.6, -0.6, 0.8); void main() { mat3 m = mat3(0.5, 0.1, 0.0, 0.2, 0.6, 0.1, 0.0, 0.3, 0.7); vec2 t = TURN * (vUv - 0.5) + 0.5; gl_FragColor = vec4(transpose(m) * vec3(t, 1.0 - t.x), 1.0); }' },
  { id: 'fragcoord-discard', geometry: PLANE, material: 'shader', side: 'front',
    vertex: 'void main() { gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'void main() { if (gl_FragCoord.y < 24.0) discard; gl_FragColor = vec4(gl_FragCoord.x / 64.0, gl_FragCoord.y / 64.0, 0.5, 1.0); }' },
  { id: 'front-facing', geometry: PLANE, material: 'shader', side: 'double', rotation: [0.0, 2.6, 0.0],
    vertex: 'void main() { gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'void main() { gl_FragColor = gl_FrontFacing ? vec4(1.0, 0.2, 0.1, 1.0) : vec4(0.1, 0.9, 0.3, 1.0); }' },
  { id: 'depth', geometry: PLANE, material: 'shader', side: 'front', rotation: [-1.1, 0.0, 0.0],
    vertex: 'void main() { gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'void main() { gl_FragColor = vec4(vec3(pow(gl_FragCoord.z, 8.0)), 1.0); }' },
  { id: 'texture-srgb', geometry: PLANE, material: 'shader', side: 'front', texture: { name: 'map', srgb: true },
    vertex: 'varying vec2 vUv; void main() { vUv = uv; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'uniform sampler2D map; varying vec2 vUv; void main() { gl_FragColor = texture2D(map, vUv); }' },
  { id: 'instanced', geometry: PLANE, material: 'shader', side: 'front', instances: [[-0.55, 0.3, 0, 0.45, [1, 0.3, 0.2]], [0.5, -0.35, 0, 0.6, [0.2, 0.5, 1]]],
    vertex: 'varying vec3 vColor; void main() { vColor = instanceColor; gl_Position = projectionMatrix * modelViewMatrix * instanceMatrix * vec4(position, 1.0); }',
    fragment: 'varying vec3 vColor; void main() { gl_FragColor = vec4(vColor, 1.0); }' },
  { id: 'basic-material', geometry: PLANE, material: 'basic', color: 0x3366cc, side: 'front' },
  { id: 'overflow-attributes', geometry: { kind: 'plane', vec4s: ['a0', 'a1', 'a2', 'a3', 'a4'] }, material: 'shader', side: 'front',
    vertex: 'attribute vec4 a0; attribute vec4 a1; attribute vec4 a2; attribute vec4 a3; attribute vec4 a4; varying vec4 vSum; void main() { vSum = (a0 + a1 * 0.5 + a2 * 0.25 + a3 * 0.125 + a4) / 2.875; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'varying vec4 vSum; void main() { gl_FragColor = vec4(vSum.rgb, 1.0); }' },
  { id: 'instanced-custom', geometry: PLANE, material: 'shader', side: 'front', instances: [[-0.6, 0.4, 0, 0.35, [1, 1, 1]], [0.55, 0.3, 0, 0.4, [1, 1, 1]], [0.0, -0.5, 0, 0.45, [1, 1, 1]]],
    instanced: { aTint: [[0.9, 0.2, 0.1], [0.1, 0.8, 0.3], [0.2, 0.3, 0.95]], aScale: [[0.7], [1.0], [1.3]], aExtra: [[0.2, 0.4, 0.6, 1.0], [0.9, 0.1, 0.5, 0.6], [0.3, 0.7, 0.2, 0.3]] },
    vertex: 'attribute vec3 aTint; attribute float aScale; attribute vec4 aExtra; varying vec3 vColor; void main() { vColor = aTint * aExtra.w + aExtra.rgb * 0.2; gl_Position = projectionMatrix * modelViewMatrix * instanceMatrix * vec4(position * aScale, 1.0); }',
    fragment: 'varying vec3 vColor; void main() { gl_FragColor = vec4(vColor, 1.0); }' },
  { id: 'skinned', geometry: { kind: 'plane', skin: true }, material: 'basic', color: 0xcc8844, side: 'double', skinned: true },
  { id: 'varying-helper-return', geometry: PLANE, material: 'shader', side: 'front',
    vertex: 'varying vec2 vUv; varying float vSide; void setVaryings(vec2 p, float s) { vUv = p; vSide = s; } void main() { setVaryings(uv, position.x > 0.0 ? 1.0 : 0.0); gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); if (position.y > 50.0) { gl_Position = vec4(0.0); return; } }',
    fragment: 'varying vec2 vUv; varying float vSide; void main() { gl_FragColor = vec4(vUv * (0.5 + 0.5 * vSide), vSide, 1.0); }' },
  { id: 'shadow-compare', geometry: PLANE, material: 'shader', side: 'front', depthTexture: true,
    vertex: 'varying vec2 vUv; void main() { vUv = uv; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'uniform sampler2DShadow shadowTex; varying vec2 vUv; void main() { float lit = texture(shadowTex, vec3(vUv, 0.5)); gl_FragColor = vec4(vec3(0.2 + 0.8 * lit), 1.0); }' },
  // multiple render targets: three draws once into two attachments (WebGLRenderTarget count 2); Godot draws one
  // pass per location (options.fragmentOutput), each compared with its attachment
  { id: 'mrt', geometry: PLANE, material: 'shader', side: 'front', glsl3: true, mrt: 2, rotation: [0.2, -0.4, 0.0],
    vertex: 'out vec2 vUv; out vec3 vNormal; void main() { vUv = uv; vNormal = normalize(normalMatrix * normal); gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'in vec2 vUv; in vec3 vNormal; layout(location = 0) out vec4 gColor; layout(location = 1) out vec4 gNormal; void main() { gColor = vec4(vUv, 1.0 - vUv.x, 1.0); gNormal = vec4(normalize(vNormal) * 0.5 + 0.5, 1.0); }' },
  // integer textures with real data (the frame's lot maps are all zero): an RGBA32UI texture read by a usampler2D,
  // values that fit 16 bits (Godot's RGBA16 uint route) and values above 65535 (the 32-bit words as float bits)
  { id: 'uint-texture-16', geometry: PLANE, material: 'shader', side: 'front', glsl3: true, uintTexture: { name: 'uData', max: 65535 },
    vertex: 'out vec2 vUv; void main() { vUv = uv; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'uniform highp usampler2D uData; in vec2 vUv; out vec4 outColor; void main() { uvec4 v = texelFetch(uData, ivec2(vUv * 4.0), 0); outColor = vec4(float(v.r & 255u), float((v.g >> 8u) & 255u), float(v.b % 251u), 255.0) / 255.0; }' },
  { id: 'uint-texture-32', geometry: PLANE, material: 'shader', side: 'front', glsl3: true, uintTexture: { name: 'uData', max: 4294967295 },
    vertex: 'out vec2 vUv; void main() { vUv = uv; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
    fragment: 'uniform highp usampler2D uData; in vec2 vUv; out vec4 outColor; void main() { uvec4 v = texelFetch(uData, ivec2(vUv * 4.0), 0); outColor = vec4(float((v.r >> 24u) & 255u), float((v.g >> 13u) & 255u), float(v.b % 251u), 255.0) / 255.0; }' },
  { id: 'standard-lit', geometry: { kind: 'sphere' }, material: 'standard', color: 0xb08850, roughness: 0.45, metalness: 0.2, side: 'front',
    lights: { directional: { color: 0xffffff, intensity: 2.0, position: [2, 3, 4] }, ambient: { color: 0x404060, intensity: 1.0 } } },
  { id: 'lambert-lit', geometry: { kind: 'sphere' }, material: 'lambert', color: 0x88aa55, side: 'front',
    lights: { directional: { color: 0xffeedd, intensity: 1.5, position: [-3, 2, 3] }, ambient: { color: 0x202030, intensity: 1.0 } } },
];

// ---- three: a page on a small server over three's build, each case drawn and read back
fs.rmSync(outDir, { recursive: true, force: true });
fs.mkdirSync(path.join(outDir, 'shaders'), { recursive: true });
const page0 = `<!doctype html><html><head><script type="importmap">{"imports":{"three":"/three/three.module.js"}}</script></head><body></body></html>`;
const server = http.createServer((req, res) => {
  const url = decodeURIComponent((req.url ?? '/').split('?')[0]);
  if (url === '/') { res.setHeader('content-type', 'text/html'); res.end(page0); return; }
  if (url.startsWith('/three/')) {
    const file = path.join(threeDir, url.slice('/three/'.length));
    if (fs.existsSync(file)) { res.setHeader('content-type', 'text/javascript'); res.end(fs.readFileSync(file)); return; }
  }
  res.statusCode = 404; res.end();
});
await new Promise((r) => server.listen(0, '127.0.0.1', r));
const base = `http://127.0.0.1:${server.address().port}/`;
const browser = await chromium.launch({ channel: 'chrome', headless: true, args: ['--enable-gpu', '--ignore-gpu-blocklist'] });
const page = await browser.newPage();
await page.goto(base);
const threeResults = await page.evaluate(async ({ cases, SIZE }) => {
  const THREE = await import('three');
  // every program linked and every uniform three uploads, by program
  const sourceOf = new WeakMap(); const attached = new WeakMap(); const names = new WeakMap(); const values = new Map(); let current = null;
  const C = WebGL2RenderingContext.prototype;
  const wrap = (m, f) => { const o = C[m]; C[m] = function (...a) { f.call(this, ...a); return o.apply(this, a); }; };
  wrap('shaderSource', (s, src) => sourceOf.set(s, src));
  wrap('attachShader', (p, s) => (attached.get(p) ?? attached.set(p, []).get(p)).push(s));
  const getLoc = C.getUniformLocation;
  C.getUniformLocation = function (p, name) { const l = getLoc.call(this, p, name); if (l) names.set(l, { program: p, name }); return l; };
  wrap('useProgram', (p) => { current = p; });
  // textures: what each unit holds, each texture's data and sampler state, the units bound at each program's draw
  let unit = 0; const units = []; const texInfo = new WeakMap(); const drawUnits = new Map();
  wrap('activeTexture', (u) => { unit = u - WebGL2RenderingContext.TEXTURE0; });
  wrap('bindTexture', (target, tex) => { if (target === WebGL2RenderingContext.TEXTURE_2D) units[unit] = tex; });
  const info = () => { const t = units[unit]; if (!t) return null; return texInfo.get(t) ?? texInfo.set(t, { params: {} }).get(t); };
  wrap('texImage2D', (...a) => {
    const i = info(); if (!i || a.length < 9) return;
    const [, level, internalformat, width, height, , format, type, pixels] = a;
    if (level !== 0 || !pixels || !ArrayBuffer.isView(pixels)) return;
    Object.assign(i, { internalformat, width, height, format, type, bytes: Array.from(new Uint8Array(pixels.buffer, pixels.byteOffset, pixels.byteLength)) });
  });
  wrap('texStorage2D', (target, levels, internalformat, width, height) => { const i = info(); if (i) Object.assign(i, { internalformat, width, height }); });
  wrap('texSubImage2D', (...a) => {
    const i = info(); if (!i || a.length < 9) return;
    const [, level, x, y, width, height, format, type, pixels] = a;
    if (level !== 0 || x !== 0 || y !== 0 || !pixels || !ArrayBuffer.isView(pixels)) return;
    Object.assign(i, { width, height, format, type, bytes: Array.from(new Uint8Array(pixels.buffer, pixels.byteOffset, pixels.byteLength)) });
  });
  wrap('texParameteri', (target, pname, param) => { if (target === WebGL2RenderingContext.TEXTURE_2D) { const i = info(); if (i) i.params[pname] = param; } });
  for (const m of ['drawElements', 'drawArrays', 'drawElementsInstanced', 'drawArraysInstanced']) wrap(m, () => { if (current) drawUnits.set(current, units.slice()); });
  for (const m of ['uniform1f', 'uniform2f', 'uniform3f', 'uniform4f', 'uniform1i', 'uniform1ui', 'uniform1fv', 'uniform2fv', 'uniform3fv', 'uniform4fv', 'uniform1iv', 'uniform1uiv', 'uniformMatrix3fv', 'uniformMatrix4fv']) {
    wrap(m, (loc, ...rest) => {
      const n = loc && names.get(loc); if (!n) return;
      const data = m.startsWith('uniformMatrix') ? Array.from(rest[1]) : /v$/.test(m) ? Array.from(rest[0]) : rest;
      (values.get(n.program) ?? values.set(n.program, {}).get(n.program))[n.name] = { kind: m.replace(/^uniform(Matrix)?/, ''), data };
    });
  }
  const out = [];
  for (const c of cases) {
    const canvas = document.createElement('canvas'); canvas.width = SIZE; canvas.height = SIZE;
    const renderer = new THREE.WebGLRenderer({ canvas, antialias: false, preserveDrawingBuffer: true });
    renderer.setPixelRatio(1); renderer.setSize(SIZE, SIZE, false); renderer.setClearColor(0x000000, 1);
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(50, 1, 0.1, 100); camera.position.set(0, 0, 3); camera.lookAt(0, 0, 0);
    let geometry = c.geometry.kind === 'sphere' ? new THREE.SphereGeometry(1, 24, 16) : new THREE.PlaneGeometry(2, 2, 2, 2);
    if (c.geometry.colors) { const n = geometry.attributes.position.count; const col = new Float32Array(n * 3); for (let i = 0; i < n; i++) { col[i * 3] = (i % 3) / 2; col[i * 3 + 1] = ((i + 1) % 4) / 3; col[i * 3 + 2] = 1 - (i % 5) / 4; } geometry.setAttribute('color', new THREE.BufferAttribute(col, 3)); }
    if (c.geometry.vec4s) { const n = geometry.attributes.position.count; c.geometry.vec4s.forEach((name, k) => { const a = new Float32Array(n * 4); for (let i = 0; i < n * 4; i++) a[i] = ((i * 7 + k * 13) % 11) / 10; geometry.setAttribute(name, new THREE.BufferAttribute(a, 4)); }); }
    if (c.geometry.skin) { const n = geometry.attributes.position.count; const si = new Uint16Array(n * 4); const sw = new Float32Array(n * 4); const p = geometry.attributes.position; for (let i = 0; i < n; i++) { const w = (p.getY(i) + 1) / 2; si[i * 4] = 0; si[i * 4 + 1] = 1; sw[i * 4] = 1 - w; sw[i * 4 + 1] = w; } geometry.setAttribute('skinIndex', new THREE.Uint16BufferAttribute(si, 4)); geometry.setAttribute('skinWeight', new THREE.Float32BufferAttribute(sw, 4)); }
    if (c.instanced) for (const [name, rows] of Object.entries(c.instanced)) geometry.setAttribute(name, new THREE.InstancedBufferAttribute(new Float32Array(rows.flat()), rows[0].length));
    if (c.geometry.custom) { const n = geometry.attributes.position.count; const a = new Float32Array(n); for (let i = 0; i < n; i++) a[i] = 0.3 + 0.7 * (i % 3) / 2; geometry.setAttribute(c.geometry.custom, new THREE.BufferAttribute(a, 1)); }
    const side = c.side === 'double' ? THREE.DoubleSide : THREE.FrontSide;
    let material;
    const uniforms = {};
    for (const [k, v] of Object.entries(c.uniforms ?? {})) {
      if (Array.isArray(v)) uniforms[k] = { value: v.map((o) => Object.fromEntries(Object.entries(o).map(([f, x]) => [f, new THREE.Vector3(...x)]))) };
      else if (v.uint !== undefined) uniforms[k] = { value: v.uint };
    }
    let texture = null;
    if (c.depthTexture) {
      const d = new Float32Array(16); for (let i = 0; i < 16; i++) d[i] = (i % 4) / 3 * 0.8 + Math.floor(i / 4) * 0.05;
      const dt = new THREE.DataTexture(d, 4, 4, THREE.DepthFormat, THREE.FloatType);
      dt.compareFunction = THREE.LessEqualCompare; dt.magFilter = THREE.NearestFilter; dt.minFilter = THREE.NearestFilter; dt.needsUpdate = true;
      uniforms.shadowTex = { value: dt };
    }
    if (c.uintTexture) {
      // 4×4 texels, RGBA, a spread of values up to the case's maximum (deterministic)
      const data = new Uint32Array(64);
      for (let i = 0; i < 64; i++) data[i] = Math.floor(((i * 2654435761) % 4294967296) / 4294967296 * (c.uintTexture.max + 1)) >>> 0;
      const ut = new THREE.DataTexture(data, 4, 4, THREE.RGBAIntegerFormat, THREE.UnsignedIntType);
      ut.internalFormat = 'RGBA32UI'; ut.magFilter = THREE.NearestFilter; ut.minFilter = THREE.NearestFilter; ut.needsUpdate = true;
      uniforms[c.uintTexture.name] = { value: ut };
    }
    if (c.texture) {
      const data = new Uint8Array([255, 40, 40, 255, 40, 200, 60, 255, 50, 80, 230, 255, 240, 230, 90, 255]);
      texture = new THREE.DataTexture(data, 2, 2, THREE.RGBAFormat);
      texture.magFilter = THREE.NearestFilter; texture.minFilter = THREE.NearestFilter;
      if (c.texture.srgb) texture.colorSpace = THREE.SRGBColorSpace;
      texture.needsUpdate = true;
      uniforms[c.texture.name] = { value: texture };
    }
    if (c.material === 'shader') material = new THREE.ShaderMaterial({ vertexShader: c.vertex, fragmentShader: c.fragment, uniforms, side, vertexColors: !!c.vertexColors, ...(c.glsl3 ? { glslVersion: THREE.GLSL3 } : {}) });
    else if (c.material === 'basic') material = new THREE.MeshBasicMaterial({ color: c.color, side });
    else if (c.material === 'standard') material = new THREE.MeshStandardMaterial({ color: c.color, roughness: c.roughness, metalness: c.metalness, side });
    else material = new THREE.MeshLambertMaterial({ color: c.color, side });
    let mesh;
    if (c.instances) {
      mesh = new THREE.InstancedMesh(geometry, material, c.instances.length);
      const m4 = new THREE.Matrix4();
      c.instances.forEach(([x, y, z, s, col], i) => { m4.makeScale(s, s, s).setPosition(x, y, z); mesh.setMatrixAt(i, m4); mesh.setColorAt(i, new THREE.Color().setRGB(...col, THREE.SRGBColorSpace)); });
    } else if (c.skinned) {
      mesh = new THREE.SkinnedMesh(geometry, material);
      const b0 = new THREE.Bone(); const b1 = new THREE.Bone(); b1.position.y = 1; b0.add(b1); b0.position.y = -1;
      mesh.add(b0); mesh.bind(new THREE.Skeleton([b0, b1]));
      b1.rotation.z = 0.6; b0.rotation.z = -0.15;
    } else mesh = new THREE.Mesh(geometry, material);
    if (c.rotation) mesh.rotation.set(...c.rotation);
    if (c.position) mesh.position.set(...c.position);
    scene.add(mesh);
    if (c.lights?.directional) { const d = new THREE.DirectionalLight(c.lights.directional.color, c.lights.directional.intensity); d.position.set(...c.lights.directional.position); scene.add(d); }
    if (c.lights?.ambient) scene.add(new THREE.AmbientLight(c.lights.ambient.color, c.lights.ambient.intensity));
    attached.get = attached.get; // (keep)
    let rt = null;
    if (c.mrt) { rt = new THREE.WebGLRenderTarget(SIZE, SIZE, { count: c.mrt, type: THREE.UnsignedByteType, depthBuffer: true }); renderer.setRenderTarget(rt); renderer.setClearColor(0x000000, 1); renderer.clear(); }
    renderer.render(scene, camera);
    // each attachment as stored (three's render target: the values the program wrote), bottom row first like readPixels
    const attachments = rt ? Array.from({ length: c.mrt }, (_, k) => { const b = new Uint8Array(SIZE * SIZE * 4); renderer.readRenderTargetPixels(rt, 0, 0, SIZE, SIZE, b, undefined, k); return Array.from(b); }) : null;
    const gl = renderer.getContext();
    // three's DataTexture in DepthFormat uploads no texels (probe: out/compare-probe.mjs): the depth data is put
    // into three's own texture with WebGL, compare mode as three set it, and the scene drawn again
    if (c.depthTexture) {
      const dt = uniforms.shadowTex.value;
      const handle = renderer.properties.get(dt).__webglTexture;
      gl.activeTexture(gl.TEXTURE0); gl.bindTexture(gl.TEXTURE_2D, handle);
      gl.texImage2D(gl.TEXTURE_2D, 0, gl.DEPTH_COMPONENT32F, 4, 4, 0, gl.DEPTH_COMPONENT, gl.FLOAT, dt.image.data);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST); gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_COMPARE_MODE, gl.COMPARE_REF_TO_TEXTURE); gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_COMPARE_FUNC, gl.LEQUAL);
      renderer.resetState();
      renderer.render(scene, camera);
    }
    const program = gl.getParameter(gl.CURRENT_PROGRAM);
    const stages = (attached.get(program) ?? []).map((s) => sourceOf.get(s));
    const pixels = new Uint8Array(SIZE * SIZE * 4);
    gl.readPixels(0, 0, SIZE, SIZE, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
    const samplers = [];
    const bound = drawUnits.get(program) ?? [];
    for (const [uname, v] of Object.entries(values.get(program) ?? {})) {
      if (v.kind !== '1i') continue;
      const t = bound[v.data[0]];
      const ti = t && texInfo.get(t);
      if (ti && ti.bytes) samplers.push({ name: uname, unit: v.data[0], width: ti.width, height: ti.height, internalformat: ti.internalformat, format: ti.format, type: ti.type, bytes: ti.bytes, params: ti.params });
    }
    mesh.updateMatrixWorld();
    const g = geometry;
    const attr = (n) => (g.attributes[n] ? Array.from(g.attributes[n].array) : null);
    out.push({
      id: c.id,
      program: { vertex: stages.find((s) => /gl_Position/.test(s)), fragment: stages.find((s) => !/gl_Position/.test(s)) },
      uniforms: values.get(program) ?? {},
      pixels: Array.from(pixels),
      mesh: { position: attr('position'), normal: attr('normal'), uv: attr('uv'), color: attr('color'), index: Array.from(g.index.array) },
      attributes: Object.fromEntries(Object.entries(g.attributes).map(([n, a]) => [n, { itemSize: a.itemSize, array: Array.from(a.array), instanced: !!a.isInstancedBufferAttribute }])),
      transform: mesh.matrixWorld.toArray(),
      instances: c.instances ? c.instances.map((_, i) => { const m = new THREE.Matrix4(); mesh.getMatrixAt(i, m); const col = new THREE.Color(); mesh.getColorAt(i, col); return { matrix: m.toArray(), color: [col.r, col.g, col.b] }; }) : null,
      samplers,
      side: c.side,
      ...(attachments ? { attachments } : {}),
    });
    renderer.dispose();
  }
  return out;
}, { cases, SIZE });
await browser.close();
server.close();
// an MRT case is one comparison per attachment: the program translated for that location
threeResults.splice(0, threeResults.length, ...threeResults.flatMap((r) => (r.attachments ? r.attachments.map((px, k) => ({ ...r, id: `${r.id}-o${k}`, pixels: px, fragmentOutput: k })) : [r])));

// ---- translate each program; the uniforms three uploaded, under the Godot names
const godotCases = [];
for (const r of threeResults) {
  const GL = { NEAREST: 0x2600, LINEAR: 0x2601, NEAREST_MIPMAP_NEAREST: 0x2700, LINEAR_MIPMAP_NEAREST: 0x2701, NEAREST_MIPMAP_LINEAR: 0x2702, LINEAR_MIPMAP_LINEAR: 0x2703, MIN: 0x2801, MAG: 0x2800, WRAP_S: 0x2802, REPEAT: 0x2901, CLAMP: 0x812f, SRGB8_ALPHA8: 0x8c43 };
  const samplerHints = {};
  for (const sm of r.samplers) {
    const min = sm.params[GL.MIN] ?? GL.LINEAR;
    const filter = min === GL.NEAREST ? 'filter_nearest' : min === GL.LINEAR ? 'filter_linear' : (min === GL.NEAREST_MIPMAP_NEAREST || min === GL.NEAREST_MIPMAP_LINEAR) ? 'filter_nearest_mipmap' : 'filter_linear_mipmap';
    const repeat = (sm.params[GL.WRAP_S] ?? GL.REPEAT) === GL.REPEAT ? 'repeat_enable' : 'repeat_disable';
    samplerHints[sm.name] = [...(sm.internalformat === GL.SRGB8_ALPHA8 ? ['source_color'] : []), filter, repeat];
  }
  // a depth texture's sampler state (its compare function, its filter): the shadow compare done like the hardware
  const depthSampler = r.samplers.find((sm) => sm.internalformat === 0x8cac || sm.internalformat === 0x81a6 || sm.format === 0x1902);
  const shadowOpts = depthSampler ? { shadowCompare: 'lequal', shadowFilter: (depthSampler.params[GL.MIN] ?? GL.LINEAR) === GL.NEAREST ? 'nearest' : 'linear' } : {};
  // an integer texture (RGBA_INTEGER, UNSIGNED_INT): Godot 4.7's widest integer texture is RGBA16 uint; wider values
  // go as their 32-bit words in a float texture, read back with floatBitsToUint (the frame replay's two routes)
  const intRoute = (sm) => { const u = new Uint32Array(new Uint8Array(sm.bytes).buffer); return u.every((x) => x <= 0xffff) ? 'UINT16' : 'BITS32'; };
  const intSamplers = r.samplers.filter((sm) => sm.format === 0x8d99 && sm.type === 0x1405);
  const floatBitsSamplers = intSamplers.filter((sm) => intRoute(sm) === 'BITS32').map((sm) => sm.name);
  const instancedAttributes = Object.entries(r.attributes).filter(([, a]) => a.instanced).map(([n]) => n);
  const s = translateProgram(glsl, r.program, { renderModes: [r.side === 'double' ? 'cull_disabled' : 'cull_back'], samplerHints, instancedAttributes, ...(r.fragmentOutput !== undefined ? { fragmentOutput: r.fragmentOutput } : {}), ...shadowOpts, ...(floatBitsSamplers.length ? { floatBitsSamplers } : {}) });
  fs.writeFileSync(path.join(outDir, 'shaders', `${r.id}.gdshader`), s.code);
  const params = [];
  const byThree = new Map(s.uniforms.map((u) => [u.three, u]));
  const seen = new Map();
  for (const [name, v] of Object.entries(r.uniforms)) {
    // "a[2].b" -> struct field array, "a[2]" -> array element, "a" -> plain
    const m = /^(\w+)(?:\[(\d+)\])?(?:\.(\w+))?$/.exec(name);
    if (!m) continue;
    const [, root, idx, field] = m;
    const u = field ? byThree.get(`${root}[].${field}`) ?? byThree.get(`${root}.${field}`) : byThree.get(root);
    if (!u || /sampler/.test(u.type)) continue;
    const key = u.godot;
    const entry = seen.get(key) ?? { godot: key, type: u.type, array: u.array !== '', elements: [] };
    seen.set(key, entry);
    const width = { float: 1, int: 1, uint: 1, bool: 1, vec2: 2, vec3: 3, vec4: 4, mat3: 9, mat4: 16, ivec2: 2, ivec3: 3, ivec4: 4, uvec4: 4 }[u.type] ?? 1;
    const start = Number(idx ?? 0);
    for (let k = 0; k * width < v.data.length; k++) entry.elements[start + k] = v.data.slice(k * width, k * width + width);
  }
  params.push(...seen.values());
  const FORMATS = { '6408:5121': 'RGBA8', '33319:5131': 'RGH', '6408:5131': 'RGBAH', '33319:5126': 'RGF', '6408:5126': 'RGBAF', '6403:5126': 'RF', '6403:5131': 'RH', '6403:5121': 'R8', '6402:5126': 'RF' };
  const textures = r.samplers.map((sm) => {
    const u = byThree.get(sm.name);
    if (intSamplers.includes(sm)) {
      const words = new Uint32Array(new Uint8Array(sm.bytes).buffer);
      // UINT16: the values as 16-bit integers (exact, they fit); BITS32: the same bytes, read as RGBA float words
      if (intRoute(sm) === 'UINT16') return { uniform: u?.godot ?? sm.name, width: sm.width, height: sm.height, format: 'UINT16', bytes: Buffer.from(Uint16Array.from(words).buffer).toString('base64') };
      return { uniform: u?.godot ?? sm.name, width: sm.width, height: sm.height, format: 'RGBAF', bytes: Buffer.from(sm.bytes).toString('base64') };
    }
    return { uniform: u?.godot ?? sm.name, width: sm.width, height: sm.height, format: FORMATS[`${sm.format}:${sm.type}`] ?? null, bytes: Buffer.from(sm.bytes).toString('base64') };
  });
  // the mesh as the translation placed each attribute: CUSTOMn components, INSTANCE_CUSTOM, a data texture, bones
  const vertexCount = r.mesh.position.length / 3;
  const customs = [null, null, null, null];
  const instanceCount = r.instances ? r.instances.length : 0;
  const instanceCustom = instanceCount ? Array.from({ length: instanceCount }, () => [0, 0, 0, 0]) : null;
  let bones = null, weights = null;
  for (const a of s.attributes) {
    const src = r.attributes[a.three];
    if (!src) continue;
    const comps = (i) => src.array.slice(i * src.itemSize, i * src.itemSize + src.itemSize);
    let m;
    if ((m = /^CUSTOM(\d)\.([xyzw]+)$/.exec(a.godot))) {
      const k = Number(m[1]); const at = 'xyzw'.indexOf(m[2][0]);
      customs[k] ??= new Array(vertexCount * 4).fill(0);
      for (let i = 0; i < vertexCount; i++) comps(i).forEach((x, j) => { customs[k][i * 4 + at + j] = x; });
    } else if ((m = /^INSTANCE_CUSTOM\.([xyzw]+)$/.exec(a.godot))) {
      const at = 'xyzw'.indexOf(m[1][0]);
      for (let i = 0; i < instanceCount; i++) comps(i).forEach((x, j) => { instanceCustom[i][at + j] = x; });
    } else if ((m = /texelFetch\((_three_attr_\w+), ivec2\((VERTEX_ID|INSTANCE_ID)/.exec(a.godot))) {
      const count = m[2] === 'VERTEX_ID' ? vertexCount : instanceCount;
      const w = Math.min(4096, count), h = Math.ceil(count / 4096);
      const data = new Float32Array(w * h * 4);
      for (let i = 0; i < count; i++) comps(i).forEach((x, j) => { data[i * 4 + j] = x; });
      textures.push({ uniform: m[1], width: w, height: h, format: 'RGBAF', bytes: Buffer.from(data.buffer).toString('base64') });
    } else if (a.godot === 'vec4(BONE_INDICES)') bones = src.array;
    else if (a.godot === 'BONE_WEIGHTS') weights = src.array;
  }
  if (instanceCustom) r.instances.forEach((x, i) => { x.custom = instanceCustom[i]; });
  r.mesh.customs = customs; r.mesh.bones = bones; r.mesh.weights = weights;
  // the vertex colours travel in a CUSTOM slot (ARRAY_COLOR is 8 bits): none in ARRAY_COLOR, where Godot would
  // multiply them into the instance colour that COLOR carries
  if (!s.attributes.some((a) => a.three === 'color' && a.godot.startsWith('COLOR'))) r.mesh.color = null;
  godotCases.push({ id: r.id, shader: `res://shaders/${r.id}.gdshader`, params, mesh: r.mesh, transform: r.transform, instances: r.instances, textures, notes: s.notes, unsupported: s.unsupported });
}
fs.writeFileSync(path.join(outDir, 'cases.json'), JSON.stringify(godotCases));
fs.writeFileSync(path.join(outDir, 'three.json'), JSON.stringify(threeResults.map((r) => ({ id: r.id, pixels: r.pixels }))));

// ---- Godot: the same scene in a SubViewport, read back
fs.writeFileSync(path.join(outDir, 'project.godot'), ['config_version=5', '', '[application]', 'config/name="three parity"', '', '[rendering]', 'renderer/rendering_method="forward_plus"', ''].join('\n'));
// parity.gd and the owner of the captured values it reads (three_values.gd, shared with replay.gd)
for (const script of ['parity.gd', 'three_values.gd']) fs.writeFileSync(path.join(outDir, script), fs.readFileSync(path.resolve(import.meta.dirname, script), 'utf8'));
const godot = machine().get('godot');
const win = path.resolve(outDir);
// a hidden task (tools/tasks), keyed by what Godot reads: the cases, the shaders, the script; its images restored on a hit
const parityRun = runTasksSync([{ name: 'converter-parity-godot', cwd: win, hidden: true, timeoutS: PARITY_TIMEOUT_S,
  outputs: godotCases.map((c) => path.join(win, `godot_${c.id}.rgba`)),
  extra: [keyOf(filesKey(['cases.json', 'parity.gd', 'three_values.gd', 'project.godot'].map((f) => path.join(win, f))), folderKey(path.join(win, 'shaders'))), toolKey(godot)],
  argv: [godot, '--path', win, '--script', 'res://parity.gd', '--rendering-driver', 'vulkan'] }])[0];
mustRun('Godot\'s parity run', parityRun);
fs.writeFileSync(path.join(win, 'godot.log'), logOf(parityRun));

// ---- compare
const report = [];
for (const r of threeResults) {
  const file = path.join(outDir, `godot_${r.id}.rgba`);
  if (!fs.existsSync(file)) { report.push({ id: r.id, pass: false, reason: 'Godot produced no image (see godot.log)' }); continue; }
  const g = fs.readFileSync(file);
  let maxDiff = 0, over = 0, sum = 0;
  for (let y = 0; y < SIZE; y++) for (let x = 0; x < SIZE; x++) {
    const ti = ((SIZE - 1 - y) * SIZE + x) * 4; // three's rows are bottom-up
    const gi = (y * SIZE + x) * 4;
    let d = 0;
    for (let k = 0; k < 3; k++) d = Math.max(d, Math.abs(r.pixels[ti + k] - g[gi + k]));
    maxDiff = Math.max(maxDiff, d); sum += d; if (d > 2) over++;
  }
  const total = SIZE * SIZE;
  // strict: every pixel within 2/255
  report.push({ id: r.id, pass: over === 0, maxDiff, meanDiff: +(sum / total).toFixed(3), pixelsOver2: over, centerThree: Array.from(r.pixels.slice(((SIZE / 2) * SIZE + SIZE / 2) * 4, ((SIZE / 2) * SIZE + SIZE / 2) * 4 + 3)), centerGodot: Array.from(g.subarray(((SIZE / 2 - 1) * SIZE + SIZE / 2) * 4, ((SIZE / 2 - 1) * SIZE + SIZE / 2) * 4 + 3)) });
}
fs.writeFileSync(path.join(outDir, 'parity.json'), JSON.stringify(report, null, 1));
const passed = report.filter((x) => x.pass).length;
console.log(`PARITY three.js vs Godot: ${passed}/${report.length} cases equal (every pixel within 2/255)`);
for (const x of report) console.log(`  ${x.pass ? 'PASS' : 'FAIL'} ${x.id.padEnd(22)} max ${x.maxDiff ?? '-'} mean ${x.meanDiff ?? '-'} over2 ${x.pixelsOver2 ?? '-'} ${x.reason ?? ''} three ${x.centerThree ?? ''} godot ${x.centerGodot ?? ''}`);
