// Where a draw's Godot image departs from WebGL: each probe expression (an intermediate value of three's shading)
// replayed for every given draw at once (scripts/frame-replay.mjs --probe), several probes in parallel; prints a
// table draw × probe (= equal, ~ near, X different, . empty, ! the program has no such value).
// `node scripts/probe-battery.mjs <capture> <draws a,b,c> <jobs> <expr>...`
import fs from 'node:fs';
import path from 'node:path';
import { spawn } from 'node:child_process';

const [capDir, draws, jobsArg, ...exprs] = process.argv.slice(2);
const here = path.resolve(import.meta.dirname, '..');
const PW = 'C:/Users/Jonathan/Desktop/city/tools/oracle-4.0/node_modules/playwright';
const GLSL = 'C:/Projects/toolchains/glsl-parser-fast';
const jobs = Number(jobsArg);
const results = new Map();
let next = 0;
await new Promise((resolve) => {
  let running = 0;
  const pump = () => {
    if (next >= exprs.length && running === 0) { resolve(); return; }
    while (running < jobs && next < exprs.length) {
      const k = next++; running++;
      const out = path.join(here, 'out', `battery${k}`);
      const t = Date.now();
      const p = spawn(process.execPath, [path.join(here, 'scripts', 'frame-replay.mjs'), capDir, out, PW, GLSL, '--only', draws, '--probe', exprs[k]], { stdio: 'ignore' });
      p.on('exit', () => {
        running--;
        const r = fs.existsSync(path.join(out, 'replay.json')) ? JSON.parse(fs.readFileSync(path.join(out, 'replay.json'), 'utf8')) : [];
        results.set(exprs[k], new Map(r.map((x) => [x.index, x.tier])));
        console.log(`probe ${k + 1}/${exprs.length} done in ${((Date.now() - t) / 1000).toFixed(0)} s: ${exprs[k]}`);
        pump();
      });
    }
  };
  pump();
});
const mark = { equal: '=', near: '~', different: 'X', empty: '.', void: '!' };
const ids = draws.split(',').map(Number);
console.log(`\n${'draw'.padEnd(6)}${exprs.map((e, k) => `p${k + 1}`.padEnd(4)).join('')}`);
for (const id of ids) console.log(`${String(id).padEnd(6)}${exprs.map((e) => (mark[results.get(e)?.get(id)] ?? '?').padEnd(4)).join('')}`);
exprs.forEach((e, k) => console.log(`p${k + 1} = ${e}`));
