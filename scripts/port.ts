// The driver for porting one feature of the Roadcraft 4.0 into the game: one command per stage, every path from
// configs/roadcraft-machine.json, every stage a converter command. The source's code (the extracted feature, the
// cached program analysis) stays in the converter's out/; the game's code (the workspace, its evidence, the package)
// goes to the port folder <ports>/<feature>. The game is only read. `node scripts/port.ts help`.
import { spawn, spawnSync } from 'node:child_process';
import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import { RULE_BY_ID } from '../src/audit/catalog.ts';
import { changedLines, findingsInChanges } from '../src/audit/changed.ts';
import type { AuditResult } from '../src/audit/run.ts';
import { removeTree } from '../src/common/files.ts';
import type { GraphIR } from '../src/graph/ir.ts';
import { readWorkspace } from '../src/sandbox/workspace.ts';

const CONVERTER = path.resolve(import.meta.dirname, '..');
const CLI = path.join(CONVERTER, 'src', 'cli.ts');
const SANDBOX_CONFIG = path.join(CONVERTER, 'configs', 'roadcraft-game.sandbox.json');
const AUDIT_CONFIG = path.join(CONVERTER, 'configs', 'roadcraft-game.audit.json');
const FEATURES = path.join(CONVERTER, 'out', 'features');
const CACHE = path.join(CONVERTER, 'out', 'cache');
/** The 4.0's dev server, where its own photo tool (scripts/look.mjs) looks. */
const SOURCE_SERVER = 'http://127.0.0.1:5300/';
const SERVER_WAIT_MS = 120_000;
const POLL_MS = 1000;
const MIN_NODE = [23, 6];

const MACHINE_KEYS = ['source', 'game', 'ports', 'jelly', 'typescript', 'types', 'postcss', 'mingw', 'clangd', 'cxx', 'godot', 'bash', 'python'] as const;
type Machine = Record<(typeof MACHINE_KEYS)[number], string>;

function fail(message: string): never {
  console.error(`port: ${message}`);
  process.exit(1);
}

/** The machine file with every `${key}` resolved. */
function readMachine(file: string): Machine {
  const raw = JSON.parse(fs.readFileSync(file, 'utf8')) as Record<string, unknown>;
  const resolve = (key: string, seen: readonly string[]): string => {
    const value = raw[key];
    if (typeof value !== 'string') fail(`${file} has no "${key}"`);
    if (seen.includes(key)) fail(`${file}: "${key}" refers to itself`);
    return value.replace(/\$\{(\w+)\}/g, (_m, other: string) => resolve(other, [...seen, key]));
  };
  return Object.fromEntries(MACHINE_KEYS.map((k) => [k, resolve(k, [])])) as Machine;
}

function environment(m: Machine): NodeJS.ProcessEnv {
  return {
    ...process.env, GODOT: m.godot, MINGW: m.mingw, BASH: m.bash, PYTHON: m.python, CONVERTER_JELLY: m.jelly,
    CONVERTER_TYPESCRIPT: m.typescript, CONVERTER_POSTCSS: m.postcss, CONVERTER_CLANGD: m.clangd, CONVERTER_CXX: m.cxx,
  };
}

/** Runs a converter command, its output shown as it comes; returns its exit code. */
function converter(m: Machine, args: readonly string[]): number {
  console.log(`\n> converter ${args.join(' ')}`);
  const run = spawnSync(process.execPath, ['--max-old-space-size=12288', CLI, ...args], { cwd: CONVERTER, env: environment(m), stdio: 'inherit' });
  return run.status ?? 1;
}

function git(dir: string, args: readonly string[]): string {
  const run = spawnSync('git', ['-C', dir, ...args], { encoding: 'utf8', maxBuffer: 256 * 1024 * 1024 });
  if (run.status !== 0) fail(`git ${args.join(' ')} in ${dir}: ${run.stderr.trim()}`);
  return run.stdout;
}

const hash = (...parts: readonly string[]): string => crypto.createHash('sha256').update(parts.join('\0')).digest('hex').slice(0, 16);
const contents = (files: readonly string[]): string => files.map((f) => `${f}\0${fs.readFileSync(f, 'utf8')}`).join('\0');

/** What identifies the source as it is now: its commit, its uncommitted changes and their sizes and times. */
function sourceFingerprint(root: string): string {
  const status = git(root, ['status', '--porcelain', '--untracked-files=all']);
  const changed = status.split(/\r?\n/).filter(Boolean).map((line) => line.slice(3).split(' -> ').at(-1)!.replace(/^"|"$/g, ''));
  const stats = changed.map((file) => {
    const full = path.join(root, file);
    if (!fs.existsSync(full)) return `${file} gone`;
    const s = fs.statSync(full);
    return `${file} ${s.size} ${s.mtimeMs}`;
  });
  return hash(git(root, ['rev-parse', 'HEAD']), status, ...stats);
}

/** The whole program's graph and Jelly's analysis, kept per state of the source and of the converter code that makes them. */
function programCache(m: Machine): { readonly graph: string; readonly jelly: string } {
  const graphCode = fs.readdirSync(path.join(CONVERTER, 'src', 'graph')).map((f) => path.join(CONVERTER, 'src', 'graph', f));
  const dir = path.join(CACHE, `source-${sourceFingerprint(m.source)}`);
  return {
    graph: path.join(dir, `program-${hash(contents(graphCode))}.json`),
    jelly: path.join(dir, `jelly-${hash(contents([path.join(CONVERTER, 'src', 'feature', 'callgraph.ts'), path.join(m.jelly, 'package.json')]))}`),
  };
}

const FEATURE_NAME = /^[a-z0-9][a-z0-9-]*$/;
function featureName(name: string | undefined): string {
  if (!name || !FEATURE_NAME.test(name)) fail(`give a feature name: lower case, digits and dashes (got "${name ?? ''}")`);
  return name;
}

const portDir = (m: Machine, feature: string): string => path.join(m.ports, feature);
const featureDir = (feature: string): string => path.join(FEATURES, feature);

function doctor(m: Machine): number {
  let bad = 0;
  const check = (what: string, ok: boolean, detail: string): void => {
    bad += ok ? 0 : 1;
    console.log(`${ok ? 'ok  ' : 'FAIL'} ${what}: ${detail}`);
  };
  const [major = 0, minor = 0] = process.versions.node.split('.').map(Number);
  check('node', major > MIN_NODE[0]! || (major === MIN_NODE[0] && minor >= MIN_NODE[1]!), process.versions.node);
  for (const key of MACHINE_KEYS) {
    // The port folder is output: made on first use, so only where it goes must exist.
    if (key === 'ports' && !fs.existsSync(m.ports)) check(key, fs.existsSync(path.dirname(m.ports)), `${m.ports} (made on first use)`);
    else check(key, fs.existsSync(m[key]), m[key]);
  }
  if (fs.existsSync(m.game)) {
    const status = git(m.game, ['status', '--porcelain']).trim();
    console.log(`info game: ${git(m.game, ['log', '-1', '--format=%h %s']).trim()}${status ? `; not committed: ${status.split(/\r?\n/).join(', ')}` : ''}`);
  }
  if (fs.existsSync(m.source)) {
    const cache = programCache(m);
    console.log(`info source cache: graph ${fs.existsSync(cache.graph) ? 'ready' : 'to build'}, Jelly ${fs.existsSync(cache.jelly) ? 'ready' : 'to run'} (${path.dirname(cache.graph)})`);
  }
  console.log(bad === 0 ? '\ndoctor OK' : `\ndoctor: ${bad} problem(s); fix ${path.join(CONVERTER, 'configs', 'roadcraft-machine.json')}`);
  return bad === 0 ? 0 : 1;
}

function extract(m: Machine, feature: string, entries: readonly string[]): number {
  if (entries.length === 0) fail('give the feature\'s entry files, relative to the 4.0 root (e.g. src/editor/poles.ts)');
  const cache = programCache(m);
  if (!fs.existsSync(cache.graph)) {
    const code = converter(m, ['graph', '--source-root', m.source, '--typescript', m.typescript, '--out', cache.graph]);
    if (code !== 0) return code;
  }
  const out = featureDir(feature);
  const analysis = fs.existsSync(cache.jelly) ? ['--analysis', cache.jelly] : ['--jelly', m.jelly];
  const code = converter(m, ['feature', '--source-root', m.source, '--typescript', m.typescript, '--postcss', m.postcss, '--graph', cache.graph,
    ...analysis, ...entries.flatMap((e) => ['--entry', e]), '--out', out]);
  if (code !== 0) return code;
  if (!fs.existsSync(cache.jelly)) fs.cpSync(path.join(out, 'jelly'), cache.jelly, { recursive: true });
  console.log(`\nfeature ${feature}: read ${path.join(out, 'feature.md')}, then the code in ${path.join(out, 'source')} and the shared ` +
    `declarations it uses in ${path.join(out, 'interface')} (boundary.json has every list)`);
  return 0;
}

function workspace(m: Machine, feature: string): number {
  if (!fs.existsSync(featureDir(feature))) console.log(`note: no extraction of ${feature} yet (node scripts/port.ts extract ${feature} <entry>...)`);
  return converter(m, ['workspace', '--game', m.game, '--feature', feature, '--port', portDir(m, feature), '--config', SANDBOX_CONFIG]);
}

function verify(m: Machine, feature: string, steps: readonly string[]): number {
  return converter(m, ['verify', '--port', portDir(m, feature), '--config', SANDBOX_CONFIG, ...steps.flatMap((s) => ['--step', s])]);
}

function audit(m: Machine, feature: string): number {
  const port = portDir(m, feature);
  const record = readWorkspace(port);
  const config = JSON.parse(fs.readFileSync(AUDIT_CONFIG, 'utf8')) as { include: string[]; compileCommands: string; rules: string; flags: string[] };
  const compileCommands = path.join(record.workspace, config.compileCommands);
  if (!fs.existsSync(compileCommands)) fail(`${compileCommands} is missing: the workspace's debug build is configured without CMAKE_EXPORT_COMPILE_COMMANDS`);
  const dir = path.join(port, 'audit');
  const graph = path.join(dir, 'cppgraph.json');
  const result = path.join(dir, 'audit.json');
  let code = converter(m, ['cppgraph', '--root', record.workspace, '--compile-commands', compileCommands, '--clangd', m.clangd,
    `--clangd-arg=--query-driver=${m.cxx}`, ...config.include.flatMap((i) => ['--include', i]), '--cache', path.join(dir, 'clangd'), '--out', graph]);
  if (code !== 0) return code;
  code = converter(m, ['audit', '--graph', graph, '--target-rules', path.join(record.workspace, config.rules), ...config.flags,
    '--out', result, '--markdown', path.join(dir, 'audit.md')]);
  if (code !== 0) return code;
  // Only the port is judged here: the findings in lines it added or changed.
  const findings = findingsInChanges((JSON.parse(fs.readFileSync(result, 'utf8')) as AuditResult).findings,
    JSON.parse(fs.readFileSync(graph, 'utf8')) as GraphIR, changedLines(record.workspace, record.base));
  const report = [`# Audit of the port ${feature}`, '', `Findings in code the port added or changed since ${record.base.slice(0, 12)}: ${findings.length}.`, '',
    ...findings.flatMap((f) => {
      const rule = RULE_BY_ID.get(f.rule);
      return [`## ${f.severity} ${f.rule}: ${f.file}:${f.line}`, '', f.message, '',
        ...(rule ? [`- Wrong: ${rule.problem}`, `- Correct: ${rule.methodology}`, `- In the game: ${rule.target}`,
          `- Sources: ${rule.sources.map((s) => `[${s.title}](${s.url})`).join('; ')}`] : []), ''];
    })].join('\n');
  fs.writeFileSync(path.join(dir, 'port-findings.md'), report, 'utf8');
  for (const f of findings) console.log(`${f.severity.padEnd(6)} ${f.rule} ${f.file}:${f.line} ${f.message}`);
  console.log(`\naudit of the port: ${findings.length} finding(s) in its code -> ${path.join(dir, 'port-findings.md')}`);
  return findings.length === 0 ? 0 : 1;
}

function pack(m: Machine, feature: string): number {
  const out = path.join(portDir(m, feature), 'package');
  removeTree(out);
  return converter(m, ['pack', '--port', portDir(m, feature), '--feature-dir', featureDir(feature), '--source-root', m.source, '--out', out]);
}

function sandbox(m: Machine, feature: string): number {
  const dir = path.join(portDir(m, feature), 'sandbox');
  removeTree(dir);
  return converter(m, ['sandbox', '--package', path.join(portDir(m, feature), 'package'), '--config', SANDBOX_CONFIG, '--dir', dir]);
}

async function answers(url: string): Promise<boolean> {
  try {
    return (await fetch(url)).ok;
  } catch {
    return false;
  }
}

/** Photos of the 4.0 by its own tool (scripts/look.mjs and a plan), on its dev server; a server this command started is stopped by its PID. */
async function reference(m: Machine, feature: string, plan: string | undefined): Promise<number> {
  if (!plan || !fs.existsSync(plan)) fail('give a look plan JSON (the 4.0\'s scripts/look format; city {"size": "small", "seed": 7})');
  const out = path.join(portDir(m, feature), 'reference');
  let server: ReturnType<typeof spawn> | null = null;
  if (!(await answers(SOURCE_SERVER))) {
    server = spawn(process.execPath, [path.join(m.source, 'node_modules', 'vite', 'bin', 'vite.js'), '--host', '127.0.0.1', '--port', '5300', '--strictPort'],
      { cwd: m.source, stdio: 'ignore' });
    const until = Date.now() + SERVER_WAIT_MS;
    while (!(await answers(SOURCE_SERVER))) {
      if (Date.now() > until) fail(`the 4.0's dev server did not answer at ${SOURCE_SERVER} in ${SERVER_WAIT_MS / 1000} s`);
      await new Promise((r) => setTimeout(r, POLL_MS));
    }
  }
  try {
    console.log(`\n> look ${plan} -> ${out}`);
    return spawnSync(process.execPath, [path.join('scripts', 'look.mjs'), path.resolve(plan), out], { cwd: m.source, stdio: 'inherit' }).status ?? 1;
  } finally {
    // Vite starts helpers of its own: the whole tree of the process started here goes, and nothing else.
    if (server?.pid) spawnSync('taskkill', ['/PID', String(server.pid), '/T', '/F'], { stdio: 'ignore' });
  }
}

const HELP = `node scripts/port.ts <command> [--machine <file>] ...
  doctor                          every path of the machine file, node, the game's state, the source cache
  extract <feature> <entry>...    the whole feature from the 4.0 -> out/features/<feature> (feature.md, source/, boundary.json)
  workspace <feature>             isolated clone of the game on convert/<feature> -> <ports>/<feature>/workspace, configured
  verify <feature> [step]...      the game's own checks in the workspace (gates, build-release, build-debug, import, tests, capture)
  audit <feature>                 the methodology audit of the port's own lines (C++ graph of the workspace through clangd)
  reference <feature> <plan>      photos of the 4.0 by its scripts/look.mjs -> <ports>/<feature>/reference
  pack <feature>                  the branch's commits as a package -> <ports>/<feature>/package
  sandbox <feature>               the package verified from a fresh clone, before and after; prints the manual integration`;

async function main(argv: readonly string[]): Promise<number> {
  const at = argv.indexOf('--machine');
  const machineFile = at >= 0 ? argv[at + 1] : path.join(CONVERTER, 'configs', 'roadcraft-machine.json');
  const args = at >= 0 ? [...argv.slice(0, at), ...argv.slice(at + 2)] : [...argv];
  const [command, name, ...rest] = args;
  if (!command || command === 'help' || !machineFile) {
    console.log(HELP);
    return command ? 0 : 2;
  }
  const m = readMachine(machineFile);
  switch (command) {
    case 'doctor': return doctor(m);
    case 'extract': return extract(m, featureName(name), rest);
    case 'workspace': return workspace(m, featureName(name));
    case 'verify': return verify(m, featureName(name), rest);
    case 'audit': return audit(m, featureName(name));
    case 'reference': return reference(m, featureName(name), rest[0]);
    case 'pack': return pack(m, featureName(name));
    case 'sandbox': return sandbox(m, featureName(name));
    default:
      console.log(HELP);
      return 2;
  }
}

process.exitCode = await main(process.argv.slice(2));
