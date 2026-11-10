// A look at the 4.0 with nothing left open. The reference is one fixed commit, so what a plan shows never changes: each
// plan's result is kept by content (the commit and the plan), and asking again copies it with no browser and no server.
// Only a plan never seen runs the game: its production build (made once, `--mode probe`, outside the 4.0's folder) from a
// static server inside this process, the 4.0's own scripts/look.mjs in headless Chrome, and both are gone when it ends.
// `node scripts/reference40.mjs <plan.json> <out folder> [--again]`
import { spawn, execFileSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import fs from 'node:fs';
import { createServer } from 'node:http';
import { createRequire } from 'node:module';
import path from 'node:path';
import { machine } from '../src/common/machine.ts';

const HERE = path.dirname(new URL(import.meta.url).pathname.replace(/^\/([A-Za-z]:)/, '$1'));
const CONVERTER = path.join(HERE, '..');
const source = machine().get('source');
const args = process.argv.slice(2);
const [planFile, outDir] = args.filter((a) => !a.startsWith('--'));
if (!planFile || !outDir) {
  console.error('usage: node scripts/reference40.mjs <plan.json> <out folder> [--again]');
  process.exit(2);
}
const commit = execFileSync('git', ['-C', source, 'rev-parse', 'HEAD'], { encoding: 'utf8' }).trim();
const build = path.join(CONVERTER, 'out', 'build40', commit.slice(0, 8));
const store = path.join(CONVERTER, 'out', 'reference-store');

/** A plan as content: its keys sorted, what only says where to run or write left out. */
function canonical(value) {
  if (Array.isArray(value)) return value.map(canonical);
  if (value === null || typeof value !== 'object') return value;
  return Object.fromEntries(Object.keys(value).filter((k) => k !== 'url' && k !== 'out').sort().map((k) => [k, canonical(value[k])]));
}

/**
 * A plan of the game's look.gd ({"city": {"seed": 7, "size": "small"}, "steps": [...]}) as the 4.0's look.mjs takes it, so one
 * plan serves both games: the city opened as the menu opens it, `hour` as the clock and the sky, `frames` as a wait.
 */
function forThe40(asked) {
  if (!asked.city) return asked;
  const { seed = 1, size = 'small' } = asked.city;
  const opening = [
    { eval: `performance.clearMarks(); window.__roadcraft.setTraffic(false); window.__roadcraft.generateCity({ seed: ${Number(seed)}, size: ${JSON.stringify(size)} }); 'started'` },
    { until: "performance.getEntriesByName('city:applied').length > 0 && performance.getEntriesByName('opening:shown').length > 0", name: 'city shown', timeout: 120000, polling: 200 },
    { until: "(() => { const c = document.getElementById('loading-curtain'); return !!c && getComputedStyle(c).display === 'none'; })()", name: 'curtain gone', timeout: 60000, polling: 200 },
    { press: 'Começar a construir', optional: true },
    { press: 'Continuar', optional: true },
  ];
  const steps = (asked.steps ?? []).map((s) => {
    if (s.hour !== undefined) return { eval: `window.__roadcraft.sim.setClock(${Number(s.hour) * 60}); 'clock'` };
    if (s.frames !== undefined) return { wait: Math.ceil(Number(s.frames) * 1000 / 60) };
    return s;
  });
  const { city: _city, ...rest } = asked;
  return { storage: { 'roadcraft.quality': 'ultra' }, viewport: [1920, 1080], ...rest, steps: [...opening, ...steps] };
}

/**
 * {"edit": "<code with d, the RoadDoc>"}: the document changed and reopened whole. An edit made straight on the document skips the road
 * the game's edits go through (`mutateBuilt`), and the scene would not know; loadDoc is the game's own way in.
 */
function withEdits(asked) {
  return { ...asked, steps: (asked.steps ?? []).flatMap((s) => (s.edit === undefined ? [s] : [
    { eval: `(() => { performance.clearMarks(); const r = window.__roadcraft, d = r.doc; ${s.edit}; r.loadDoc(d.toJSON()); return 'edited'; })()` },
    { until: "performance.getEntriesByName('opening:shown').length > 0", name: 'city reopened', timeout: 120000, polling: 200 },
    { until: "(() => { const c = document.getElementById('loading-curtain'); return !!c && getComputedStyle(c).display === 'none'; })()", name: 'curtain gone', timeout: 60000, polling: 200 },
  ])) };
}

const plan = withEdits(forThe40(JSON.parse(fs.readFileSync(planFile, 'utf8'))));
const key =createHash('sha256').update(commit).update(JSON.stringify(canonical(plan))).digest('hex').slice(0, 24);
const kept = path.join(store, key);
fs.mkdirSync(outDir, { recursive: true });

if (fs.existsSync(path.join(kept, 'plan.json')) && !args.includes('--again')) {
  fs.cpSync(kept, outDir, { recursive: true });
  console.log(`reference40: ${key} from the store: nothing opened -> ${outDir}`);
  process.exit(0);
}
if (!fs.existsSync(path.join(build, 'index.html'))) {
  console.error(`reference40: no build of ${commit.slice(0, 8)}: in ${source}, run vite build --mode probe --outDir ${build} --emptyOutDir`);
  process.exit(1);
}

const TYPES = { '.html': 'text/html', '.js': 'text/javascript', '.mjs': 'text/javascript', '.css': 'text/css', '.json': 'application/json', '.wasm': 'application/wasm',
  '.png': 'image/png', '.jpg': 'image/jpeg', '.svg': 'image/svg+xml', '.ktx2': 'image/ktx2', '.glb': 'model/gltf-binary', '.bin': 'application/octet-stream' };
// the headers the game's own server sends: it needs to be cross-origin isolated
const isolated = createRequire(path.join(source, 'package.json'))('./desktop/isolation.cjs');
const server = createServer((req, res) => {
  const asked = decodeURIComponent(new URL(req.url, 'http://x').pathname);
  const file = path.join(build, asked.endsWith('/') ? `${asked}index.html` : asked);
  if (!file.startsWith(build) || !fs.existsSync(file) || fs.statSync(file).isDirectory()) {
    res.writeHead(404, isolated).end();
    return;
  }
  res.writeHead(200, { ...isolated, 'Content-Type': TYPES[path.extname(file)] ?? 'application/octet-stream' });
  fs.createReadStream(file).pipe(res);
});
await new Promise((done) => { server.listen(0, '127.0.0.1', done); });
const url = `http://127.0.0.1:${server.address().port}/`;
const run = path.join(store, `${key}.run`);
fs.rmSync(run, { recursive: true, force: true });
fs.mkdirSync(run, { recursive: true });
const code = await new Promise((done) => {
  const planned = path.join(run, 'asked.json');
  fs.writeFileSync(planned, JSON.stringify(plan));
  const look = spawn(process.execPath, ['scripts/look.mjs', planned, run, `--url=${url}`], { cwd: source, stdio: 'inherit', windowsHide: true });
  look.on('exit', (status) => done(status ?? 1));
});
server.close();
server.closeAllConnections();
if (code !== 0) {
  console.error(`reference40: the look failed (${code}); nothing kept`);
  process.exit(code);
}
fs.writeFileSync(path.join(run, 'plan.json'), JSON.stringify(plan, null, 1));
fs.rmSync(kept, { recursive: true, force: true });
fs.renameSync(run, kept);
fs.cpSync(kept, outDir, { recursive: true });
console.log(`reference40: ${key} made and kept -> ${outDir}`);
