// Translates every three.js program the 4.0 linked (scripts/oracle.mjs → programs.json) into a Godot shader and
// writes a Godot project that compiles each one and draws it once, so Godot itself judges every translation.
// Fast by construction:
//  - a translation is cached by content (the program, its options and the translator's own source): a run redoes
//    only what changed (out/cache/shader-translation);
//  - what is left is translated by a pool of worker threads (one per core, the parser loaded once in each);
//  - a shader whose code Godot already compiled and drew is not compiled again (out/cache/godot-pass.json): the
//    check project skips it (res://live-skip.txt) and the verdict counts it as passed from the cache.
// Every item says ITEM BEGIN/END (scripts/live-run.mjs watches them).
// `node scripts/shaders.ts <programs.json> <out dir> --glsl <dir> [--skip <file>] [--jobs <n>] [--no-cache]`
import crypto from 'node:crypto';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { Worker, isMainThread, parentPort, workerData } from 'node:worker_threads';
import { loadGlsl, translateProgram, type ThreeProgram } from '../src/render/godot-shader.ts';

type Program = ThreeProgram & { instanced?: string[]; options?: Record<string, unknown>; name?: string };
interface Job { index: number; program: Program }
interface Done { index: number; entry: Record<string, unknown>; code: string | null; ms: number }

if (!isMainThread) {
  // a worker: the parser once, then each program it is given
  const glsl = loadGlsl(workerData.glslDir as string);
  parentPort!.on('message', (job: Job) => {
    const t = Date.now();
    const id = String(job.index).padStart(3, '0');
    try {
      const s = translateProgram(glsl, job.program, { ...(job.program.options ?? {}), instancedAttributes: job.program.instanced ?? [] });
      const file = `${job.program.name ?? id}_${s.material}.gdshader`;
      parentPort!.postMessage({ index: job.index, code: s.code, ms: Date.now() - t, entry: { id, file, material: s.material, name: s.name, uniforms: s.uniforms, attributes: s.attributes, notes: s.notes, unsupported: s.unsupported } } satisfies Done);
    } catch (e) {
      parentPort!.postMessage({ index: job.index, code: null, ms: Date.now() - t, entry: { id, error: String((e as Error).message).split('\n').slice(0, 6).join('\n') } } satisfies Done);
    }
  });
} else {
  const args = process.argv.slice(2);
  const flag = (n: string): string | undefined => { const i = args.indexOf(n); return i >= 0 ? args[i + 1] : undefined; };
  const [programsFile, outDir] = args;
  const glslDir = flag('--glsl') ?? process.env['CONVERTER_GLSL'];
  if (!programsFile || !outDir || !glslDir) { console.error('usage: shaders.ts <programs.json> <out dir> --glsl <dir>'); process.exit(2); }
  const programs = JSON.parse(fs.readFileSync(programsFile, 'utf8')) as Program[];
  const here = path.resolve(import.meta.dirname, '..');
  const cacheDir = path.join(here, 'out', 'cache', 'shader-translation');
  const passFile = path.join(here, 'out', 'cache', 'godot-pass.json');
  fs.mkdirSync(cacheDir, { recursive: true });
  // the translator's own source is part of every key: a change to it redoes everything it may change
  const translatorHash = crypto.createHash('sha256').update(fs.readdirSync(path.join(here, 'src', 'render')).sort().map((f) => fs.readFileSync(path.join(here, 'src', 'render', f), 'utf8')).join('\0'))
    // the parser's generated code too: a patched or updated parser redoes everything
    .update(fs.readFileSync(path.join(glslDir, 'node_modules', '@shaderfrog', 'glsl-parser', 'parser', 'parser.js')))
    .update(fs.readFileSync(path.join(glslDir, 'node_modules', '@shaderfrog', 'glsl-parser', 'preprocessor', 'preprocessor.js')))
    .digest('hex');
  const keyOf = (p: Program): string => crypto.createHash('sha256').update(translatorHash).update(p.vertex).update('\0').update(p.fragment).update('\0').update(JSON.stringify(p.instanced ?? [])).update(JSON.stringify(p.options ?? {})).digest('hex');
  const useCache = !args.includes('--no-cache');
  const shaderDir = path.join(outDir, 'shaders');
  const reportDir = path.join(outDir, 'report');
  const skipFile = flag('--skip');
  const skip = new Set(skipFile && fs.existsSync(skipFile) ? fs.readFileSync(skipFile, 'utf8').split(/\r?\n/).filter(Boolean) : []);
  if (skip.size === 0) fs.rmSync(outDir, { recursive: true, force: true });
  fs.mkdirSync(shaderDir, { recursive: true });
  fs.mkdirSync(reportDir, { recursive: true });
  const t0 = Date.now();
  console.log(`LIVE PID ${process.pid}`);
  const write = (d: Done): void => {
    if (d.code !== null) fs.writeFileSync(path.join(shaderDir, d.entry['file'] as string), d.code);
    fs.writeFileSync(path.join(reportDir, `${d.entry['id']}.json`), JSON.stringify(d.entry));
  };
  // cached ones at once, the rest to the pool
  const todo: Job[] = [];
  let cached = 0;
  programs.forEach((program, index) => {
    const id = String(index).padStart(3, '0');
    if (skip.has(id)) return;
    const file = path.join(cacheDir, `${keyOf(program)}.json`);
    if (useCache && fs.existsSync(file)) {
      console.log(`ITEM BEGIN ${id}`);
      // the cached translation is the program's; its id is this list's (the cache is shared by every list)
      const done = JSON.parse(fs.readFileSync(file, 'utf8')) as Done;
      const entry = done.entry['error'] ? { ...done.entry, id } : { ...done.entry, id, file: `${program.name ?? id}_${String(done.entry['material'])}.gdshader` };
      write({ ...done, index, entry });
      console.log(`ITEM END ${id}`);
      cached++;
    } else todo.push({ index, program });
  });
  const jobs = Math.max(1, Math.min(Number(flag('--jobs') ?? os.cpus().length - 2), todo.length));
  let failed = 0;
  await new Promise<void>((resolve) => {
    if (todo.length === 0) { resolve(); return; }
    let next = 0;
    let running = 0;
    const start = (w: Worker): void => {
      const job = todo[next++];
      if (!job) { void w.terminate(); if (--running === 0) resolve(); return; }
      console.log(`ITEM BEGIN ${String(job.index).padStart(3, '0')}`);
      w.postMessage(job);
    };
    for (let k = 0; k < jobs; k++) {
      const w = new Worker(new URL(import.meta.url), { workerData: { glslDir } });
      running++;
      w.on('message', (d: Done) => {
        if (d.code === null) { failed++; console.log(`ITEM FAIL ${String(d.index).padStart(3, '0')} ${String(d.entry['error']).split('\n')[0]}`); }
        write(d);
        fs.writeFileSync(path.join(cacheDir, `${keyOf(programs[d.index]!)}.json`), JSON.stringify(d));
        console.log(`ITEM END ${String(d.index).padStart(3, '0')}`);
        start(w);
      });
      w.on('error', (e) => { console.log(`Error: worker ${e.message}`); if (--running === 0) resolve(); });
      start(w);
    }
  });
  const report = programs.map((_, i) => {
    const id = String(i).padStart(3, '0');
    const file = path.join(reportDir, `${id}.json`);
    return fs.existsSync(file) ? JSON.parse(fs.readFileSync(file, 'utf8')) : { id, error: 'translation hung (scripts/live-run.mjs killed it)' };
  });
  fs.writeFileSync(path.join(outDir, 'translation.json'), JSON.stringify(report, null, 1));
  // Godot's cache: a shader whose code already compiled and drew is skipped by the check (and passed by the verdict)
  const passed = new Set<string>(fs.existsSync(passFile) ? JSON.parse(fs.readFileSync(passFile, 'utf8')) as string[] : []);
  const known: string[] = [];
  for (const f of fs.readdirSync(shaderDir)) {
    const h = crypto.createHash('sha256').update(fs.readFileSync(path.join(shaderDir, f))).digest('hex');
    if (useCache && passed.has(h)) known.push(f);
  }
  fs.writeFileSync(path.join(outDir, 'live-skip.txt'), known.join('\n'));
  fs.writeFileSync(path.join(outDir, 'godot-cached.json'), JSON.stringify(known));
  fs.writeFileSync(path.join(outDir, 'project.godot'), [
    'config_version=5', '', '[application]', 'config/name="three shader check"', '', '[rendering]',
    'renderer/rendering_method="forward_plus"', '', '[display]', 'window/size/viewport_width=128', 'window/size/viewport_height=128', '',
  ].join('\n'));
  fs.writeFileSync(path.join(outDir, 'check.gd'), `extends SceneTree
# Every shader in res://shaders not already known to pass: compiled and drawn; the engine prints a shader's errors
# between its markers.
func _initialize() -> void:
	print("LIVE PID ", OS.get_process_id())
	var skip := {}
	if FileAccess.file_exists("res://live-skip.txt"):
		for l in FileAccess.get_file_as_string("res://live-skip.txt").split("\\n"):
			if l.strip_edges() != "":
				skip[l.strip_edges()] = true
	var cam := Camera3D.new()
	cam.position = Vector3(0, 0, 2)
	root.add_child(cam)
	var quad := MeshInstance3D.new()
	quad.mesh = QuadMesh.new()
	root.add_child(quad)
	var files := DirAccess.get_files_at("res://shaders")
	files.sort()
	for f in files:
		if not f.ends_with(".gdshader") or skip.has(f):
			continue
		print("ORACLE BEGIN ", f)
		var shader := Shader.new()
		shader.code = FileAccess.get_file_as_string("res://shaders/" + f)
		var mat := ShaderMaterial.new()
		mat.shader = shader
		quad.material_override = mat
		RenderingServer.force_sync()
		await process_frame
		await process_frame
		RenderingServer.force_sync()
		print("ORACLE END ", f)
	print("ORACLE DONE")
	quit()
`);
  console.log(`translated ${programs.length - failed}/${programs.length} (${cached} from the cache, ${todo.length} on ${jobs} workers, ${failed} threw) in ${((Date.now() - t0) / 1000).toFixed(1)} s; ${known.length} already known to compile in Godot -> ${outDir}`);
}
