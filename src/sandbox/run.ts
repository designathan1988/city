// Tests a conversion package without touching the game: an isolated clone of the game at the package's base commit
// gets the files the build needs, runs the game's own setup and verification (the base, "before"), receives the
// package with `git am` on a branch of its own, and runs the verification again ("after"). Every step's output and
// the photos go to the package's evidence folder. The game is only read (cloned, copied from); its state is
// fingerprinted before and after, and a difference, which can only come from another process working on the game
// meanwhile, is recorded so that the result says which base the evidence was taken against.
import { spawnSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';
import { MAX_CHILD_OUTPUT_BYTES } from '../common/files.ts';
import { cloneIsolated, fingerprint, git } from '../common/git.ts';
import { normalizePath } from '../common/paths.ts';
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

/** A step's time limit when its configuration gives none. */
const DEFAULT_STEP_TIMEOUT_S = 900;
const LAUNCHER_GRACE_MS = 60_000;
const MS_PER_SECOND = 1000;

function runStep(step: Step, config: SandboxConfig, vars: Variables, logFile: string): { code: number | null; output: string } {
  const argv = step.run.map((part) => expand(part, vars));
  const seconds = step.timeoutSeconds ?? DEFAULT_STEP_TIMEOUT_S;
  const timeout = seconds * MS_PER_SECOND;
  const command = step.hidden
    ? ['powershell', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', path.join(vars.sandbox, config.hiddenRunner),
      '-TimeoutS', String(seconds), '-WorkDir', vars.sandbox, '-Command', argv.join(' ')]
    : argv;
  // The hidden launcher enforces the step's own timeout; this outer one only stops a launcher that hangs.
  const result = spawnSync(command[0]!, command.slice(1), {
    cwd: vars.sandbox, encoding: 'utf8', timeout: timeout + LAUNCHER_GRACE_MS, maxBuffer: MAX_CHILD_OUTPUT_BYTES,
  });
  const output = `${result.stdout ?? ''}${result.stderr ?? ''}${result.error ? `\n${result.error.message}` : ''}`;
  fs.writeFileSync(logFile, `$ ${command.join(' ')}\n\n${output}`, 'utf8');
  return { code: result.status, output };
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

function runPhase(phase: string, steps: readonly Step[], config: SandboxConfig, vars: Variables, results: StepResult[]): boolean {
  fs.mkdirSync(vars.evidence, { recursive: true });
  for (const step of steps) {
    const started = Date.now();
    const log = path.join(vars.evidence, `${step.name}.log`);
    const { code, output } = runStep(step, config, vars, log);
    const problem = check(step.expect, code, output, vars);
    results.push({ phase, name: step.name, ok: problem === null, seconds: Math.round((Date.now() - started) / MS_PER_SECOND), problem, log });
    if (problem) return false;
  }
  return true;
}

export interface SandboxInput {
  readonly packageDir: string;
  readonly config: SandboxConfig;
  /** Where the sandbox clone goes; must not exist yet and must be outside the game. */
  readonly sandboxDir: string;
}

/** An isolated clone of the game at `base`, with what the build needs copied in (read-only from the game). */
function prepare(game: string, base: string, sandbox: string, config: SandboxConfig): void {
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
  const ok = runPhase('setup', input.config.setup, input.config, evidence('setup'), results) &&
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
  let ok = runPhase('setup', input.config.setup, input.config, evidence('setup'), results) &&
    runPhase('before', input.config.verify, input.config, evidence('before'), results);
  if (ok) {
    git(sandbox, ['checkout', '--quiet', '-b', branch]);
    const patches = manifest.patches.map((p) => path.join(packageDir, PATCHES, p));
    const identity = ['-c', `user.name=${manifest.author.name}`, '-c', `user.email=${manifest.author.email}`];
    const applied = spawnSync('git', ['-C', sandbox, ...identity, 'am', '--quiet', ...patches], { encoding: 'utf8' });
    fs.mkdirSync(path.join(packageDir, EVIDENCE, 'after'), { recursive: true });
    fs.writeFileSync(path.join(packageDir, EVIDENCE, 'after', 'apply.log'), `${applied.stdout}${applied.stderr}`, 'utf8');
    if (applied.status !== 0) {
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
