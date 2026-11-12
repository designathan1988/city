// The converter's client of the repository's one orchestration owner (`python tools/tasks/cli.py serve`): every child
// process the converter starts is a task there, with its own deadline (the whole process tree is stopped past it), keyed
// by the content of its inputs (a result whose inputs did not change is not made again) and run in the shared pool.
// Two ways in: `runTask` (a long-lived serve process per converter process; answers come as each task finishes) and
// `runTasksSync` (one serve process for a batch, for code that cannot wait asynchronously). Content keys of what lies
// outside the repository (the 4.0 source, a clone, generated files) are computed here from content, never from times.
import { spawn, spawnSync, type ChildProcess } from 'node:child_process';
import { createHash } from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import readline from 'node:readline';
import { MAX_CHILD_OUTPUT_BYTES } from './files.ts';
import { machine } from './machine.ts';

/** How long the serve process itself may take to start and to answer beyond the deadlines of the tasks it runs. */
const SERVE_OVERHEAD_S = 60;
const MS_PER_SECOND = 1000;
/** A git query of a tree's state (ls-files, status, hash-object): it reads the index and hashes files, nothing more. */
const GIT_QUERY_TIMEOUT_MS = 120_000;

export type TaskStatus = 'PASS' | 'FAIL' | 'TIMEOUT' | 'BROKEN' | 'SKIPPED' | 'CACHED_PASS' | 'CACHED_FAIL';

export interface TaskSpec {
  /** Stable, file-name safe (it names the task's cache folder). */
  readonly name: string;
  readonly argv: readonly string[];
  /** Not part of the key: give paths in argv relative to it so the same work in another folder is the same task. */
  readonly cwd?: string;
  /** Files or globs (repository-relative) whose content keys the result. */
  readonly inputs?: readonly string[];
  /** Strings that also key it: content keys of what lies outside the repository, tool identities. */
  readonly extra?: readonly string[];
  /** Files and folders the task makes, restored from the cache on a hit. */
  readonly outputs?: readonly string[];
  readonly timeoutS: number;
  readonly env?: Readonly<Record<string, string>>;
  /** Run where no window reaches the user's desktop. */
  readonly hidden?: boolean;
}

export interface TaskResult {
  readonly id: string;
  readonly status: TaskStatus;
  readonly exit: number | null;
  readonly seconds: number;
  /** The task's combined standard output and error. */
  readonly log: string;
  readonly key: string;
}

export class TaskError extends Error {}

/** The task ran to its end and exited 0 (now or in the cached run). */
export const passed = (r: TaskResult): boolean => r.status === 'PASS' || r.status === 'CACHED_PASS';
/** The task ran to its end with an exit code: its log is its answer (a compiler that refuses a file did run). */
export const ran = (r: TaskResult): boolean => passed(r) || r.status === 'FAIL' || r.status === 'CACHED_FAIL';

export function logOf(r: TaskResult): string {
  if (!r.log || !fs.existsSync(r.log)) throw new TaskError(`task ${r.id} (${r.status}) left no log at ${r.log}`);
  return fs.readFileSync(r.log, 'utf8');
}

/** The result of a task that must have run to its end: a timeout, a broken or skipped task is an error that says so. */
export function mustRun(name: string, r: TaskResult): TaskResult {
  if (ran(r)) return r;
  let tail = '';
  try {
    tail = logOf(r).slice(-4000);
  } catch {
    tail = '(no log)';
  }
  throw new TaskError(`${name}: ${r.status}${r.exit !== null ? ` (exit ${r.exit})` : ''} after ${r.seconds} s:\n${tail}`);
}

const REPO = (): string => path.resolve(machine().get('repo'));

/** A path as the serve protocol takes it: repository-relative inside the repository, absolute (forward slashes) outside. */
function protocolPath(p: string): string {
  const abs = path.resolve(p);
  const rel = path.relative(REPO(), abs);
  return (!rel.startsWith('..') && !path.isAbsolute(rel) ? rel : abs).split(path.sep).join('/');
}

let nextId = 0;

function request(spec: TaskSpec, id: string): string {
  if (!(spec.timeoutS > 0)) throw new TaskError(`task ${spec.name} has no deadline`);
  return JSON.stringify({
    id, name: spec.name, argv: spec.argv, cwd: spec.cwd ? path.resolve(spec.cwd).split(path.sep).join('/') : null,
    inputs: spec.inputs ?? [], extra: spec.extra ?? [], outputs: (spec.outputs ?? []).map(protocolPath),
    timeout_s: spec.timeoutS, env: spec.env ?? null, hidden: spec.hidden ?? false,
  });
}

function serveCommand(): { exe: string; args: string[] } {
  const cli = machine().get('tasks');
  if (!fs.existsSync(cli)) throw new TaskError(`no tasks owner at ${cli} (configs/roadcraft-machine.json "tasks")`);
  return { exe: machine().get('python'), args: [cli, 'serve'] };
}

function parseResult(line: string): TaskResult {
  const r = JSON.parse(line) as { id: string; status: TaskStatus; exit: number | null; seconds: number; log: string; key: string };
  return { id: String(r.id), status: r.status, exit: r.exit ?? null, seconds: r.seconds, log: r.log, key: r.key };
}

/** One serve process for the converter process: tasks submitted while others run share its pool. */
class TaskServer {
  readonly #child: ChildProcess;
  readonly #pending = new Map<string, { resolve: (r: TaskResult) => void; reject: (e: Error) => void }>();
  #stderr = '';
  #dead: Error | null = null;

  constructor() {
    const { exe, args } = serveCommand();
    this.#child = spawn(exe, args, { cwd: REPO(), stdio: ['pipe', 'pipe', 'pipe'], windowsHide: true });
    readline.createInterface({ input: this.#child.stdout! }).on('line', (line) => {
      if (!line.trim()) return;
      const r = parseResult(line);
      const waiting = this.#pending.get(r.id);
      if (!waiting) return;
      this.#pending.delete(r.id);
      waiting.resolve(r);
      this.#idle();
    });
    this.#child.stderr!.on('data', (d: Buffer) => { this.#stderr = (this.#stderr + d.toString()).slice(-8000); });
    const died = (why: string): void => {
      this.#dead = new TaskError(`the tasks server ${why}:\n${this.#stderr}`);
      for (const w of this.#pending.values()) w.reject(this.#dead);
      this.#pending.clear();
      if (shared === this) shared = null;
    };
    this.#child.on('error', (e) => died(`could not run (${e.message})`));
    this.#child.on('exit', (code) => died(`exited (${code})`));
    // the converter ends when its own work does: the server is closed with it, never waited for while idle
    process.on('exit', () => this.close());
    this.#idle();
  }

  #idle(): void {
    const streams = [this.#child.stdout, this.#child.stderr, this.#child.stdin] as unknown as { ref?: () => void; unref?: () => void }[];
    if (this.#pending.size === 0) {
      this.#child.unref();
      for (const s of streams) s.unref?.();
    } else {
      this.#child.ref();
      for (const s of streams) s.ref?.();
    }
  }

  submit(spec: TaskSpec): Promise<TaskResult> {
    if (this.#dead) return Promise.reject(this.#dead);
    const id = String(++nextId);
    return new Promise((resolve, reject) => {
      this.#pending.set(id, { resolve, reject });
      this.#idle();
      this.#child.stdin!.write(`${request(spec, id)}\n`);
    });
  }

  close(): void {
    if (!this.#child.stdin!.destroyed) this.#child.stdin!.end();
  }
}

let shared: TaskServer | null = null;

/** One task through the shared server; resolves when it has finished (or was found in the cache). */
export function runTask(spec: TaskSpec): Promise<TaskResult> {
  shared ??= new TaskServer();
  return shared.submit(spec);
}

/** A batch through one serve process of its own, waited for: the batch's deadline is the sum of its tasks' and the server's own. */
export function runTasksSync(specs: readonly TaskSpec[]): TaskResult[] {
  if (specs.length === 0) throw new TaskError('an empty batch of tasks');
  const ids = specs.map(() => String(++nextId));
  const { exe, args } = serveCommand();
  const deadline = specs.reduce((s, t) => s + t.timeoutS, SERVE_OVERHEAD_S) * MS_PER_SECOND;
  const run = spawnSync(exe, args, { cwd: REPO(), input: specs.map((s, i) => request(s, ids[i]!)).join('\n') + '\n', encoding: 'utf8',
    timeout: deadline, maxBuffer: MAX_CHILD_OUTPUT_BYTES, windowsHide: true });
  if (run.error) throw new TaskError(`the tasks server: ${run.error.message}\n${run.stderr ?? ''}`);
  const byId = new Map(String(run.stdout).split(/\r?\n/).filter((l) => l.trim()).map(parseResult).map((r) => [r.id, r]));
  return ids.map((id, i) => {
    const r = byId.get(id);
    if (!r) throw new TaskError(`the tasks server (exit ${run.status}) gave no result for ${specs[i]!.name}:\n${run.stderr}`);
    return r;
  });
}

// ---- content keys

const sha = (...parts: readonly string[]): string => createHash('sha256').update(parts.join('\0')).digest('hex');

function gitQuery(cwd: string, args: readonly string[], input?: string): { ok: boolean; out: string } {
  const r = spawnSync('git', args, { cwd, encoding: 'utf8', input, maxBuffer: MAX_CHILD_OUTPUT_BYTES, timeout: GIT_QUERY_TIMEOUT_MS, windowsHide: true });
  if (r.error && (r.error as NodeJS.ErrnoException).code === 'ETIMEDOUT') throw new TaskError(`git ${args.join(' ')} in ${cwd} passed ${GIT_QUERY_TIMEOUT_MS / MS_PER_SECOND} s`);
  return { ok: r.status === 0, out: String(r.stdout ?? '') };
}

function walk(dir: string, out: string[], root: string): void {
  for (const entry of fs.readdirSync(dir, { withFileTypes: true }).sort((a, b) => (a.name < b.name ? -1 : 1))) {
    if (entry.name === '.git' || entry.name === 'node_modules') continue;
    const full = path.join(dir, entry.name);
    if (entry.isDirectory()) walk(full, out, root);
    else if (entry.isFile()) out.push(`${path.relative(root, full).split(path.sep).join('/')} ${createHash('sha256').update(fs.readFileSync(full)).digest('hex')}`);
  }
}

/**
 * The content of a folder (or of the pathspecs under it) as one key, read now. In a git work tree: the blob ids of the
 * tracked files, `git hash-object` of the modified and untracked ones (ignored files are not the tree's content).
 * Elsewhere: every file hashed. Never remembered here: a tree the converter changes (a sandbox after its patches) has a
 * new key; a caller that reads one tree many times and never writes it keeps the key itself.
 */
export function treeKey(dir: string, pathspecs: readonly string[] = []): string {
  const abs = path.resolve(dir);
  let key: string;
  const top = gitQuery(abs, ['rev-parse', '--show-toplevel']);
  if (top.ok) {
    const root = top.out.trim();
    const specs = pathspecs.length > 0 ? [...pathspecs] : ['.'];
    const listed = gitQuery(abs, ['ls-files', '-s', '-z', '--full-name', '--', ...specs]).out.split('\0').filter(Boolean)
      .map((l) => { const tab = l.indexOf('\t'); return [l.slice(tab + 1), l.slice(0, tab).split(' ')[1]!] as [string, string]; });
    const blobs = new Map(listed);
    const status = gitQuery(abs, ['status', '--porcelain=v1', '-z', '--untracked-files=all', '--', ...specs]).out.split('\0').filter(Boolean);
    const changed: string[] = [];
    for (let i = 0; i < status.length; i++) {
      const entry = status[i]!;
      const file = entry.slice(3);
      if (entry[0] === 'R' || entry[0] === 'C') i++; // the next field is the old name
      if (fs.existsSync(path.join(root, file)) && fs.statSync(path.join(root, file)).isFile()) changed.push(file);
      else blobs.set(file, 'deleted');
    }
    if (changed.length > 0) {
      const hashes = gitQuery(root, ['hash-object', '--stdin-paths'], changed.join('\n') + '\n').out.split(/\r?\n/).filter(Boolean);
      changed.forEach((f, i) => blobs.set(f, hashes[i] ?? 'unreadable'));
    }
    key = sha('git', ...[...blobs].sort(([a], [b]) => (a < b ? -1 : 1)).map(([f, b]) => `${f} ${b}`));
  } else {
    const rows: string[] = [];
    const roots = pathspecs.length > 0 ? pathspecs.map((p) => path.join(abs, p)) : [abs];
    for (const r of roots) if (fs.existsSync(r)) (fs.statSync(r).isDirectory() ? walk(r, rows, abs) : rows.push(`${path.relative(abs, r)} ${createHash('sha256').update(fs.readFileSync(r)).digest('hex')}`));
    key = sha('files', ...rows);
  }
  return key;
}

/** Every file under a folder hashed, whatever git says of it (a copy made outside version control), as one key. */
export function folderKey(dir: string): string {
  const rows: string[] = [];
  if (fs.existsSync(dir)) walk(dir, rows, dir);
  return sha('folder', ...rows);
}

/** The content of named files (missing ones count as missing), as one key. */
export function filesKey(files: readonly string[]): string {
  return sha(...files.map((f) => `${path.resolve(f)}\0${fs.existsSync(f) ? createHash('sha256').update(fs.readFileSync(f)).digest('hex') : 'missing'}`));
}

/** Texts held in memory (generated files), by name, as one key. */
export function textsKey(texts: Iterable<readonly [string, string]>): string {
  return sha(...[...texts].sort(([a], [b]) => (a < b ? -1 : 1)).map(([f, t]) => `${f}\0${t}`));
}

export const keyOf = (...parts: readonly string[]): string => sha(...parts);

const toolKeys = new Map<string, string>();

/** An external tool's identity: its executable's content, or its package's manifest (read once per process). */
export function toolKey(tool: string): string {
  const known = toolKeys.get(tool);
  if (known !== undefined) return known;
  const manifest = path.join(tool, 'package.json');
  const key = fs.existsSync(manifest) ? sha('package', fs.readFileSync(manifest, 'utf8')) : filesKey([tool]);
  toolKeys.set(tool, key);
  return key;
}

/** A folder named by a key under the converter's cache: what a key makes lives there, so nothing is wiped to be made again. */
export function keyedDir(kind: string, key: string): string {
  return path.join(machine().get('cache'), kind, key.slice(0, 2), key);
}
