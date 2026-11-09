// Where a draw's Godot image departs from WebGL: each probe expression (an intermediate value of three's shading)
// replayed for every given draw at once (scripts/frame-replay.mjs --probe), several probes in parallel; prints a
// table draw × probe (= equal, ~ near, X different, . empty, ! the program has no such value).
// `node scripts/probe-battery.mjs <capture> <draws a,b,c> <jobs> <expr>...`
import fs from 'node:fs';
import path from 'node:path';
import { machine } from '../src/common/machine.ts';
import { filesKey, mustRun, runTask } from '../src/common/tasks.ts';

/** One probe: the WebGL and Godot replays of the given draws (frame-replay.mjs, its own processes tasks too); past this it is stopped. */
const PROBE_TIMEOUT_S = 3600;

const [capDir, draws, jobsArg, ...exprs] = process.argv.slice(2);
const here = path.resolve(import.meta.dirname, '..');
const PW = `${machine().get('game')}/tools/oracle-4.0/node_modules/playwright`;
const GLSL = machine().get('glsl');
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
      // a probe is a task keyed by the capture's index and the replay's code: the same probe of the same capture is not replayed again
      const replay = path.join(here, 'scripts', 'frame-replay.mjs');
      runTask({ name: 'converter-probe', argv: [process.execPath, replay, capDir, out, PW, GLSL, '--only', draws, '--probe', exprs[k]], cwd: here, timeoutS: PROBE_TIMEOUT_S,
        extra: [filesKey([path.join(capDir, 'index.json'), replay, path.join(here, 'scripts', 'replay.gd'), path.join(here, 'src', 'render', 'godot-shader.ts')])],
        outputs: [path.join(out, 'replay.json')] }).then((r) => mustRun(`probe ${exprs[k]}`, r), (e) => console.error(String(e))).catch((e) => console.error(String(e))).finally(() => {
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
