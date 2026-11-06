// How Node reads the 4.0 as its bundler does: its aliases, its virtual modules, its TypeScript by its own compiler, what the build defines.
import { createHash } from 'node:crypto';
import fs from 'node:fs';
import { createRequire, registerHooks } from 'node:module';
import path from 'node:path';
import { pathToFileURL } from 'node:url';

const HERE = path.dirname(new URL(import.meta.url).pathname.replace(/^\/([A-Za-z]:)/, '$1'));
const machine = JSON.parse(fs.readFileSync(path.join(HERE, '..', 'configs', 'roadcraft-machine.json'), 'utf8'));
const root = machine.source;

// what the game's build gives its code (vite.config.ts `define`, import.meta.env of a production build)
globalThis.__MAP_TILES__ = Number(process.env.ROADCRAFT_MAP_TILES ?? 20);
globalThis.__WEAK_MACHINE__ = '';
globalThis.__env40 = { DEV: false, PROD: true, MODE: 'production', BASE_URL: '/' };

// the source's `@core/x` paths and its imports without an extension
const config = JSON.parse(fs.readFileSync(path.join(root, 'tsconfig.json'), 'utf8').replace(/^\s*\/\/.*$/gm, '').replace(/,(\s*[}\]])/g, '$1'));
const aliases = Object.entries(config.compilerOptions?.paths ?? {}).map(([from, [to]]) => [from.replace(/\*$/, ''), path.join(root, to.replace(/\*$/, ''))]);
const asFile = (base) => [base, `${base}.ts`, `${base}.tsx`, path.join(base, 'index.ts')].find((f) => fs.existsSync(f) && fs.statSync(f).isFile());

/** A module as JavaScript, by the 4.0's own TypeScript (Node's type stripper refuses part of its syntax); kept by content, so a run reads it back. */
const typescript = createRequire(path.join(root, 'package.json'))('typescript');
const cache = path.join(HERE, '..', 'out', 'cache40');
fs.mkdirSync(cache, { recursive: true });
function compiled(text) {
  const file = path.join(cache, `${createHash('sha256').update(typescript.version).update(text).digest('hex').slice(0, 32)}.mjs`);
  if (fs.existsSync(file)) return fs.readFileSync(file, 'utf8');
  const js = typescript.transpileModule(text, { compilerOptions: { target: typescript.ScriptTarget.ES2022, module: typescript.ModuleKind.ESNext, useDefineForClassFields: true } }).outputText;
  fs.writeFileSync(file, js);
  return js;
}

const MODEL_URLS = 'virtual:model-urls/';
/** The module the 4.0's plugin makes: every file of a folder of public/models with those extensions, by its path, as its address. */
function modelUrls(id) {
  const [folder = '', query = ''] = id.slice(MODEL_URLS.length).split('?');
  const extensions = new Set((new URLSearchParams(query).get('ext') ?? '').split(',').filter(Boolean));
  const dir = path.join(root, 'public', 'models', folder);
  const urls = {};
  const walk = (at, prefix) => {
    if (!fs.existsSync(at)) return;
    for (const e of fs.readdirSync(at, { withFileTypes: true })) {
      const rel = prefix ? `${prefix}/${e.name}` : e.name;
      if (e.isDirectory()) walk(path.join(at, e.name), rel);
      else if (extensions.has(path.extname(e.name).slice(1).toLowerCase())) urls[rel] = `/public/models/${folder}/${rel}`;
    }
  };
  walk(dir, '');
  return `export default ${JSON.stringify(urls)};`;
}

registerHooks({
  resolve(specifier, context, next) {
    // the 4.0's virtual module of model addresses (scripts/vite/model-urls-plugin.ts)
    if (specifier.startsWith(MODEL_URLS)) return { url: `data:text/javascript,${encodeURIComponent(modelUrls(specifier))}`, shortCircuit: true };
    const alias = aliases.find(([from]) => specifier.startsWith(from));
    const parent = context.parentURL?.startsWith('file:') ? path.dirname(new URL(context.parentURL).pathname.replace(/^\/([A-Za-z]:)/, '$1')) : null;
    const clean = specifier.replace(/\?.*$/, '');
    const base = alias ? alias[1] + clean.slice(alias[0].length) : clean.startsWith('.') && parent ? path.resolve(parent, clean) : null;
    const file = base ? asFile(base) : null;
    if (file) return { url: pathToFileURL(file).href + (specifier.includes('?') ? specifier.slice(specifier.indexOf('?')) : ''), shortCircuit: true };
    return next(specifier, context);
  },
  load(url, context, next) {
    const asked = new URL(url);
    if (asked.protocol !== 'file:' || asked.pathname.includes('/node_modules/')) {
      if (asked.protocol === 'file:' && asked.pathname.endsWith('.json')) return { format: 'module', source: `export default ${fs.readFileSync(asked, 'utf8')};`, shortCircuit: true };
      return next(url, context);
    }
    // what Vite gives a module: a file's text (?raw), its address (?url), JSON as a module
    const file = new URL(asked.pathname, 'file:');
    if (asked.search.includes('raw')) return { format: 'module', source: `export default ${JSON.stringify(fs.readFileSync(file, 'utf8'))};`, shortCircuit: true };
    if (asked.search.includes('url')) return { format: 'module', source: `export default ${JSON.stringify(asked.pathname)};`, shortCircuit: true };
    if (asked.pathname.endsWith('.json')) return { format: 'module', source: `export default ${fs.readFileSync(file, 'utf8')};`, shortCircuit: true };
    if (/\.tsx?$/.test(asked.pathname)) return { format: 'module', source: compiled(fs.readFileSync(file, 'utf8').replaceAll('import.meta.env', 'globalThis.__env40')), shortCircuit: true };
    // what is no code here (a style, a picture a module imports): an empty module
    if (!/\.(m?js|cjs|node)$/.test(asked.pathname)) return { format: 'module', source: 'export default "";', shortCircuit: true };
    return next(url, context);
  },
});

