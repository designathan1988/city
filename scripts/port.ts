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
import { folderName, portFile, readInventory } from '../src/convert/inventory.ts';
import type { GraphIR } from '../src/graph/ir.ts';
import { readWorkspace } from '../src/sandbox/workspace.ts';
import { addLayer, addMapLine, addStateMember } from '../src/translate/registry.ts';

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
const LOOK_TIMEOUT_S = 3600;
/** The shared 4.0 server's bookkeeping (inside the ignored cache): a lock, one lease per run using it, and who started it. */
const SERVER_DIR = path.join(CACHE, 'source-server');
const SERVER_STATE = path.join(SERVER_DIR, 'server.json');
const LOCK_STALE_MS = 30_000;
const LOCK_WAIT_MS = 10_000;
/** One photo run of the 4.0 at a time, machine-wide: measured on this machine one run takes 40-54 % of the 3D engine of its GPU, about 245 % of a core and 3 GB of video memory, so two at once freeze the desktop. */
const PHOTO_SLOT = path.join(SERVER_DIR, 'photo-slot.json');
const SLOT_POLL_MS = 2000;
const SLOT_TELL_MS = 30_000;
const SLOT_MAX_WAIT_MS = 45 * 60_000;
const SLOT_STALE_GRACE_S = 600;
/** Win32_ProcessStartup.ShowWindow takes the nCmdShow values of ShowWindow(); 0 is SW_HIDE (Microsoft Learn: Win32_ProcessStartup, Create method of Win32_Process). Without it the 4.0 server's console opens on the user's desktop. */
const SW_HIDE = 0;
const MIN_NODE = [23, 6];

const MACHINE_KEYS = ['source', 'game', 'ports', 'inventory', 'jelly', 'typescript', 'types', 'postcss', 'mingw', 'clangd', 'cxx', 'godot', 'bash', 'python'] as const;
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
  const run = spawnSync(process.execPath, ['--max-old-space-size=12288', CLI, ...args], { cwd: CONVERTER, env: environment(m), stdio: 'inherit', windowsHide: true });
  return run.status ?? 1;
}

function git(dir: string, args: readonly string[]): string {
  const run = spawnSync('git', ['-C', dir, ...args], { encoding: 'utf8', maxBuffer: 256 * 1024 * 1024, windowsHide: true });
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

interface Registration {
  readonly subsystem: string;
  readonly includes: readonly string[];
  readonly mapLine: string;
  readonly state: { readonly header: string; readonly anchor: string; readonly member: string; readonly root: string } | null;
}

/** The accepted files into the feature's clone, never the game: each new owner registered where the gates look. A file already there is refused. */
function applyConversion(m: Machine, feature: string, dir: string): number {
  const record = readWorkspace(portDir(m, feature));
  const files = path.join(dir, 'files');
  if (!fs.existsSync(files)) {
    console.log('apply: no accepted files');
    return 1;
  }
  const relative = (fs.readdirSync(files, { recursive: true, encoding: 'utf8' })).filter((f) => fs.statSync(path.join(files, f)).isFile());
  const taken = relative.filter((f) => fs.existsSync(path.join(record.workspace, f)));
  if (taken.length > 0) fail(`these files are already in the clone: ${taken.join(', ')}`);
  fs.cpSync(files, record.workspace, { recursive: true });
  const rulesFile = path.join(record.workspace, 'tools', 'gates', 'rules.json');
  const docFile = path.join(record.workspace, 'CLAUDE.md');
  const registrations = JSON.parse(fs.readFileSync(path.join(dir, 'registration.json'), 'utf8')) as Registration[];
  for (const r of registrations) {
    const rules = fs.readFileSync(rulesFile, 'utf8');
    const parsed = JSON.parse(rules) as { cpp_layers: Record<string, string[]>; map_markers: [string, string] };
    if (!parsed.cpp_layers[r.subsystem]) fs.writeFileSync(rulesFile, addLayer(rules, r.subsystem, r.includes), 'utf8');
    const doc = fs.readFileSync(docFile, 'utf8');
    if (!doc.includes(`\`${r.subsystem}/**\``)) fs.writeFileSync(docFile, addMapLine(doc, parsed.map_markers, r.subsystem, r.mapLine), 'utf8');
    if (r.state) {
      const root = path.join(record.workspace, r.state.root);
      const header = fs.readFileSync(root, 'utf8');
      if (!header.includes(`#include "${r.state.header}"`)) fs.writeFileSync(root, addStateMember(header, r.state.header, r.state.anchor, r.state.member), 'utf8');
    }
  }
  console.log(`apply: ${relative.length} files into ${record.workspace}; run: node scripts/port.ts verify ${feature}`);
  return 0;
}

/** A feature of the inventory (editor:poles) or a name already extracted, carried as far as facts allow: `converter convert`, then the clone with --apply. */
function convert(m: Machine, id: string | undefined, rest: readonly string[]): number {
  if (!id) fail('give a feature of the inventory (editor:poles) or the name of one already extracted');
  const known = readInventory(m.inventory).find((f) => f.id === id);
  const feature = known ? folderName(known.id) : featureName(id);
  const boundary = path.join(featureDir(feature), 'boundary.json');
  if (!fs.existsSync(boundary)) {
    if (!known) fail(`${id} is not in ${m.inventory} and has no extraction`);
    const code = extract(m, feature, known.sementes);
    if (code !== 0) return code;
  }
  const cache = programCache(m);
  if (!fs.existsSync(cache.graph)) fail(`${cache.graph} is missing: extract a feature first`);
  const targetGraph = path.join(CONVERTER, 'out', 'game', 'cppgraph.json');
  if (!fs.existsSync(targetGraph)) fail(`${targetGraph} is missing: run converter cppgraph on the game`);
  const dir = path.join(portDir(m, feature), 'conversion');
  const code = converter(m, ['convert', '--feature', feature, '--source-root', m.source, '--typescript', m.typescript, '--graph', cache.graph, '--boundary', boundary,
    '--target-root', m.game, '--target-graph', targetGraph, '--library', path.join(CONVERTER, 'configs', 'conversion-library.json'), '--ports', m.ports,
    '--cxx', m.cxx, '--constants', portFile(portDir(m, feature), 'constants.json', 'constantes.json'), '--out', dir]);
  console.log(`\nwhat stays open, unit by unit: ${path.join(dir, 'conversao.md')}`);
  if (rest.includes('--apply')) {
    const applied = applyConversion(m, feature, dir);
    if (applied !== 0) return applied;
  }
  return code;
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

/** The same question, answered at once: whether to start the server is decided under the lock. */
function answersNow(url: string): boolean {
  const probe = 'fetch(process.argv[1]).then((r) => process.exit(r.ok ? 0 : 1), () => process.exit(1))';
  return spawnSync(process.execPath, ['-e', probe, url], { stdio: 'ignore', windowsHide: true }).status === 0;
}

function alive(pid: number): boolean {
  try {
    process.kill(pid, 0);
    return true;
  } catch (error) {
    return (error as NodeJS.ErrnoException).code === 'EPERM';
  }
}

function sleepNow(ms: number): void {
  Atomics.wait(new Int32Array(new SharedArrayBuffer(4)), 0, 0, ms);
}

/** One owner at a time over the server's bookkeeping: a folder made atomically, taken over when older than LOCK_STALE_MS. */
function underLock<T>(work: () => T): T {
  fs.mkdirSync(SERVER_DIR, { recursive: true });
  const lock = path.join(SERVER_DIR, 'lock');
  const until = Date.now() + LOCK_WAIT_MS;
  for (;;) {
    try {
      fs.mkdirSync(lock);
      break;
    } catch (error) {
      if ((error as NodeJS.ErrnoException).code !== 'EEXIST') throw error;
      try {
        if (Date.now() - fs.statSync(lock).mtimeMs > LOCK_STALE_MS) fs.rmdirSync(lock);
      } catch {
        /* taken or freed meanwhile: look again */
      }
      if (Date.now() > until) fail(`the 4.0 server bookkeeping at ${lock} stayed locked for ${LOCK_WAIT_MS / 1000} s`);
      sleepNow(50);
    }
  }
  try {
    return work();
  } finally {
    fs.rmSync(lock, { recursive: true, force: true });
  }
}

const leaseFile = (pid: number): string => path.join(SERVER_DIR, 'leases', String(pid));

/** The runs using the server now; the lease of a run whose process is gone is removed. */
function liveLeases(): number[] {
  const dir = path.join(SERVER_DIR, 'leases');
  fs.mkdirSync(dir, { recursive: true });
  const live: number[] = [];
  for (const name of fs.readdirSync(dir)) {
    const pid = Number(name);
    if (Number.isInteger(pid) && alive(pid)) live.push(pid);
    else fs.rmSync(path.join(dir, name), { force: true });
  }
  return live;
}

/** The server a run started and recorded, while its process lives. */
function recordedServer(): number | null {
  try {
    const { pid } = JSON.parse(fs.readFileSync(SERVER_STATE, 'utf8')) as { pid: number };
    return alive(pid) ? pid : null;
  } catch {
    return null;
  }
}

/** Starts a process outside this process's tree (WMI makes it nobody's child): a run stopped with its whole tree must not take the shared server along. Its window is created hidden (ProcessStartupInformation.ShowWindow = SW_HIDE). */
function startOutsideTree(exe: string, args: readonly string[], cwd: string): number {
  const literal = (s: string): string => `'${s.replace(/'/g, "''")}'`;
  const line = [exe, ...args].map((s) => `"${s}"`).join(' ');
  const script = `$hidden = New-CimInstance -ClassName Win32_ProcessStartup -ClientOnly -Property @{ ShowWindow = [uint16]${SW_HIDE} }; $r = Invoke-CimMethod -ClassName Win32_Process -MethodName Create -Arguments @{ CommandLine = ${literal(line)}; CurrentDirectory = ${literal(cwd)}; ProcessStartupInformation = $hidden }; if ($r.ReturnValue -ne 0) { exit 1 }; $r.ProcessId`;
  const run = spawnSync('powershell', ['-NoProfile', '-NonInteractive', '-Command', script], { encoding: 'utf8', windowsHide: true });
  const pid = Number(String(run.stdout).trim());
  if (run.status !== 0 || !Number.isInteger(pid) || pid <= 0) fail(`could not start ${exe} outside this process's tree: ${String(run.stderr).trim()}`);
  return pid;
}

/** Stops the server a run started, only when no run holds a lease; a server nobody here started is never touched. */
function stopServerIfIdle(): boolean {
  if (liveLeases().length > 0) return false;
  const pid = recordedServer();
  if (pid !== null) spawnSync('taskkill', ['/PID', String(pid), '/T', '/F'], { stdio: 'ignore', windowsHide: true });
  fs.rmSync(SERVER_STATE, { force: true });
  return true;
}

let leaseReleased = false;
function releaseServer(): void {
  if (leaseReleased) return;
  leaseReleased = true;
  underLock(() => {
    fs.rmSync(leaseFile(process.pid), { force: true });
    stopServerIfIdle();
  });
}

interface SlotHolder {
  pid: number;
  who: string;
  since: number;
}

function readSlot(): SlotHolder | null {
  try {
    return JSON.parse(fs.readFileSync(PHOTO_SLOT, 'utf8')) as SlotHolder;
  } catch {
    return null;
  }
}

/** A holder is dead when its process is gone, or when it has held the slot longer than any run may (its own --timeout plus a grace): a reused PID cannot hold it forever. */
function slotIsStale(holder: SlotHolder | null, timeoutS: number): boolean {
  if (!holder) {
    // A file that cannot be read is a writer that died between creating it and writing it, once it is old.
    try {
      return Date.now() - fs.statSync(PHOTO_SLOT).mtimeMs > LOCK_STALE_MS;
    } catch {
      return false;
    }
  }
  return !alive(holder.pid) || Date.now() - holder.since > (timeoutS + SLOT_STALE_GRACE_S) * 1000;
}

let slotHeld = false;
function releasePhotoSlot(): void {
  if (!slotHeld) return;
  slotHeld = false;
  if (readSlot()?.pid === process.pid) fs.rmSync(PHOTO_SLOT, { force: true });
}

/** Waits for the machine-wide photo slot and takes it; released at exit, and taken over from a dead or overlong holder. */
async function takePhotoSlot(who: string, timeoutS: number): Promise<void> {
  fs.mkdirSync(SERVER_DIR, { recursive: true });
  process.on('exit', releasePhotoSlot);
  for (const signal of ['SIGINT', 'SIGTERM', 'SIGHUP'] as const) process.on(signal, () => process.exit(130));
  const started = Date.now();
  let told = 0;
  for (;;) {
    try {
      const fd = fs.openSync(PHOTO_SLOT, 'wx');
      fs.writeSync(fd, JSON.stringify({ pid: process.pid, who, since: Date.now() } satisfies SlotHolder));
      fs.closeSync(fd);
      slotHeld = true;
      if (told) console.log(`port: photo slot taken after ${Math.round((Date.now() - started) / 1000)} s`);
      return;
    } catch (error) {
      if ((error as NodeJS.ErrnoException).code !== 'EEXIST') throw error;
    }
    const holder = readSlot();
    if (slotIsStale(holder, timeoutS)) {
      fs.rmSync(PHOTO_SLOT, { force: true });
      continue;
    }
    if (Date.now() - started > SLOT_MAX_WAIT_MS) fail(`no photo slot in ${SLOT_MAX_WAIT_MS / 60_000} min; held by ${holder?.who ?? '?'} (pid ${holder?.pid ?? '?'})`);
    if (Date.now() - told > SLOT_TELL_MS) {
      console.log(`port: waiting for the photo slot, held by ${holder?.who ?? '?'} (pid ${holder?.pid ?? '?'}) for ${Math.round((Date.now() - (holder?.since ?? Date.now())) / 60_000)} min; waiting ${Math.round((Date.now() - started) / 1000)} s`);
      told = Date.now();
    }
    await new Promise((r) => setTimeout(r, SLOT_POLL_MS));
  }
}

/** A lease on the 4.0's dev server, started when nobody has it: two runs share one server, and the last to leave stops it. */
async function leaseServer(m: Machine): Promise<void> {
  process.on('exit', releaseServer);
  for (const signal of ['SIGINT', 'SIGTERM', 'SIGHUP'] as const) process.on(signal, () => process.exit(130));
  underLock(() => {
    liveLeases();
    fs.writeFileSync(leaseFile(process.pid), '');
    if (answersNow(SOURCE_SERVER) || recordedServer() !== null) return;
    const vite = path.join(m.source, 'node_modules', 'vite', 'bin', 'vite.js');
    const pid = startOutsideTree(process.execPath, [vite, '--host', '127.0.0.1', '--port', '5300', '--strictPort'], m.source);
    fs.writeFileSync(SERVER_STATE, JSON.stringify({ pid }));
  });
  const until = Date.now() + SERVER_WAIT_MS;
  while (!(await answers(SOURCE_SERVER))) {
    if (Date.now() > until) fail(`the 4.0's dev server did not answer at ${SOURCE_SERVER} in ${SERVER_WAIT_MS / 1000} s`);
    await new Promise((r) => setTimeout(r, POLL_MS));
  }
}

/** The 4.0's photo tool under a time limit: past it, the whole tree (node, Chrome) is stopped by its PID. */
function runLook(m: Machine, plan: string, out: string, timeoutS: number): Promise<number> {
  return new Promise((resolve) => {
    const child = spawn(process.execPath, [path.join('scripts', 'look.mjs'), path.resolve(plan), out], { cwd: m.source, stdio: 'inherit', windowsHide: true });
    let timedOut = false;
    const timer = setTimeout(() => {
      timedOut = true;
      console.error(`port: look.mjs passed ${timeoutS} s; its whole tree is stopped`);
      if (child.pid) spawnSync('taskkill', ['/PID', String(child.pid), '/T', '/F'], { stdio: 'ignore', windowsHide: true });
    }, timeoutS * 1000);
    child.on('exit', (code) => {
      clearTimeout(timer);
      resolve(timedOut ? 124 : (code ?? 1));
    });
  });
}

/** Photos of the 4.0 by its own tool (scripts/look.mjs and a plan), on its dev server, shared with the runs that use it at the same time. */
async function reference(m: Machine, feature: string, args: readonly string[]): Promise<number> {
  const at = args.indexOf('--timeout');
  const timeoutS = at >= 0 ? Number(args[at + 1]) : LOOK_TIMEOUT_S;
  if (!Number.isFinite(timeoutS) || timeoutS <= 0) fail('--timeout takes a number of seconds');
  const plan = (at >= 0 ? args.filter((_, i) => i !== at && i !== at + 1) : args)[0];
  if (!plan || !fs.existsSync(plan)) fail('give a look plan JSON (the 4.0\'s scripts/look format; city {"size": "small", "seed": 7})');
  const out = path.join(portDir(m, feature), 'reference');
  await takePhotoSlot(feature, timeoutS);
  await leaseServer(m);
  console.log(`\n> look ${plan} -> ${out}`);
  return runLook(m, plan, out, timeoutS);
}

/** What holds the shared server now, and stopping it when nobody does. */
async function serverCommand(action: string | undefined, rest: readonly string[]): Promise<number> {
  if (action === 'hold') {
    const seconds = Number(rest[0]);
    if (!Number.isFinite(seconds) || seconds <= 0) fail('server hold <seconds>');
    await takePhotoSlot('hold', LOOK_TIMEOUT_S);
    console.log(`port: holding the photo slot for ${seconds} s`);
    await new Promise((r) => setTimeout(r, seconds * 1000));
    return 0;
  }
  if (action === 'status') {
    const leases = underLock(() => liveLeases());
    console.log(`4.0 server ${SOURCE_SERVER}: ${answersNow(SOURCE_SERVER) ? 'answering' : 'not answering'}; started by a run: ${recordedServer() ?? 'no'}; runs holding it: ${leases.join(', ') || 'none'}`);
    const holder = readSlot();
    console.log(`photo slot: ${holder && !slotIsStale(holder, LOOK_TIMEOUT_S) ? `held by ${holder.who} (pid ${holder.pid}) for ${Math.round((Date.now() - holder.since) / 1000)} s` : 'free'}`);
    return 0;
  }
  if (action === 'stop') {
    const stopped = underLock(() => stopServerIfIdle());
    console.log(stopped ? 'no run holds the 4.0 server; the one a run started is stopped' : 'runs still hold the 4.0 server: left running');
    return stopped ? 0 : 1;
  }
  console.log(HELP);
  return 2;
}

const HELP = `node scripts/port.ts <command> [--machine <file>] ...
  doctor                          every path of the machine file, node, the game's state, the source cache
  extract <feature> <entry>...    the whole feature from the 4.0 -> out/features/<feature> (feature.md, source/, boundary.json)
  convert <feature> [--apply]     a feature of the inventory (editor:poles): every unit decided from facts, the clean ones translated and
                                  judged, the rest listed with the correct method -> <ports>/<feature>/conversion (conversao.md);
                                  exit 1 while anything is open; --apply copies the accepted files into the feature's clone
  workspace <feature>            isolated clone of the game on convert/<feature> -> <ports>/<feature>/workspace, configured
  verify <feature> [step]...      the game's own checks in the workspace (gates, build-release, build-debug, import, tests, capture)
  audit <feature>                 the methodology audit of the port's own lines (C++ graph of the workspace through clangd)
  reference <feature> <plan> [--timeout <s>]
                                  photos of the 4.0 by its scripts/look.mjs -> <ports>/<feature>/reference; runs share one dev server
                                  (the last to finish stops it) and look.mjs is stopped, tree and all, after --timeout (default 3600)
  server status|stop              who holds the shared 4.0 dev server and the photo slot; stop the server when no run does
  server hold <seconds>           takes the machine-wide photo slot for that long (test and operation)
  (reference waits for the photo slot: one 4.0 photo run at a time on the whole machine)
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
    case 'convert': return convert(m, name, rest);
    case 'workspace': return workspace(m, featureName(name));
    case 'verify': return verify(m, featureName(name), rest);
    case 'audit': return audit(m, featureName(name));
    case 'reference': return reference(m, featureName(name), rest);
    case 'server': return serverCommand(name, rest);
    case 'pack': return pack(m, featureName(name));
    case 'sandbox': return sandbox(m, featureName(name));
    default:
      console.log(HELP);
      return 2;
  }
}

process.exitCode = await main(process.argv.slice(2));
