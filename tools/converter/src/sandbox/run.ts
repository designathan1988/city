// Tests a conversion package without touching the game: an isolated clone of the game at the package's base commit
// gets the files the build needs, runs the game's own setup and verification (the base, "before"), receives the
// package with `git am` on a branch of its own, and runs the verification again ("after"). Every step's output and
// the photos go to the package's evidence folder. The game is only read (cloned, copied from); its state is
// fingerprinted before and after, and a difference, which can only come from another process working on the game
// meanwhile, is recorded so that the result says which base the evidence was taken against.
import fs from 'node:fs';
import path from 'node:path';
import { cloneIsolated, fingerprint, git } from '../common/git.ts';
import { normalizePath } from '../common/paths.ts';
import { folderKey, keyOf, logOf, ran, runTasksSync, treeKey, type TaskResult, type TaskSpec } from '../common/tasks.ts';
import { EVIDENCE, PATCHES, readManifest } from '../package/manifest.ts';
import { expand, type Expectation, type SandboxConfig, type Step, type Variables } from './config.ts';

export interface StepResult {
  readonly phase: string;
  readonly name: string;
  readonly ok: boolean;
  readonly seconds: number;
  readonly problem: string | null;
  readonly log: string;
}

export interface SandboxResult {
  readonly feature: string;
  readonly game: string;
  readonly sandbox: string;
  readonly base: string;
  readonly branch: string;
  readonly steps: StepResult[];
  readonly ok: boolean;
  /** The game's repository changed while the sandbox ran (another process): the base was cloned before that. */
  readonly gameChanged: boolean;
}

/**
 * A step as a task: run in the clone, keyed by the clone's content (its tracked files, as committed and as changed, and
 * the copies made into it), the copies' content and the step's own command, so the same tree and step give the same
 * result; what it makes for later steps is restored with it.
 */
function taskOf(step: Step, vars: Variables, cloneKey: string): TaskSpec {
  const argv = step.run.map((part) => expand(part, vars));
  return { name: `converter-sandbox-${step.name}`, argv, cwd: vars.sandbox, extra: [cloneKey], timeoutS: step.timeoutSeconds, hidden: step.hidden ?? false,
    outputs: (step.outputs ?? []).map((o) => expand(o, vars)) };
}

/** What a step's run says: its log kept in the evidence, its exit code. */
function record(step: Step, task: TaskSpec, result: TaskResult, logFile: string): { code: number | null; output: string } {
  let output: string;
  try {
    output = logOf(result);
  } catch (error) {
    output = (error as Error).message;
  }
  fs.writeFileSync(logFile, `$ ${task.argv.join(' ')}\n(${result.status} in ${result.seconds} s)\n\n${output}`, 'utf8');
  return { code: ran(result) ? result.exit : null, output: ran(result) ? output : `${step.name}: ${result.status}\n${output}` };
}

/** The first expectation the run fails, or null. */
function check(expect: Expectation | undefined, code: number | null, output: string, vars: Variables): string | null {
  const wanted = expect?.exitCode ?? 0;
  if (code !== wanted) return `exit code ${code}, expected ${wanted}`;
  if (!expect) return null;
  let text = output;
  if (expect.file) {
    const file = expand(expect.file, vars);
    if (!fs.existsSync(file)) return `${file} was not written`;
    text = fs.readFileSync(file, 'utf8');
  }
  for (const needle of expect.includes ?? []) if (!text.includes(needle)) return `output lacks "${needle}"`;
  for (const needle of expect.excludes ?? []) if (text.includes(needle)) return `output contains "${needle}"`;
  if (expect.atLeast) {
    const count = text.split(expect.atLeast.text).length - 1;
    if (count < expect.atLeast.count) return `"${expect.atLeast.text}" appears ${count} times, expected at least ${expect.atLeast.count}`;
  }
  if (expect.files) {
    const dir = expand(expect.files.dir, vars);
    const count = fs.existsSync(dir) ? fs.readdirSync(dir).length : 0;
    if (count < expect.files.count) return `${count} files in ${dir}, expected at least ${expect.files.count}`;
  }
  return null;
}

/**
 * A phase's steps: each runs as soon as the steps it comes after have passed, beside the others; a failing step leaves
 * only the steps that need it undone (recorded as skipped), every independent step still runs. A verification with no
 * step proves nothing and fails; a setup may have none. A step it comes after that this run does not hold (a workspace
 * verified step by step) is taken as done: its products are the clone's.
 */
export function runPhase(phase: string, steps: readonly Step[], config: SandboxConfig, vars: Variables, results: StepResult[], verification = true): boolean {
  fs.mkdirSync(vars.evidence, { recursive: true });
  if (steps.length === 0) {
    if (!verification) return true;
    results.push({ phase, name: '(none)', ok: false, seconds: 0, problem: 'no verification step was run: an empty verification proves nothing', log: '' });
    return false;
  }
  const inRun = new Set(steps.map((s) => s.name));
  const outcome = new Map<string, boolean>();
  // the copies made into the clone are outside version control: their content keys the steps with the tracked tree
  const cloneKey = keyOf(treeKey(vars.sandbox), ...config.copy.map((c) => folderKey(path.join(vars.sandbox, c))));
  let left = [...steps];
  while (left.length > 0) {
    const needs = (s: Step): string[] => (s.after ?? []).filter((n) => inRun.has(n));
    const blocked = left.filter((s) => needs(s).some((n) => outcome.get(n) === false));
    for (const s of blocked) {
      const missing = needs(s).filter((n) => outcome.get(n) === false);
      results.push({ phase, name: s.name, ok: false, seconds: 0, problem: `skipped: it comes after ${missing.join(', ')}, which failed`, log: '' });
      outcome.set(s.name, false);
    }
    left = left.filter((s) => !blocked.includes(s));
    const ready = left.filter((s) => needs(s).every((n) => outcome.get(n) === true));
    if (ready.length === 0) {
      if (left.length > 0) throw new Error(`steps ${left.map((s) => s.name).join(', ')} wait on each other`);
      break;
    }
    const tasks = ready.map((s) => taskOf(s, vars, cloneKey));
    const done = runTasksSync(tasks);
    ready.forEach((step, i) => {
      const log = path.join(vars.evidence, `${step.name}.log`);
      const { code, output } = record(step, tasks[i]!, done[i]!, log);
      const problem = check(step.expect, code, output, vars);
      results.push({ phase, name: step.name, ok: problem === null, seconds: Math.round(done[i]!.seconds), problem, log });
      outcome.set(step.name, problem === null);
    });
    left = left.filter((s) => !ready.includes(s));
  }
  return steps.every((s) => outcome.get(s.name) === true);
}

export interface SandboxInput {
  readonly packageDir: string;
  readonly config: SandboxConfig;
  /** Where the sandbox clone goes; must not exist yet and must be outside the game. */
  readonly sandboxDir: string;
}

/** An isolated clone of the game at `base`, with what the build needs copied in (read-only from the game). */
export function prepare(game: string, base: string, sandbox: string, config: SandboxConfig): void {
  if (normalizePath(sandbox).startsWith(normalizePath(game) + '/')) throw new Error(`the sandbox must be outside the game (${sandbox})`);
  if (fs.existsSync(sandbox)) throw new Error(`${sandbox} already exists`);
  cloneIsolated(game, sandbox, base);
  for (const relative of config.copy) {
    const from = path.join(game, relative);
    if (!fs.existsSync(from)) throw new Error(`the configuration copies ${relative}, which the game does not have`);
    fs.cpSync(from, path.join(sandbox, relative), { recursive: true });
  }
}

export interface BaselineInput {
  readonly gameRoot: string;
  readonly base?: string | undefined;
  readonly config: SandboxConfig;
  readonly sandboxDir: string;
  readonly evidenceDir: string;
}

/** The game's own setup and verification on its base, in a sandbox: proves a configuration and keeps the base's evidence. */
export function runBaseline(input: BaselineInput): { readonly ok: boolean; readonly base: string; readonly steps: StepResult[]; readonly gameChanged: boolean } {
  const game = normalizePath(input.gameRoot);
  const base = git(game, ['rev-parse', '--verify', `${input.base ?? 'HEAD'}^{commit}`]).trim();
  const sandbox = path.resolve(input.sandboxDir);
  const before = fingerprint(game);
  const results: StepResult[] = [];
  prepare(game, base, sandbox, input.config);
  const evidence = (phase: string): Variables => ({ sandbox, evidence: path.join(path.resolve(input.evidenceDir), phase), game });
  const ok = runPhase('setup', input.config.setup, input.config, evidence('setup'), results, false) &&
    runPhase('before', input.config.verify, input.config, evidence('before'), results);
  return { ok, base, steps: results, gameChanged: fingerprint(game) !== before };
}

export function runSandbox(input: SandboxInput): SandboxResult {
  const packageDir = path.resolve(input.packageDir);
  const manifest = readManifest(packageDir);
  const game = manifest.game.root;
  const sandbox = path.resolve(input.sandboxDir);
  const before = fingerprint(game);
  const results: StepResult[] = [];
  const branch = `convert/${manifest.feature}`;
  const evidence = (phase: string): Variables => ({ sandbox, evidence: path.join(packageDir, EVIDENCE, phase), game });
  prepare(game, manifest.game.base, sandbox, input.config);
  let ok = runPhase('setup', input.config.setup, input.config, evidence('setup'), results, false) &&
    runPhase('before', input.config.verify, input.config, evidence('before'), results);
  if (ok) {
    // The base's verification leaves outputs of its own in the clone (a Godot import writes a .uid the game may
    // lack). They are recorded and the clone goes back to the base, so the package is applied to the base exactly;
    // ignored files (builds, the import cache) stay.
    fs.writeFileSync(path.join(packageDir, EVIDENCE, 'before', 'leftovers.log'), git(sandbox, ['status', '--porcelain']), 'utf8');
    git(sandbox, ['reset', '--quiet', '--hard']);
    git(sandbox, ['clean', '--quiet', '-d', '--force']);
    git(sandbox, ['checkout', '--quiet', '-b', branch]);
    const patches = manifest.patches.map((p) => path.join(packageDir, PATCHES, p));
    const identity = ['-c', `user.name=${manifest.author.name}`, '-c', `user.email=${manifest.author.email}`];
    fs.mkdirSync(path.join(packageDir, EVIDENCE, 'after'), { recursive: true });
    let applied = '';
    let appliedOk = true;
    try {
      applied = git(sandbox, [...identity, 'am', '--quiet', ...patches]);
    } catch (error) {
      applied = (error as Error).message;
      appliedOk = false;
    }
    fs.writeFileSync(path.join(packageDir, EVIDENCE, 'after', 'apply.log'), applied, 'utf8');
    if (!appliedOk) {
      results.push({ phase: 'after', name: 'apply', ok: false, seconds: 0, problem: 'the patch series does not apply to the base', log: path.join(packageDir, EVIDENCE, 'after', 'apply.log') });
      ok = false;
    } else {
      ok = runPhase('after', input.config.verify, input.config, evidence('after'), results);
    }
  }
  const result: SandboxResult = {
    feature: manifest.feature, game: normalizePath(game), sandbox: normalizePath(sandbox), base: manifest.game.base, branch,
    steps: results, ok, gameChanged: fingerprint(game) !== before,
  };
  fs.writeFileSync(path.join(packageDir, EVIDENCE, 'sandbox.json'), JSON.stringify(result, null, 2) + '\n', 'utf8');
  return result;
}

/** The manual integration, printed, never run: the reviewed branch is fetched from the sandbox into the game. */
export function integrationCommands(result: SandboxResult): string[] {
  const gameRoot = result.game;
  const worktree = path.join(path.dirname(gameRoot), `wt-${result.branch.replace('/', '-')}`).replace(/\\/g, '/');
  return [
    `git -C "${result.sandbox}" log --oneline ${result.base}..${result.branch}`,
    `git -C "${result.sandbox}" diff --stat ${result.base}..${result.branch}`,
    `git -C "${gameRoot}" fetch "${result.sandbox}" ${result.branch}:${result.branch}`,
    `git -C "${gameRoot}" worktree add "${worktree}" ${result.branch}`,
    '# then the usual integration: review, merge into master, verificar_integracao.py, photos analysed, push',
  ];
}
