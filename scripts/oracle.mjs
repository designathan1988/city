// The runtime oracle of a trace: the 4.0 run in Chrome (its copy's dev server), with what V8 executed (precise
// coverage: every function, with its count), who called whom (a CPU profile: sampled stacks, each one a real call
// path), every shader the GPU was given (gl.shaderSource) and every code made from a string (Function, eval).
// `node scripts/oracle.mjs <url> <out dir> <playwright dir>`
import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';

const [url, out, pwDir] = process.argv.slice(2);
if (!url || !out || !pwDir) { console.error('usage: oracle.mjs <url> <out> <playwright package dir>'); process.exit(2); }
const { chromium } = createRequire(path.join(pwDir, 'package.json'))('playwright');
fs.mkdirSync(out, { recursive: true });

const browser = await chromium.launch({ channel: 'chrome', headless: true, args: ['--enable-gpu', '--ignore-gpu-blocklist'] });
const context = await browser.newContext({ viewport: { width: 1600, height: 900 } });
await context.addInitScript(() => {
  const shaders = (window.__oracleShaders = new Map());
  // each linked program as its pair of stages (Godot's shader is one file holding both)
  const programs = (window.__oraclePrograms = new Map());
  const sourceOf = new WeakMap();
  const attached = new WeakMap();
  for (const C of [window.WebGL2RenderingContext, window.WebGLRenderingContext]) {
    if (!C) continue;
    const original = C.prototype.shaderSource;
    C.prototype.shaderSource = function (shader, source) {
      shaders.set(source, (shaders.get(source) ?? 0) + 1);
      sourceOf.set(shader, source);
      return original.call(this, shader, source);
    };
    const attach = C.prototype.attachShader;
    C.prototype.attachShader = function (program, shader) {
      (attached.get(program) ?? attached.set(program, []).get(program)).push(shader);
      return attach.call(this, program, shader);
    };
    const link = C.prototype.linkProgram;
    const entryOf = new WeakMap();
    C.prototype.linkProgram = function (program) {
      const stages = (attached.get(program) ?? []).map((s) => sourceOf.get(s) ?? '');
      const vertex = stages.find((s) => /\bgl_Position\b/.test(s)) ?? stages[0] ?? '';
      const fragment = stages.find((s) => s !== vertex) ?? '';
      const key = vertex + '\u0000' + fragment;
      const entry = programs.get(key) ?? { vertex, fragment, instanced: [] };
      programs.set(key, entry);
      entryOf.set(program, entry);
      return link.call(this, program);
    };
    // which attributes each program reads per instance: the divisor set while it is in use
    let currentProgram = null;
    const attribNames = new WeakMap();
    const use = C.prototype.useProgram;
    C.prototype.useProgram = function (p) { currentProgram = p; return use.call(this, p); };
    const getAttrib = C.prototype.getAttribLocation;
    C.prototype.getAttribLocation = function (p, name) {
      const loc = getAttrib.call(this, p, name);
      if (loc >= 0) (attribNames.get(p) ?? attribNames.set(p, new Map()).get(p)).set(loc, name);
      return loc;
    };
    if (C.prototype.vertexAttribDivisor) {
      const divisor = C.prototype.vertexAttribDivisor;
      C.prototype.vertexAttribDivisor = function (index, d) {
        const name = currentProgram && attribNames.get(currentProgram)?.get(index);
        const entry = currentProgram && entryOf.get(currentProgram);
        if (name && entry && d > 0 && !entry.instanced.includes(name)) entry.instanced.push(name);
        return divisor.call(this, index, d);
      };
    }
  }
  const strings = (window.__oracleStrings = []);
  const F = window.Function;
  window.Function = new Proxy(F, {
    construct(t, a) { strings.push({ how: 'Function', body: String(a.at(-1)), stack: new Error().stack }); return Reflect.construct(t, a); },
    apply(t, s, a) { strings.push({ how: 'Function', body: String(a.at(-1)), stack: new Error().stack }); return Reflect.apply(t, s, a); },
  });
  const ev = window.eval;
  window.eval = function (code) { strings.push({ how: 'eval', body: String(code), stack: new Error().stack }); return ev(code); };
});
const page = await context.newPage();
const errors = [];
page.on('pageerror', (e) => errors.push(e.message));
const cdp = await context.newCDPSession(page);
await cdp.send('Debugger.enable');
const scripts = new Map();
cdp.on('Debugger.scriptParsed', (s) => scripts.set(s.scriptId, s.url));
await cdp.send('Profiler.enable');
await cdp.send('Profiler.startPreciseCoverage', { callCount: true, detailed: true });
await cdp.send('Profiler.setSamplingInterval', { interval: 50 });
await cdp.send('Profiler.start');

const t0 = Date.now();
await page.goto(url, { waitUntil: 'load' });
await page.waitForFunction(() => Boolean(window.__roadcraft?.memory), null, { timeout: 180_000 });
const gpu = await page.evaluate(() => {
  const gl = document.createElement('canvas').getContext('webgl2');
  const info = gl?.getExtension('WEBGL_debug_renderer_info');
  return info ? String(gl.getParameter(info.UNMASKED_RENDERER_WEBGL)) : 'no WebGL2';
});
console.log(`GPU: ${gpu}; page ready in ${((Date.now() - t0) / 1000).toFixed(1)} s`);
await page.evaluate(async () => { await window.__roadcraft.generateCity({ size: 'small', seed: 7 }); performance.clearMarks('opening:shown'); });
await page.waitForFunction(() => performance.getEntriesByName('opening:shown').length > 0 || (window.__roadcraft?.memory?.().drawCalls ?? 0) > 100,
  null, { timeout: 1_800_000, polling: 1000 });
console.log(`city ready in ${((Date.now() - t0) / 1000).toFixed(1)} s`);
// Views close to the ground along the streets, where poles are drawn, each held for frames to be rendered.
const views = JSON.parse(process.env.ORACLE_VIEWS ?? '[]');
const plan = views.length > 0 ? views : await page.evaluate(() => {
  const doc = window.__roadcraft.doc;
  const pts = [];
  const map = doc?.poleMap ?? doc?.poles ?? doc?.nodes;
  const nodes = map ? [...(map.values?.() ?? Object.values(map))] : [];
  for (let i = 0; i < nodes.length && pts.length < 6; i += Math.max(1, Math.floor(nodes.length / 6))) {
    const n = nodes[i];
    const x = n.x ?? n.pos?.x ?? n.p?.x; const y = n.y ?? n.pos?.y ?? n.p?.y;
    if (Number.isFinite(x) && Number.isFinite(y)) pts.push([x, y, 60]);
  }
  return pts;
});
console.log(`views: ${JSON.stringify(plan)}`);
for (const [x, y, zoom] of plan) {
  for (const [az, el] of [[30, 25], [210, 15]]) {
    await page.evaluate(([x, y, zoom, az, el]) => window.__roadcraft.setView({ x, y, zoom, azimuth: (az * Math.PI) / 180, elevation: (el * Math.PI) / 180 }), [x, y, zoom, az, el]);
    await page.evaluate(() => new Promise((r) => { let n = 0; const f = () => (++n >= 30 ? r() : requestAnimationFrame(f)); requestAnimationFrame(f); }));
  }
}
await page.screenshot({ path: path.join(out, 'last-view.png') });

const { profile } = await cdp.send('Profiler.stop');
const { result: coverage } = await cdp.send('Profiler.takePreciseCoverage');
const sources = {};
for (const s of coverage) {
  if (!s.url.includes('/src/')) continue;
  try { sources[s.scriptId] = { url: s.url, text: (await cdp.send('Debugger.getScriptSource', { scriptId: s.scriptId })).scriptSource }; } catch { /* gone */ }
}
const shaders = await page.evaluate(() => [...window.__oracleShaders].map(([source, count]) => ({ source, count })));
const strings = await page.evaluate(() => window.__oracleStrings);
const programs = await page.evaluate(() => [...window.__oraclePrograms.values()]);
fs.writeFileSync(path.join(out, 'programs.json'), JSON.stringify(programs));
console.log(`linked programs: ${programs.length}`);
fs.writeFileSync(path.join(out, 'coverage.json'), JSON.stringify(coverage));
fs.writeFileSync(path.join(out, 'profile.json'), JSON.stringify(profile));
fs.writeFileSync(path.join(out, 'sources.json'), JSON.stringify(sources));
fs.writeFileSync(path.join(out, 'shaders.json'), JSON.stringify(shaders));
fs.writeFileSync(path.join(out, 'strings.json'), JSON.stringify(strings));
fs.writeFileSync(path.join(out, 'run.json'), JSON.stringify({ url, gpu, views: plan, errors, seconds: (Date.now() - t0) / 1000 }, null, 1));
console.log(`coverage: ${coverage.length} scripts; profile: ${profile.nodes.length} nodes, ${profile.samples.length} samples; shaders: ${shaders.length}; code from strings: ${strings.length}; page errors: ${errors.length}`);
await browser.close();
