// Compares a Godot replay's images (scripts/replay.gd → godot/out, or the C++ one → cpp/project/out_cpp) with the
// WebGL reference, by the verdict of scripts/replay-judge.mjs (orientation, alpha, float targets, depth-only draws);
// with --same-as <dir>, also byte for byte with another Godot replay (the C++ port against the GDScript one).
// `node scripts/replay-compare.mjs <replay dir> <images dir> [--same-as <images dir>] [--json <file>]`
import fs from 'node:fs';
import path from 'node:path';
import { judge } from './replay-judge.mjs';

const [dir, imgDir] = process.argv.slice(2);
const flag = (n) => { const i = process.argv.indexOf(n); return i >= 0 ? process.argv[i + 1] : null; };
const same = flag('--same-as');
const cases = JSON.parse(fs.readFileSync(path.join(dir, 'godot', 'cases.json'), 'utf8'));
const extra = fs.existsSync(path.join(dir, 'judge.json')) ? JSON.parse(fs.readFileSync(path.join(dir, 'judge.json'), 'utf8')) : { depthOnly: [], programs: {} };
const report = judge(cases, path.join(dir, 'webgl'), imgDir, { depthOnly: new Set(extra.depthOnly), programOf: (i) => extra.programs[i] ?? null });
let identical = 0, compared = 0;
if (same) for (const c of cases) {
  const name = `${c.id ?? c.index}.${c.bits ? 'f32' : 'rgba'}`;
  const a = path.join(imgDir, name), b = path.join(same, name);
  if (fs.existsSync(a) && fs.existsSync(b)) { compared++; if (Buffer.compare(fs.readFileSync(a), fs.readFileSync(b)) === 0) identical++; }
}
const count = (t) => report.filter((r) => r.tier === t).length;
const j = flag('--json'); if (j) fs.writeFileSync(j, JSON.stringify(report, null, 1));
console.log(`${path.relative(process.cwd(), imgDir)} vs WebGL (${report.length} draws): ${count('equal')} equal, ${count('near')} near, ${count('different')} different, ${count('empty')} empty, ${count('void')} void${same ? `; byte-identical to ${path.relative(process.cwd(), same)}: ${identical}/${compared} images` : ''}`);
