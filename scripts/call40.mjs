// What the 4.0 answers, with no browser, no server and no game open: its own TypeScript run by Node, as it is, in one short
// process. The modules that touch neither the page nor the GPU (the world, the simulation, the geometry) answer directly.
// `node --experimental-transform-types --no-warnings scripts/call40.mjs <src/module.ts> <expression>`: the expression has
// `m`, the module; its value is printed as JSON. `@alias/...` in the expression's own imports is not needed: pass more modules
// with --also=<src/other.ts>, read as `also[0]`...
import fs from 'node:fs';
import { registerHooks } from 'node:module';
import path from 'node:path';
import { pathToFileURL } from 'node:url';

const HERE = path.dirname(new URL(import.meta.url).pathname.replace(/^\/([A-Za-z]:)/, '$1'));
const machine = JSON.parse(fs.readFileSync(path.join(HERE, '..', 'configs', 'roadcraft-machine.json'), 'utf8'));
const root = machine.source;
const args = process.argv.slice(2);
const also = args.filter((a) => a.startsWith('--also=')).map((a) => a.slice('--also='.length));
const [moduleFile, expression] = args.filter((a) => !a.startsWith('--'));
if (!moduleFile || !expression) {
  console.error('usage: node --experimental-transform-types --no-warnings scripts/call40.mjs <src/module.ts> <expression with m> [--also=<src/other.ts>]');
  process.exit(2);
}

// what the game's build gives its code (vite.config.ts `define`, import.meta.env of a production build)
globalThis.__MAP_TILES__ = Number(process.env.ROADCRAFT_MAP_TILES ?? 20);
globalThis.__WEAK_MACHINE__ = '';
globalThis.__env40 = { DEV: false, PROD: true, MODE: 'production', BASE_URL: '/' };

// the source's `@core/x` paths and its imports without an extension
const config = JSON.parse(fs.readFileSync(path.join(root, 'tsconfig.json'), 'utf8').replace(/^\s*\/\/.*$/gm, '').replace(/,(\s*[}\]])/g, '$1'));
const aliases = Object.entries(config.compilerOptions?.paths ?? {}).map(([from, [to]]) => [from.replace(/\*$/, ''), path.join(root, to.replace(/\*$/, ''))]);
const asFile = (base) => [base, `${base}.ts`, `${base}.tsx`, path.join(base, 'index.ts')].find((f) => fs.existsSync(f) && fs.statSync(f).isFile());

registerHooks({
  resolve(specifier, context, next) {
    const alias = aliases.find(([from]) => specifier.startsWith(from));
    const parent = context.parentURL?.startsWith('file:') ? path.dirname(new URL(context.parentURL).pathname.replace(/^\/([A-Za-z]:)/, '$1')) : null;
    const clean = specifier.replace(/\?.*$/, '');
    const base = alias ? alias[1] + clean.slice(alias[0].length) : clean.startsWith('.') && parent ? path.resolve(parent, clean) : null;
    const file = base ? asFile(base) : null;
    if (file) return { url: pathToFileURL(file).href, shortCircuit: true };
    return next(specifier, context);
  },
  load(url, context, next) {
    if (!url.startsWith('file:') || !url.includes('/src/') || !/\.tsx?$/.test(new URL(url).pathname)) {
      // what is no code here (a style, a picture a module imports): an empty module
      if (url.startsWith('file:') && !/\.(m?js|cjs|json|node|tsx?)$/.test(new URL(url).pathname)) return { format: 'module', source: 'export default "";', shortCircuit: true };
      return next(url, context);
    }
    const text = fs.readFileSync(new URL(url), 'utf8').replaceAll('import.meta.env', 'globalThis.__env40');
    return { format: 'module-typescript', source: text, shortCircuit: true };
  },
});

const load = (file) => import(pathToFileURL(path.join(root, file)).href);
const m = await load(moduleFile);
const others = await Promise.all(also.map(load));
const value = await new Function('m', 'also', `return (${expression});`)(m, others);
const replacer = (_, v) => (v instanceof Map ? [...v] : v instanceof Set ? [...v] : ArrayBuffer.isView(v) ? Array.from(v) : v);
console.log(JSON.stringify(value, replacer));
