// FRAME CAPTURE of the running 4.0 (the approach of GFXReconstruct and Spector.js): every draw call of one frame,
// with everything it reads, so each draw can be replayed alone by WebGL and by Godot and the two compared.
// The state is not rebuilt from hooks: it is queried from GL at the draw (getUniform, getVertexAttrib,
// getBufferSubData, getParameter, getTexParameter), so it is what the GPU used. Hooks only see what GL cannot be
// asked: the programs' sources and each texture's internal format and size.
// Textures are read back from the GPU (render targets and shadow maps included): a colour-renderable level by
// readPixels of a framebuffer holding it; a depth or compressed one through a copy pass (texelFetch into RGBA32F).
// Blobs go to a local server as they are read (no data through the DevTools protocol).
// `node scripts/frame-capture.mjs <game url> <out dir> <playwright dir> [--views <json>] [--max-draws <n>]`
import fs from 'node:fs';
import http from 'node:http';
import path from 'node:path';
import { createRequire } from 'node:module';

const [url, outDir, pwDir] = process.argv.slice(2);
const flag = (n, d) => { const i = process.argv.indexOf(n); return i >= 0 ? process.argv[i + 1] : d; };
const maxDraws = Number(flag('--max-draws', '4000'));
const { chromium } = createRequire(path.join(pwDir, 'package.json'))('playwright');
fs.rmSync(outDir, { recursive: true, force: true });
fs.mkdirSync(path.join(outDir, 'blobs'), { recursive: true });
const t0 = Date.now();
const say = (m) => console.log(`[${((Date.now() - t0) / 1000).toFixed(1)} s] ${m}`);

// blobs posted by the page
let blobBytes = 0;
const server = http.createServer((req, res) => {
  const name = decodeURIComponent((req.url ?? '').replace(/^\/blob\//, ''));
  const chunks = [];
  req.on('data', (c) => chunks.push(c));
  req.on('end', () => {
    const buf = Buffer.concat(chunks);
    blobBytes += buf.length;
    fs.writeFileSync(path.join(outDir, 'blobs', name.replace(/[^\w.-]/g, '_')), buf);
    res.setHeader('access-control-allow-origin', '*');
    res.end('ok');
  });
});
await new Promise((r) => server.listen(0, '127.0.0.1', r));
const blobUrl = `http://127.0.0.1:${server.address().port}/blob/`;

const browser = await chromium.launch({ channel: 'chrome', headless: true, args: ['--enable-gpu', '--ignore-gpu-blocklist'] });
const context = await browser.newContext({ viewport: { width: 1280, height: 720 } });
await context.addInitScript(() => {
  const C = WebGL2RenderingContext.prototype;
  const ids = new WeakMap(); let nextId = 1;
  const idOf = (o) => { if (!o) return 0; let i = ids.get(o); if (!i) { i = nextId++; ids.set(o, i); } return i; };
  const src = new WeakMap(); const attached = new WeakMap(); const texAlloc = new WeakMap();
  const wrap = (m, f) => { const o = C[m]; if (!o) return; C[m] = function (...a) { f.call(this, ...a); return o.apply(this, a); }; };
  wrap('shaderSource', (s, t) => src.set(s, t));
  wrap('attachShader', (p, s) => (attached.get(p) ?? attached.set(p, []).get(p)).push(s));
  // a texture's internal format, size and levels: what GL does not answer
  const boundTex = (gl, target) => gl.getParameter(target === gl.TEXTURE_2D ? gl.TEXTURE_BINDING_2D : target === gl.TEXTURE_2D_ARRAY ? gl.TEXTURE_BINDING_2D_ARRAY : target === gl.TEXTURE_3D ? gl.TEXTURE_BINDING_3D : gl.TEXTURE_BINDING_CUBE_MAP);
  const note = (gl, target, info) => { const t = boundTex(gl, target); if (t) texAlloc.set(t, { ...(texAlloc.get(t) ?? {}), ...info, target }); };
  wrap('texStorage2D', function (target, levels, internalformat, width, height) { note(this, target, { levels, internalformat, width, height, depth: 1 }); });
  wrap('texStorage3D', function (target, levels, internalformat, width, height, depth) { note(this, target, { levels, internalformat, width, height, depth }); });
  wrap('texImage2D', function (target, level, internalformat, ...rest) {
    if (level !== 0) { const t = boundTex(this, target); const a = t && texAlloc.get(t); if (a) a.levels = Math.max(a.levels ?? 1, level + 1); return; }
    let width = rest[0], height = rest[1];
    if (typeof width !== 'number') { const s = rest[rest.length - 1] ?? rest[0]; width = s?.width ?? s?.videoWidth ?? 0; height = s?.height ?? s?.videoHeight ?? 0; }
    note(this, target, { levels: 1, internalformat, width, height, depth: 1 });
  });
  wrap('texImage3D', function (target, level, internalformat, width, height, depth) { if (level === 0) note(this, target, { levels: 1, internalformat, width, height, depth }); });
  wrap('compressedTexImage2D', function (target, level, internalformat, width, height) { if (level === 0) note(this, target, { levels: 1, internalformat, width, height, depth: 1, compressed: true }); else { const t = boundTex(this, target); const a = t && texAlloc.get(t); if (a) a.levels = Math.max(a.levels ?? 1, level + 1); } });
  wrap('generateMipmap', function (target) { const t = boundTex(this, target); const a = t && texAlloc.get(t); if (a) a.levels = Math.floor(Math.log2(Math.max(a.width, a.height))) + 1; });

  const cap = (window.__frameCapture = { on: false, draws: [], programs: new Map(), textures: new Map(), buffers: new Map(), max: 4000 });
  const P = (gl, n) => gl.getParameter(n);
  const snapshot = function (gl, mode, count, type, offset, instances) {
    if (cap.draws.length >= cap.max) return;
    const program = P(gl, gl.CURRENT_PROGRAM);
    if (!program) return;
    const pid = idOf(program);
    if (!cap.programs.has(pid)) {
      const stages = (attached.get(program) ?? []).map((s) => src.get(s) ?? '');
      cap.programs.set(pid, { vertex: stages.find((s) => /\bgl_Position\b/.test(s)) ?? stages[0] ?? '', fragment: stages.find((s) => !/\bgl_Position\b/.test(s)) ?? '' });
    }
    // uniforms: each active one (arrays element by element), and each sampler's texture
    const uniforms = {}; const samplers = [];
    const nu = gl.getProgramParameter(program, gl.ACTIVE_UNIFORMS);
    const active = P(gl, gl.ACTIVE_TEXTURE);
    for (let i = 0; i < nu; i++) {
      const info = gl.getActiveUniform(program, i);
      if (!info) continue;
      const base = info.name.replace(/\[0\]$/, '');
      const isSampler = [gl.SAMPLER_2D, gl.SAMPLER_2D_ARRAY, gl.SAMPLER_3D, gl.SAMPLER_CUBE, gl.SAMPLER_2D_SHADOW, gl.SAMPLER_2D_ARRAY_SHADOW, gl.SAMPLER_CUBE_SHADOW, gl.INT_SAMPLER_2D, gl.UNSIGNED_INT_SAMPLER_2D, gl.INT_SAMPLER_2D_ARRAY, gl.UNSIGNED_INT_SAMPLER_2D_ARRAY].includes(info.type);
      for (let k = 0; k < info.size; k++) {
        const name = info.size > 1 || /\[0\]$/.test(info.name) ? `${base}[${k}]` : base;
        const loc = gl.getUniformLocation(program, name);
        if (!loc) continue;
        const v = gl.getUniform(program, loc);
        const data = v === null || v === undefined ? null : typeof v === 'number' || typeof v === 'boolean' ? [Number(v)] : Array.from(v, Number);
        uniforms[name] = { type: info.type, data };
        if (isSampler && data) {
          const unit = data[0];
          gl.activeTexture(gl.TEXTURE0 + unit);
          const is2DArray = [gl.SAMPLER_2D_ARRAY, gl.SAMPLER_2D_ARRAY_SHADOW, gl.INT_SAMPLER_2D_ARRAY, gl.UNSIGNED_INT_SAMPLER_2D_ARRAY].includes(info.type);
          const target = is2DArray ? gl.TEXTURE_2D_ARRAY : gl.TEXTURE_2D;
          const tex = P(gl, is2DArray ? gl.TEXTURE_BINDING_2D_ARRAY : gl.TEXTURE_BINDING_2D);
          const params = tex ? {
            min: gl.getTexParameter(target, gl.TEXTURE_MIN_FILTER), mag: gl.getTexParameter(target, gl.TEXTURE_MAG_FILTER),
            wrapS: gl.getTexParameter(target, gl.TEXTURE_WRAP_S), wrapT: gl.getTexParameter(target, gl.TEXTURE_WRAP_T),
            compareMode: gl.getTexParameter(target, gl.TEXTURE_COMPARE_MODE), compareFunc: gl.getTexParameter(target, gl.TEXTURE_COMPARE_FUNC),
            // the rest of the sampler state: anisotropy (three's texture.anisotropy), the LOD range and the levels
            wrapR: gl.getTexParameter(target, gl.TEXTURE_WRAP_R),
            minLod: gl.getTexParameter(target, gl.TEXTURE_MIN_LOD), maxLod: gl.getTexParameter(target, gl.TEXTURE_MAX_LOD),
            baseLevel: gl.getTexParameter(target, gl.TEXTURE_BASE_LEVEL), maxLevel: gl.getTexParameter(target, gl.TEXTURE_MAX_LEVEL),
            anisotropy: gl.getExtension('EXT_texture_filter_anisotropic') ? gl.getTexParameter(target, 0x84fe) : 1,
          } : null;
          const tid = idOf(tex);
          if (tex && !cap.textures.has(tid)) cap.textures.set(tid, { tex, target, alloc: texAlloc.get(tex) ?? null });
          samplers.push({ name, unit, texture: tid, target, samplerType: info.type, params });
        }
      }
    }
    gl.activeTexture(active);
    // attributes: each active one's pointer, buffer and divisor, as the vertex array holds them
    const attributes = [];
    const na = gl.getProgramParameter(program, gl.ACTIVE_ATTRIBUTES);
    for (let i = 0; i < na; i++) {
      const info = gl.getActiveAttrib(program, i);
      if (!info || info.name.startsWith('gl_')) continue;
      const loc = gl.getAttribLocation(program, info.name);
      const rows = info.type === gl.FLOAT_MAT4 ? 4 : info.type === gl.FLOAT_MAT3 ? 3 : info.type === gl.FLOAT_MAT2 ? 2 : 1;
      const parts = [];
      for (let r = 0; r < rows; r++) {
        const l = loc + r;
        const enabled = gl.getVertexAttrib(l, gl.VERTEX_ATTRIB_ARRAY_ENABLED);
        const buffer = gl.getVertexAttrib(l, gl.VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);
        const bid = idOf(buffer);
        if (buffer && !cap.buffers.has(bid)) cap.buffers.set(bid, { buffer });
        parts.push({ location: l, enabled, buffer: bid, size: gl.getVertexAttrib(l, gl.VERTEX_ATTRIB_ARRAY_SIZE), type: gl.getVertexAttrib(l, gl.VERTEX_ATTRIB_ARRAY_TYPE),
          normalized: gl.getVertexAttrib(l, gl.VERTEX_ATTRIB_ARRAY_NORMALIZED), stride: gl.getVertexAttrib(l, gl.VERTEX_ATTRIB_ARRAY_STRIDE),
          integer: gl.getVertexAttrib(l, gl.VERTEX_ATTRIB_ARRAY_INTEGER), divisor: gl.getVertexAttrib(l, gl.VERTEX_ATTRIB_ARRAY_DIVISOR),
          offset: gl.getVertexAttribOffset(l, gl.VERTEX_ATTRIB_ARRAY_POINTER), current: enabled ? null : Array.from(gl.getVertexAttrib(l, gl.CURRENT_VERTEX_ATTRIB)) });
      }
      attributes.push({ name: info.name, type: info.type, rows, parts });
    }
    const elements = P(gl, gl.ELEMENT_ARRAY_BUFFER_BINDING);
    const eid = idOf(elements);
    if (elements && !cap.buffers.has(eid)) cap.buffers.set(eid, { buffer: elements });
    const fb = P(gl, gl.DRAW_FRAMEBUFFER_BINDING);
    let target = { default: !fb, width: gl.drawingBufferWidth, height: gl.drawingBufferHeight, color: !fb, depth: !fb };
    if (fb) {
      const att = (a, p) => gl.getFramebufferAttachmentParameter(gl.DRAW_FRAMEBUFFER, a, p);
      const colorType = att(gl.COLOR_ATTACHMENT0, gl.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
      const depthType = att(gl.DEPTH_ATTACHMENT, gl.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
      const c1 = att(gl.COLOR_ATTACHMENT1, gl.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
      target = { default: false, color: colorType !== gl.NONE, depth: depthType !== gl.NONE, outputs: c1 !== gl.NONE ? 2 : 1 };
      // the colour attachment's internal format (a float target keeps values a byte target clamps): from the
      // attached texture's allocation, or the renderbuffer's own
      if (colorType === gl.TEXTURE) {
        const tex = att(gl.COLOR_ATTACHMENT0, gl.FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
        const a = tex && texAlloc.get(tex);
        if (a) target.format = a.internalformat;
      } else if (colorType === gl.RENDERBUFFER) {
        const rb = att(gl.COLOR_ATTACHMENT0, gl.FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
        const prev = P(gl, gl.RENDERBUFFER_BINDING);
        gl.bindRenderbuffer(gl.RENDERBUFFER, rb);
        target.format = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
        gl.bindRenderbuffer(gl.RENDERBUFFER, prev);
      }
      // the component type: float (also half float) or not
      if (colorType !== gl.NONE) target.componentType = att(gl.COLOR_ATTACHMENT0, gl.FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE);
    }
    const vp = Array.from(P(gl, gl.VIEWPORT));
    cap.draws.push({
      index: cap.draws.length, program: pid, mode, count, type, offset, instances, elements: eid, uniforms, samplers, attributes, target, viewport: vp,
      state: { blend: P(gl, gl.BLEND), blendSrc: P(gl, gl.BLEND_SRC_RGB), blendDst: P(gl, gl.BLEND_DST_RGB), blendSrcA: P(gl, gl.BLEND_SRC_ALPHA), blendDstA: P(gl, gl.BLEND_DST_ALPHA), blendEq: P(gl, gl.BLEND_EQUATION_RGB),
        depthTest: P(gl, gl.DEPTH_TEST), depthFunc: P(gl, gl.DEPTH_FUNC), depthMask: P(gl, gl.DEPTH_WRITEMASK), cull: P(gl, gl.CULL_FACE), cullFace: P(gl, gl.CULL_FACE_MODE), frontFace: P(gl, gl.FRONT_FACE),
        colorMask: Array.from(P(gl, gl.COLOR_WRITEMASK)), polygonOffset: P(gl, gl.POLYGON_OFFSET_FILL) },
    });
  };
  for (const [m, f] of [['drawArrays', (a) => [a[0], a[2], 0, a[1], 1]], ['drawElements', (a) => [a[0], a[1], a[2], a[3], 1]],
    ['drawArraysInstanced', (a) => [a[0], a[2], 0, a[1], a[3]]], ['drawElementsInstanced', (a) => [a[0], a[1], a[2], a[3], a[4]]]]) {
    const o = C[m];
    C[m] = function (...a) { if (cap.on) { try { snapshot(this, ...f(a)); } catch (e) { cap.error = String(e); } } return o.apply(this, a); };
  }
});
const page = await context.newPage();
const errors = [];
page.on('pageerror', (e) => errors.push(e.message));
await page.goto(url, { waitUntil: 'load', timeout: 600_000 });
await page.waitForFunction(() => Boolean(window.__roadcraft?.memory), null, { timeout: 600_000, polling: 1000 });
say('page ready');
await page.evaluate(async () => { await window.__roadcraft.generateCity({ size: 'small', seed: 7 }); performance.clearMarks('opening:shown'); });
await page.waitForFunction(() => performance.getEntriesByName('opening:shown').length > 0 || (window.__roadcraft?.memory?.().drawCalls ?? 0) > 100, null, { timeout: 1_800_000, polling: 1000 });
say('city ready');
// the view: [x, y, zoom, azimuth°, elevation°]; x and y null (or no --views) = the default anchor (a pole a third
// of the way through the map), so other cameras over the same place cover the draws one view leaves off screen
const anchor = await page.evaluate(() => { const all = [...window.__roadcraft.doc.poleMap.values()]; const p = all[Math.floor(all.length / 3)]; return [p.x, p.y]; });
const given = JSON.parse(flag('--views', 'null') ?? 'null');
const view = given ? [given[0] ?? anchor[0], given[1] ?? anchor[1], given[2], given[3], given[4]] : [anchor[0], anchor[1], 90, 35, 20];
await page.evaluate(([x, y, zoom, az, el]) => window.__roadcraft.setView({ x, y, zoom, azimuth: (az * Math.PI) / 180, elevation: (el * Math.PI) / 180 }), view);
await page.evaluate(() => new Promise((r) => { let n = 0; const f = () => (++n >= 90 ? r() : requestAnimationFrame(f)); requestAnimationFrame(f); }));
await page.screenshot({ path: path.join(outDir, 'game.png') });
// one frame captured
const summary = await page.evaluate(async ({ maxDraws, blobUrl }) => {
  const cap = window.__frameCapture;
  cap.max = maxDraws;
  await new Promise((r) => requestAnimationFrame(() => { cap.on = true; requestAnimationFrame(() => { cap.on = false; r(); }); }));
  const canvas = document.querySelector('canvas');
  const gl = canvas.getContext('webgl2');
  gl.getExtension('EXT_color_buffer_float');
  const post = (name, data) => fetch(blobUrl + encodeURIComponent(name), { method: 'POST', body: data });
  const pending = [];
  // buffers, read back
  for (const [bid, b] of cap.buffers) {
    gl.bindBuffer(gl.COPY_READ_BUFFER, b.buffer);
    const size = gl.getBufferParameter(gl.COPY_READ_BUFFER, gl.BUFFER_SIZE);
    const data = new Uint8Array(size);
    gl.getBufferSubData(gl.COPY_READ_BUFFER, 0, data);
    b.size = size;
    pending.push(post(`buffer_${bid}.bin`, data));
  }
  gl.bindBuffer(gl.COPY_READ_BUFFER, null);
  // textures, read back level by level (and layer by layer)
  const GLF = gl;
  const intFormats = new Set([GLF.RGBA8UI, GLF.RGBA16UI, GLF.RGBA32UI, GLF.R8UI, GLF.R16UI, GLF.R32UI, GLF.RG8UI, GLF.RG16UI, GLF.RG32UI]);
  const sintFormats = new Set([GLF.RGBA8I, GLF.RGBA16I, GLF.RGBA32I, GLF.R8I, GLF.R16I, GLF.R32I, GLF.RG8I, GLF.RG16I, GLF.RG32I]);
  const floatFormats = new Set([GLF.RGBA16F, GLF.RGBA32F, GLF.R16F, GLF.R32F, GLF.RG16F, GLF.RG32F, GLF.R11F_G11F_B10F, GLF.RGB16F, GLF.RGB32F]);
  const depthFormats = new Set([GLF.DEPTH_COMPONENT16, GLF.DEPTH_COMPONENT24, GLF.DEPTH_COMPONENT32F, GLF.DEPTH24_STENCIL8, GLF.DEPTH32F_STENCIL8]);
  const fbo = gl.createFramebuffer();
  // the copy pass for what cannot be read directly (depth, compressed): texelFetch into an RGBA32F target
  const mk = (t, s) => { const x = gl.createShader(t); gl.shaderSource(x, s); gl.compileShader(x); return x; };
  const copyProgram = (sampler) => {
    const p = gl.createProgram();
    gl.attachShader(p, mk(gl.VERTEX_SHADER, '#version 300 es\nvoid main(){ vec2 v = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2)); gl_Position = vec4(v * 2.0 - 1.0, 0.0, 1.0); }'));
    gl.attachShader(p, mk(gl.FRAGMENT_SHADER, `#version 300 es\nprecision highp float; precision highp ${sampler}; uniform ${sampler} t; uniform int lod; uniform int layer; out vec4 o; void main(){ ivec2 c = ivec2(gl_FragCoord.xy); o = ${sampler.includes('Array') ? 'texelFetch(t, ivec3(c, layer), lod)' : 'texelFetch(t, c, lod)'}; }`));
    gl.linkProgram(p); return p;
  };
  const copy2D = copyProgram('sampler2D'); const copyArray = copyProgram('sampler2DArray');
  // integer textures: texelFetch into an integer target (a float sampler cannot read them)
  const copyInt = (sampler, out) => {
    const p = gl.createProgram();
    gl.attachShader(p, mk(gl.VERTEX_SHADER, '#version 300 es\nvoid main(){ vec2 v = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2)); gl_Position = vec4(v * 2.0 - 1.0, 0.0, 1.0); }'));
    gl.attachShader(p, mk(gl.FRAGMENT_SHADER, `#version 300 es\nprecision highp float; precision highp int; precision highp ${sampler}; uniform ${sampler} t; uniform int lod; uniform int layer; out ${out} o; void main(){ ivec2 c = ivec2(gl_FragCoord.xy); o = ${out}(${sampler.includes('Array') ? 'texelFetch(t, ivec3(c, layer), lod)' : 'texelFetch(t, c, lod)'}); }`));
    gl.linkProgram(p); return p;
  };
  const copyU = { T2D: copyInt('usampler2D', 'uvec4'), Array: copyInt('usampler2DArray', 'uvec4') };
  const copyI = { T2D: copyInt('isampler2D', 'ivec4'), Array: copyInt('isampler2DArray', 'ivec4') };
  const intCopy = (t, kind, level, layer, w, h) => {
    const target = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, target);
    gl.texStorage2D(gl.TEXTURE_2D, 1, kind === 'uint' ? gl.RGBA32UI : gl.RGBA32I, w, h);
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, target, 0);
    const p = (kind === 'uint' ? copyU : copyI)[t.target === gl.TEXTURE_2D_ARRAY ? 'Array' : 'T2D'];
    gl.useProgram(p); gl.bindVertexArray(vao);
    gl.activeTexture(gl.TEXTURE0); gl.bindTexture(t.target, t.tex);
    gl.uniform1i(gl.getUniformLocation(p, 't'), 0); gl.uniform1i(gl.getUniformLocation(p, 'lod'), level); gl.uniform1i(gl.getUniformLocation(p, 'layer'), layer);
    gl.disable(gl.BLEND); gl.disable(gl.DEPTH_TEST); gl.disable(gl.CULL_FACE); gl.disable(gl.SCISSOR_TEST); gl.colorMask(true, true, true, true);
    gl.viewport(0, 0, w, h);
    gl.drawArrays(gl.TRIANGLES, 0, 3);
    const data = kind === 'uint' ? new Uint32Array(w * h * 4) : new Int32Array(w * h * 4);
    gl.readPixels(0, 0, w, h, gl.RGBA_INTEGER, kind === 'uint' ? gl.UNSIGNED_INT : gl.INT, data);
    const err = gl.getError();
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, null, 0);
    gl.deleteTexture(target);
    return err === gl.NO_ERROR ? data : null;
  };
  const vao = gl.createVertexArray();
  const texInfo = {};
  for (const [tid, t] of cap.textures) {
    const a = t.alloc;
    if (!a) { texInfo[tid] = { error: 'allocation not seen' }; continue; }
    const kind = intFormats.has(a.internalformat) ? 'uint' : sintFormats.has(a.internalformat) ? 'int' : depthFormats.has(a.internalformat) ? 'depth' : floatFormats.has(a.internalformat) ? 'float' : a.compressed ? 'compressed' : 'unorm';
    const layers = t.target === gl.TEXTURE_2D_ARRAY ? a.depth : 1;
    const levels = a.levels ?? 1;
    texInfo[tid] = { ...a, kind, layers, levels, srgb: a.internalformat === gl.SRGB8_ALPHA8 || a.internalformat === gl.SRGB8, blobs: [] };
    for (let level = 0; level < levels; level++) {
      const w = Math.max(1, a.width >> level), h = Math.max(1, a.height >> level);
      for (let layer = 0; layer < layers; layer++) {
        let data = null;
        if (kind !== 'depth' && kind !== 'compressed') {
          gl.bindFramebuffer(gl.READ_FRAMEBUFFER, fbo);
          if (t.target === gl.TEXTURE_2D_ARRAY) gl.framebufferTextureLayer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, t.tex, level, layer);
          else gl.framebufferTexture2D(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, t.tex, level);
          if (gl.checkFramebufferStatus(gl.READ_FRAMEBUFFER) === gl.FRAMEBUFFER_COMPLETE) {
            if (kind === 'uint') { data = new Uint32Array(w * h * 4); gl.readPixels(0, 0, w, h, gl.RGBA_INTEGER, gl.UNSIGNED_INT, data); }
            else if (kind === 'int') { data = new Int32Array(w * h * 4); gl.readPixels(0, 0, w, h, gl.RGBA_INTEGER, gl.INT, data); }
            else if (kind === 'float') { data = new Float32Array(w * h * 4); gl.readPixels(0, 0, w, h, gl.RGBA, gl.FLOAT, data); }
            else { data = new Uint8Array(w * h * 4); gl.readPixels(0, 0, w, h, gl.RGBA, gl.UNSIGNED_BYTE, data); }
            // a refused read leaves the array as it was (zeros): never taken as the texture's content
            const err = gl.getError();
            if (err !== gl.NO_ERROR) { texInfo[tid].readErrors = [...(texInfo[tid].readErrors ?? []), `level ${level} layer ${layer}: readPixels GL error 0x${err.toString(16)}`]; data = null; }
          }
          gl.framebufferTexture2D(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, null, 0);
        }
        if (kind === 'uint' || kind === 'int') {
          // integer textures: read a second, independent way (a shader's texelFetch) and compared word for word
          const fetched = intCopy(t, kind, level, layer, w, h);
          if (!data) data = fetched;
          else if (fetched) {
            let same = true;
            for (let k = 0; k < data.length; k++) if (data[k] !== fetched[k]) { same = false; break; }
            texInfo[tid].crossChecked = (texInfo[tid].crossChecked ?? true) && same;
          }
          if (!data) texInfo[tid].readErrors = [...(texInfo[tid].readErrors ?? []), `level ${level} layer ${layer}: neither read succeeded`];
        }
        if (!data && kind !== 'uint' && kind !== 'int') {
          // copy pass: the texel values as the shader sees them (a depth texture with compare off)
          const target = gl.createTexture();
          gl.bindTexture(gl.TEXTURE_2D, target);
          gl.texStorage2D(gl.TEXTURE_2D, 1, gl.RGBA32F, w, h);
          gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
          gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, target, 0);
          const p = t.target === gl.TEXTURE_2D_ARRAY ? copyArray : copy2D;
          gl.useProgram(p); gl.bindVertexArray(vao);
          gl.activeTexture(gl.TEXTURE0); gl.bindTexture(t.target, t.tex);
          const cmp = gl.getTexParameter(t.target, gl.TEXTURE_COMPARE_MODE);
          gl.texParameteri(t.target, gl.TEXTURE_COMPARE_MODE, gl.NONE);
          gl.uniform1i(gl.getUniformLocation(p, 't'), 0); gl.uniform1i(gl.getUniformLocation(p, 'lod'), level); gl.uniform1i(gl.getUniformLocation(p, 'layer'), layer);
          gl.disable(gl.BLEND); gl.disable(gl.DEPTH_TEST); gl.disable(gl.CULL_FACE); gl.disable(gl.SCISSOR_TEST); gl.colorMask(true, true, true, true);
          gl.viewport(0, 0, w, h);
          gl.drawArrays(gl.TRIANGLES, 0, 3);
          gl.texParameteri(t.target, gl.TEXTURE_COMPARE_MODE, cmp);
          data = new Float32Array(w * h * 4);
          gl.readPixels(0, 0, w, h, gl.RGBA, gl.FLOAT, data);
          gl.deleteTexture(target);
          texInfo[tid].decoded = true; // values as sampled: an sRGB compressed texture is linear here
        }
        if (!data) continue; // readErrors says why: the replay refuses a texture without data
        const name = `texture_${tid}_l${level}_z${layer}.bin`;
        texInfo[tid].blobs.push({ level, layer, width: w, height: h, name, bytes: data.byteLength, element: data.constructor.name });
        pending.push(post(name, new Uint8Array(data.buffer)));
      }
    }
  }
  await Promise.all(pending);
  const programs = Object.fromEntries(cap.programs);
  const buffers = Object.fromEntries([...cap.buffers].map(([k, v]) => [k, { size: v.size }]));
  const index = { canvas: { width: canvas.width, height: canvas.height }, programs, buffers, textures: texInfo, draws: cap.draws, error: cap.error ?? null };
  await post('index.json', JSON.stringify(index));
  return { draws: cap.draws.length, programs: cap.programs.size, buffers: cap.buffers.size, textures: cap.textures.size, error: cap.error ?? null };
}, { maxDraws, blobUrl });
fs.renameSync(path.join(outDir, 'blobs', 'index.json'), path.join(outDir, 'index.json'));
say(`captured: ${summary.draws} draws, ${summary.programs} programs, ${summary.buffers} buffers, ${summary.textures} textures, ${(blobBytes / 2 ** 20).toFixed(0)} MB; page errors ${errors.length}${summary.error ? `; capture error: ${summary.error}` : ''}`);
await browser.close();
server.close();
