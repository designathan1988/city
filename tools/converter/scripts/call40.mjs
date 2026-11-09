// What the 4.0 answers, with no browser, no server and no game open: its own TypeScript run by Node, as it is, in one short
// process. The modules that touch neither the page nor the GPU (the world, the simulation, the geometry) answer directly.
// `node scripts/call40.mjs <src/module.ts> <expression>`: the expression has
// `m`, the module; its value is printed as JSON. `@alias/...` in the expression's own imports is not needed: pass more modules
// with --also=<src/other.ts>, read as `also[0]`...
import fs from 'node:fs';
import path from 'node:path';
import { pathToFileURL } from 'node:url';
import { machine } from '../src/common/machine.ts';

const HERE = path.dirname(new URL(import.meta.url).pathname.replace(/^\/([A-Za-z]:)/, '$1'));
const root = machine().get('source');
const args = process.argv.slice(2);
const also = args.filter((a) => a.startsWith('--also=')).map((a) => a.slice('--also='.length));
const scene = args.includes('--scene');
const [moduleFile, expression] = scene ? ['--scene', args.filter((a) => !a.startsWith('--'))[0]] : args.filter((a) => !a.startsWith('--'));
if (!moduleFile || !expression) {
  console.error('usage: node scripts/call40.mjs <src/module.ts> <expression with m> [--also=<src/other.ts>]');
  process.exit(2);
}

await import('./hooks40.mjs');

const load = (file) => import(pathToFileURL(path.join(root, file)).href);
/**
 * --scene: the real scene with no browser and no GPU: the 4.0's createSceneRenderer on three's WebGLRenderer over the null WebGL2
 * context of its own tests (tests/support/nullGpu.ts, sceneHarness.ts), the small city opened as the menu opens it. `m` is
 * { city, view([x, y, zoom, azimuth, elevation]), settle() }: what the game would draw and what the world holds, as data.
 */
async function openScene() {
  const gpu = await load('tests/support/nullGpu.ts');
  const harness = await load('tests/support/sceneHarness.ts');
  process.chdir(root);
  gpu.installNullPage(harness.WIDTH, harness.HEIGHT, root);
  gpu.primeCookedSurfaces(root);
  const city = await harness.openSmallCity();
  return { city, harness, view: (v) => harness.setView(city.scene, v), settle: () => harness.settle(city) };
}
const m = moduleFile === '--scene' ? await openScene() : await load(moduleFile);
const others = await Promise.all(also.map(load));
const value = await new Function('m', 'also', `return (${expression});`)(m, others);
const replacer = (_, v) => (v instanceof Map ? [...v] : v instanceof Set ? [...v] : ArrayBuffer.isView(v) ? Array.from(v) : v);
console.log(JSON.stringify(value, replacer));
// the game's timers would keep the process alive: what was asked is answered
process.exit(0);
