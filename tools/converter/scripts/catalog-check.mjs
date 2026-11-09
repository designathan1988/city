// The three.js → Godot catalog (configs/three-godot-catalog.json) checked two ways:
//  1. complete: every three.js class a feature's trace uses (boundary.json external keys `three:Class.member`) has an
//     entry; a key missing is a failure.
//  2. real: every Godot class and method an entry relies on exists in the Godot binary (ClassDB, run headless).
// `node scripts/catalog-check.mjs <catalog> <features dir> <godot executable>`
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { filesKey, logOf, mustRun, runTasksSync, toolKey } from '../src/common/tasks.ts';

/** Godot headless answering what its ClassDB holds, and its version; past this its process is stopped. */
const GODOT_QUERY_TIMEOUT_S = 120;

const [catalogFile, featuresDir, godot] = process.argv.slice(2);
const catalog = JSON.parse(fs.readFileSync(catalogFile, 'utf8'));
const used = new Map();
for (const d of fs.readdirSync(featuresDir)) {
  const f = path.join(featuresDir, d, 'boundary.json');
  if (!fs.existsSync(f)) continue;
  for (const x of JSON.parse(fs.readFileSync(f, 'utf8')).external) {
    if (!x.key.startsWith('three:')) continue;
    const cls = x.key.slice(6).split('.')[0];
    (used.get(cls) ?? used.set(cls, new Set()).get(cls)).add(d);
  }
}
const missing = [...used.keys()].filter((c) => !catalog.classes[c]);
console.log(`complete: ${used.size - missing.length}/${used.size} three.js classes the features use have an entry${missing.length ? ` — missing: ${missing.join(', ')}` : ''}`);

const pairs = [...new Set(Object.values(catalog.classes).flatMap((e) => e.verify ?? []).map((p) => p.join('.')))].map((p) => p.split('.'));
const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'catalog-check-'));
fs.writeFileSync(path.join(dir, 'project.godot'), 'config_version=5\n');
fs.writeFileSync(path.join(dir, 'check.gd'), `extends SceneTree
func _initialize() -> void:
	var pairs: Array = ${JSON.stringify(pairs)}
	for p in pairs:
		var ok := ClassDB.class_exists(p[0])
		if ok and p.size() > 1:
			ok = ClassDB.class_has_method(p[0], p[1]) or ClassDB.class_has_integer_constant(p[0], p[1])
		if not ok and p.size() > 1 and Variant.has_method(p[0]) if false else false:
			pass
		print("CATALOG ", "OK " if ok else "MISSING ", p[0], ".", p[1] if p.size() > 1 else "")
	quit()
`);
// built-in value types (Vector3, AABB, Basis...) are not in ClassDB: checked as Variant methods by a probe
const valueTypes = new Set(['Vector3', 'Vector2', 'AABB', 'Basis', 'Quaternion', 'Color', 'Plane', 'Projection', 'Transform3D']);
const classPairs = pairs.filter((p) => !valueTypes.has(p[0]));
const valuePairs = pairs.filter((p) => valueTypes.has(p[0]));
fs.writeFileSync(path.join(dir, 'check.gd'), `extends SceneTree
func _initialize() -> void:
	for p in ${JSON.stringify(classPairs)}:
		var ok := ClassDB.class_exists(p[0]) and ClassDB.class_has_method(p[0], p[1], false)
		print("CATALOG ", "OK " if ok else "MISSING ", p[0], ".", p[1])
	var samples := { "Vector3": Vector3(), "Vector2": Vector2(), "AABB": AABB(), "Basis": Basis(), "Quaternion": Quaternion(), "Color": Color(), "Plane": Plane(), "Projection": Projection(), "Transform3D": Transform3D() }
	for p in ${JSON.stringify(valuePairs)}:
		var v: Variant = samples[p[0]]
		print("CATALOG ", "OK " if Callable.create(v, p[1]).is_valid() else "MISSING ", p[0], ".", p[1])
	quit()
`);
// two tasks, keyed by the probe script and the Godot binary: the same question of the same binary is answered once
const [checked, version] = runTasksSync([
  { name: 'converter-catalog-check', cwd: dir, timeoutS: GODOT_QUERY_TIMEOUT_S, extra: [filesKey([path.join(dir, 'check.gd')]), toolKey(godot)],
    argv: [godot, '--headless', '--path', dir, '--script', 'res://check.gd', '--quit-after', '100'] },
  { name: 'converter-godot-version', timeoutS: GODOT_QUERY_TIMEOUT_S, extra: [toolKey(godot)], argv: [godot, '--version'] },
]);
const out = logOf(mustRun('the catalog check', checked));
const lines = out.split(/\r?\n/).filter((l) => l.startsWith('CATALOG '));
const bad = lines.filter((l) => l.includes('MISSING'));
console.log(`real: ${lines.length - bad.length}/${lines.length} Godot classes and methods the entries rely on exist in ${logOf(mustRun('godot --version', version)).trim().split(/\r?\n/).at(-1)}`);
for (const l of bad) console.log(`  ${l}`);
process.exitCode = missing.length + bad.length > 0 ? 1 : 0;
