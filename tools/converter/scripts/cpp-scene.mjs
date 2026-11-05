// The C++ port of a captured frame: every draw (scripts/frame-replay.mjs's cases: the translated shader, the mesh as
// the translation placed its attributes, the instances, the textures in the formats three's GPU held, the uniforms)
// written as C++ code for the godot-cpp runtime (cpp/runtime): uniforms as typed Godot values (Vector3, Basis,
// Projection, packed arrays...), instances as Transform3D literals, textures and mesh arrays as the data files they
// are. Then compiled (llvm-mingw, against the prebuilt godot-cpp; objects cached by content, compiled in parallel)
// into a GDExtension, and run in Godot, watched live — the same replay as scripts/replay.gd, by C++.
// `node scripts/cpp-scene.mjs <replay dir> [--jobs n]` (the replay dir of scripts/frame-replay.mjs)
import crypto from 'node:crypto';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { execFile, execFileSync } from 'node:child_process';

const [replayDir] = process.argv.slice(2);
const flag = (n, d) => { const i = process.argv.indexOf(n); return i >= 0 ? process.argv[i + 1] : d; };
const here = path.resolve(import.meta.dirname, '..');
const t0 = Date.now();
const say = (m) => console.log(`[${((Date.now() - t0) / 1000).toFixed(1)} s] ${m}`);
const gDir = path.join(replayDir, 'godot');
const cases = JSON.parse(fs.readFileSync(path.join(gDir, 'cases.json'), 'utf8'));
const genDir = path.join(replayDir, 'cpp', 'gen');
fs.rmSync(genDir, { recursive: true, force: true });
fs.mkdirSync(genDir, { recursive: true });

// ---- C++ literals
const f = (x) => { const v = Number(x); if (!Number.isFinite(v)) return Number.isNaN(v) ? 'NAN' : v > 0 ? 'INFINITY' : '-INFINITY'; const s = String(Math.fround(v)); return /[.e]/.test(s) ? `${s}f` : `${s}.0f`; };
const str = (s) => JSON.stringify(String(s));
const one = (type, e) => {
  switch (type) {
    case 'float': return `Variant(${f(e[0])})`;
    case 'int': case 'uint': return `Variant(int64_t(${Math.trunc(e[0])}))`;
    case 'bool': return `Variant(${e[0] ? 'true' : 'false'})`;
    case 'vec2': return `Variant(Vector2(${f(e[0])}, ${f(e[1])}))`;
    case 'vec3': return `Variant(Vector3(${f(e[0])}, ${f(e[1])}, ${f(e[2])}))`;
    case 'vec4': return `Variant(Vector4(${f(e[0])}, ${f(e[1])}, ${f(e[2])}, ${f(e[3])}))`;
    case 'ivec2': case 'uvec2': return `Variant(Vector2i(${e.map(Math.trunc).slice(0, 2).join(', ')}))`;
    case 'ivec3': case 'uvec3': return `Variant(Vector3i(${e.map(Math.trunc).slice(0, 3).join(', ')}))`;
    case 'ivec4': case 'uvec4': return `Variant(Vector4i(${e.map(Math.trunc).slice(0, 4).join(', ')}))`;
    case 'mat2': return `Variant(Transform2D(Vector2(${f(e[0])}, ${f(e[1])}), Vector2(${f(e[2])}, ${f(e[3])}), Vector2()))`;
    case 'mat3': return `Variant(Basis(Vector3(${f(e[0])}, ${f(e[1])}, ${f(e[2])}), Vector3(${f(e[3])}, ${f(e[4])}, ${f(e[5])}), Vector3(${f(e[6])}, ${f(e[7])}, ${f(e[8])})))`;
    case 'mat4': return `Variant(Projection(Vector4(${f(e[0])}, ${f(e[1])}, ${f(e[2])}, ${f(e[3])}), Vector4(${f(e[4])}, ${f(e[5])}, ${f(e[6])}, ${f(e[7])}), Vector4(${f(e[8])}, ${f(e[9])}, ${f(e[10])}, ${f(e[11])}), Vector4(${f(e[12])}, ${f(e[13])}, ${f(e[14])}, ${f(e[15])})))`;
  }
  return null;
};
const value = (p) => {
  if (!p.array) return one(p.type, p.elements[0]);
  const els = p.elements;
  switch (p.type) {
    case 'float': return `Variant(PackedFloat32Array(std::initializer_list<float>{${els.map((e) => f(e[0])).join(', ')}}))`;
    case 'int': case 'uint': case 'bool': return `Variant(PackedInt32Array(std::initializer_list<int32_t>{${els.map((e) => Math.trunc(e[0])).join(', ')}}))`;
    case 'vec2': return `Variant(PackedVector2Array(std::initializer_list<Vector2>{${els.map((e) => `Vector2(${f(e[0])}, ${f(e[1])})`).join(', ')}}))`;
    case 'vec3': return `Variant(PackedVector3Array(std::initializer_list<Vector3>{${els.map((e) => `Vector3(${f(e[0])}, ${f(e[1])}, ${f(e[2])})`).join(', ')}}))`;
    case 'vec4': return `Variant(PackedVector4Array(std::initializer_list<Vector4>{${els.map((e) => `Vector4(${f(e[0])}, ${f(e[1])}, ${f(e[2])}, ${f(e[3])})`).join(', ')}}))`;
  }
  return `Variant([]() { Array a; ${els.map((e) => `a.push_back(${one(p.type, e)});`).join(' ')} return a; }())`;
};
// columns, as Godot's Basis(x, y, z) and GLSL's column-major data
const xf = (m) => `Transform3D(Basis(Vector3(${f(m[0])}, ${f(m[1])}, ${f(m[2])}), Vector3(${f(m[4])}, ${f(m[5])}, ${f(m[6])}), Vector3(${f(m[8])}, ${f(m[9])}, ${f(m[10])})), Vector3(${f(m[12])}, ${f(m[13])}, ${f(m[14])}))`;
const color = (c) => `Color(${f(c[0])}, ${f(c[1])}, ${f(c[2])}, ${f(c[3] ?? 1)})`;

// a C++ name for a case (ids like 12c3)
const fid = (c) => String(c.id ?? c.index).replace(/[^A-Za-z0-9_]/g, '_');
// ---- one function per draw, a file per 8 draws (compiled in parallel)
const fnOf = (c) => {
  const L = [];
  L.push(`void three_draw_${fid(c)}(three::Draw &d) {`);
  // the case's id: its draw, or <draw>c<k> (one component pass of a float target, its image kept as floats)
  L.push(`\td.id = ${str(c.id ?? c.index)}; d.bits = ${c.bits ? 'true' : 'false'};`);
  L.push(`\td.index = ${c.index}; d.shader = ${str(c.shader)}; d.width = ${c.size[0]}; d.height = ${c.size[1]}; d.anisotropy = ${c.anisotropy ?? 0};`);
  L.push(`\td.primitive = Mesh::${c.primitive === 'lines' ? 'PRIMITIVE_LINES' : c.primitive === 'points' ? 'PRIMITIVE_POINTS' : 'PRIMITIVE_TRIANGLES'}; d.vertex_count = ${c.vertexCount}; d.indices = ${str(c.indices)};`);
  for (const [k, v] of Object.entries(c.arrays)) L.push(`\td.${k} = ${str(v)};`);
  c.customs.forEach((v, k) => { if (v) L.push(`\td.customs[${k}] = ${str(v)};`); });
  // weights only when they exist (three's skinWeight is a CUSTOM slot now: the runtime gives Godot zero weights)
  if (c.bones) L.push(`\td.bones = ${str(c.bones)};${c.weights ? ` d.weights = ${str(c.weights)};` : ''}`);
  for (const p of c.params) { const v = value(p); if (v) L.push(`\td.param(${str(p.godot)}, ${v});`); else L.push(`\t// ${p.godot}: ${p.type} has no Godot value`); }
  for (const t of c.textures) {
    const kind = t.kind === 'unorm' ? 'UNORM' : t.kind === 'uint16' ? 'UINT16' : 'FLOAT';
    const levels = t.levels.map((l) => `{ ${l.width}, ${l.height}, { ${(l.files ?? [l.file]).map(str).join(', ')} } }`).join(', ');
    L.push(`\td.textures.push_back({ StringName(${str(t.uniform)}), ${t.element ?? -1}, three::Texture{ three::Texture::${kind}, ${t.srgb ? 'true' : 'false'}, ${t.array ? 'true' : 'false'}, ${t.layers ?? 1}, { ${levels} } } });`);
  }
  // instances are data: a file (matrix columns, colour, custom: 24 floats each), not literals for clang to optimise
  if (c.instances) {
    const v = new Float32Array(c.instances.length * 24);
    c.instances.forEach((x, k) => { v.set(x.matrix.slice(0, 16), k * 24); v.set([...x.color.slice(0, 3), x.color[3] ?? 1], k * 24 + 16); v.set(x.custom.slice(0, 4), k * 24 + 20); });
    const name = `${c.index}_instances.bin`;
    fs.writeFileSync(path.join(gDir, 'data', name), Buffer.from(v.buffer));
    L.push(`\td.instances_file = ${str(name)};`);
  }
  L.push('}');
  return L.join('\n');
};
const header = '// generated by scripts/cpp-scene.mjs from a captured three.js frame: do not edit\n#include "three_runtime.h"\n#include <cmath>\nusing namespace godot;\n';
const chunks = [];
for (let k = 0; k < cases.length; k += 8) chunks.push(cases.slice(k, k + 8));
chunks.forEach((ch, k) => fs.writeFileSync(path.join(genDir, `scene_${k}.cpp`), header + ch.map(fnOf).join('\n\n') + '\n'));
fs.writeFileSync(path.join(genDir, 'scene_index.cpp'), header + cases.map((c) => `void three_draw_${fid(c)}(three::Draw &);`).join('\n') +
  `\nextern const three::DrawFn three_gen_draws[] = { ${cases.map((c) => `three_draw_${fid(c)}`).join(', ')} };\nextern const int three_gen_count = ${cases.length};\n`);
say(`${cases.length} draws written as C++ (${chunks.length + 1} generated files)`);

// ---- compile: every file in parallel, objects cached by the source's and the flags' hash
const TC = 'C:/Projects/toolchains/llvm-mingw-20261020-ucrt-x86_64/bin';
const NATIVE = 'C:/Users/Jonathan/Desktop/city/native';
const CXX = `${TC}/x86_64-w64-mingw32-clang++.exe`;
const flags = ['-std=c++17', '-O2', '-DNDEBUG', '-DGDEXTENSION', '-DTHREADS_ENABLED', '-DWINDOWS_ENABLED', '-fvisibility=hidden', '-Wall', '-Wextra',
  `-I${path.join(here, 'cpp', 'runtime')}`, '-isystem', `${NATIVE}/build-win/godot-cpp/gen/include`, '-isystem', `${NATIVE}/godot-cpp/include`, '-isystem', `${NATIVE}/godot-cpp/gdextension`];
const objDir = path.join(here, 'out', 'cache', 'cpp-obj');
fs.mkdirSync(objDir, { recursive: true });
const sources = [path.join(here, 'cpp', 'runtime', 'three_runtime.cpp'), path.join(here, 'cpp', 'runtime', 'register_types.cpp'), ...fs.readdirSync(genDir).map((x) => path.join(genDir, x))];
const headerHash = crypto.createHash('sha256').update(fs.readFileSync(path.join(here, 'cpp', 'runtime', 'three_runtime.h'))).update(flags.join(' ')).digest('hex');
const jobs = Number(flag('--jobs', String(Math.max(1, os.cpus().length - 2))));
let cached = 0;
const objects = [];
const queue = sources.map((src) => {
  const key = crypto.createHash('sha256').update(headerHash).update(fs.readFileSync(src)).digest('hex').slice(0, 24);
  const obj = path.join(objDir, `${path.basename(src, '.cpp')}-${key}.o`);
  objects.push(obj);
  return { src, obj };
}).filter((q) => { if (fs.existsSync(q.obj)) { cached++; return false; } return true; });
let failed = 0;
await new Promise((resolve) => {
  let running = 0; let i = 0;
  const pump = () => {
    if (i >= queue.length && running === 0) { resolve(); return; }
    while (running < jobs && i < queue.length) {
      const q = queue[i++]; running++;
      const t = Date.now();
      console.log(`ITEM BEGIN ${path.basename(q.src)}`);
      // generated scenes only fill structures: -O0 (clang's optimiser took minutes on functions of thousands of
      // literals); the runtime at -O2
      execFile(CXX, [...flags, ...(q.src.startsWith(genDir) ? ['-O0'] : []), '-c', q.src, '-o', `${q.obj}.tmp`], { maxBuffer: 1 << 26 }, (err, _o, stderr) => {
        running--;
        if (err) { failed++; console.log(`ITEM FAIL ${path.basename(q.src)} ${String(stderr).split('\n').filter((l) => /error/.test(l)).slice(0, 3).join(' | ')}`); }
        else { fs.renameSync(`${q.obj}.tmp`, q.obj); if (stderr.trim()) console.log(`warnings in ${path.basename(q.src)}:\n${stderr.trim().split('\n').slice(0, 8).join('\n')}`); }
        console.log(`ITEM END ${path.basename(q.src)} ${Date.now() - t} ms`);
        pump();
      });
    }
  };
  pump();
});
if (failed) { say(`${failed} files did not compile`); process.exit(1); }
say(`compiled ${queue.length} files (${cached} from the cache) on ${jobs} jobs`);
const projDir = path.join(replayDir, 'cpp', 'project');
fs.mkdirSync(path.join(projDir, 'bin'), { recursive: true });
const dll = path.join(projDir, 'bin', 'three_replay.dll');
execFileSync(CXX, ['-shared', '-o', dll, ...objects, `${NATIVE}/build-win/bin/libgodot-cpp.windows.template_release.x86_64.a`, '-Wl,--no-undefined', '-static', '-static-libgcc', '-static-libstdc++', '-s'], { stdio: 'inherit' });
say(`linked ${path.relative(here, dll)} (${(fs.statSync(dll).size / 1e6).toFixed(1)} MB)`);

// ---- the Godot project: the same shaders and data as the GDScript replay (linked, not copied), the extension, a scene
for (const sub of ['shaders', 'data']) {
  const to = path.join(projDir, sub);
  fs.rmSync(to, { recursive: true, force: true });
  fs.mkdirSync(to, { recursive: true });
  for (const x of fs.readdirSync(path.join(gDir, sub))) { try { fs.linkSync(path.join(gDir, sub, x), path.join(to, x)); } catch { fs.copyFileSync(path.join(gDir, sub, x), path.join(to, x)); } }
}
fs.copyFileSync(path.join(gDir, 'project.godot'), path.join(projDir, 'project.godot'));
fs.writeFileSync(path.join(projDir, 'three_replay.gdextension'), ['[configuration]', 'entry_symbol = "three_replay_init"', 'compatibility_minimum = "4.7"', '', '[libraries]',
  'windows.debug.x86_64 = "res://bin/three_replay.dll"', 'windows.release.x86_64 = "res://bin/three_replay.dll"', 'windows.editor.x86_64 = "res://bin/three_replay.dll"', ''].join('\n'));
// Godot loads the extensions listed here (the editor writes it on import; a project run directly needs it)
fs.mkdirSync(path.join(projDir, '.godot'), { recursive: true });
fs.writeFileSync(path.join(projDir, '.godot', 'extension_list.cfg'), 'res://three_replay.gdextension\n');
fs.writeFileSync(path.join(projDir, 'main.tscn'), '[gd_scene format=3]\n\n[node name="Replay" type="ThreeReplay"]\n');
fs.rmSync(path.join(projDir, 'out_cpp'), { recursive: true, force: true });
say('project ready');
