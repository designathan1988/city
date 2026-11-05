// Godot's judgment of each translated shader (the check project's log, scripts/shaders.ts): a shader passes when
// Godot compiled it and drew it with no error between its markers.
// `node scripts/shaders-verdict.mjs <check dir> [--json <file>]`
import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';

const dir = process.argv[2];
const log = fs.readFileSync(path.join(dir, 'godot.log'), 'utf8').split(/\r?\n/);
const translation = JSON.parse(fs.readFileSync(path.join(dir, 'translation.json'), 'utf8'));
const results = new Map();
// shaders whose code Godot already compiled and drew (scripts/shaders.ts skipped them): passed from the cache
const cachedFile = path.join(dir, 'godot-cached.json');
const cachedPass = new Set(fs.existsSync(cachedFile) ? JSON.parse(fs.readFileSync(cachedFile, 'utf8')) : []);
for (const f of cachedPass) results.set(f, { file: f, errors: [], done: true, cached: true });
let current = null;
for (const line of log) {
  const begin = /^ORACLE BEGIN (\S+)/.exec(line);
  if (begin) { current = { file: begin[1], errors: [], done: false }; results.set(current.file, current); continue; }
  if (/^ORACLE END /.test(line)) { if (current) current.done = true; current = null; continue; }
  if (!current) continue;
  const m = /(SHADER ERROR|ERROR): (.*)$/.exec(line);
  if (m) current.errors.push(m[2].trim());
}
const rows = translation.map((t) => {
  if (t.error) return { id: t.id, material: '?', pass: false, errors: [`translation: ${t.error.split('\n')[0]}`] };
  const r = results.get(t.file);
  const errors = r ? r.errors : ['not reached by the check'];
  return { id: t.id, file: t.file, material: t.material, pass: !!r?.done && errors.length === 0, errors, unsupported: t.unsupported };
});
const byMaterial = {};
for (const r of rows) {
  const k = r.material;
  byMaterial[k] ??= { pass: 0, total: 0 };
  byMaterial[k].total++;
  if (r.pass) byMaterial[k].pass++;
}
const failing = rows.filter((r) => !r.pass);
// what passed now is known from here on (by its code's hash)
const passFile = path.resolve(import.meta.dirname, '..', 'out', 'cache', 'godot-pass.json');
const known = new Set(fs.existsSync(passFile) ? JSON.parse(fs.readFileSync(passFile, 'utf8')) : []);
for (const r of rows) if (r.pass && r.file) known.add(crypto.createHash('sha256').update(fs.readFileSync(path.join(dir, 'shaders', r.file))).digest('hex'));
fs.mkdirSync(path.dirname(passFile), { recursive: true });
fs.writeFileSync(passFile, JSON.stringify([...known]));
const fromCache = rows.filter((r) => r.pass && cachedPass.has(r.file)).length;
const kinds = {};
for (const r of failing) for (const e of new Set(r.errors)) kinds[e] = (kinds[e] ?? 0) + 1;
const out = { pass: rows.length - failing.length, total: rows.length, byMaterial, errorKinds: kinds, failing: failing.map((r) => ({ id: r.id, file: r.file, errors: [...new Set(r.errors)] })) };
const j = process.argv.indexOf('--json');
if (j > 0) fs.writeFileSync(process.argv[j + 1], JSON.stringify(out, null, 1));
console.log(`Godot compiled and drew ${out.pass}/${out.total} translated programs (${fromCache} known from the cache, ${out.pass - fromCache} compiled now)`);
for (const [k, v] of Object.entries(byMaterial)) console.log(`  ${k.padEnd(22)} ${v.pass}/${v.total}`);
for (const [k, n] of Object.entries(kinds).sort((a, b) => b[1] - a[1]).slice(0, 12)) console.log(`  ${String(n).padStart(4)} × ${k}`);
