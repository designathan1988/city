// A workspace: an isolated clone of the game on a branch of its own (`convert/<feature>`), where a feature is ported
// and committed topic by topic, verified with the game's own steps (the sandbox configuration), and packaged from its
// branch (`converter pack`). The clone is `<port>/workspace`; its record and every verification's evidence sit beside
// it in `<port>`, outside the clone, so the game's gates see only the game. The game itself is only read.
import fs from 'node:fs';
import path from 'node:path';
import { fingerprint, git } from '../common/git.ts';
import { normalizePath } from '../common/paths.ts';
import type { SandboxConfig, Step, Variables } from './config.ts';
import { prepare, runPhase, type StepResult } from './run.ts';

export const WORKSPACE_VERSION = 1;
export const WORKSPACE_RECORD = 'workspace.json';
export const WORKSPACE_DIR = 'workspace';
export const WORKSPACE_EVIDENCE = 'evidence';
/** A feature names a branch and a folder: lower case, digits and dashes. */
const FEATURE_NAME = /^[a-z0-9][a-z0-9-]*$/;
const SHORT_SHA = 12;
/** Godot's `override.cfg` (project root, same format as project.godot) overrides any project setting; it stays out of git. */
const OVERRIDE_CFG = 'project/override.cfg';

export interface WorkspaceRecord {
  readonly version: number;
  readonly feature: string;
  /** The game repository the workspace was cloned from (read only). */
  readonly game: string;
  /** The game commit the workspace starts from: the base of the package. */
  readonly base: string;
  readonly branch: string;
  readonly workspace: string;
}

export interface WorkspaceRun {
  readonly steps: StepResult[];
  readonly ok: boolean;
  /** Where this run's logs and photos are. */
  readonly evidence: string;
}

export interface CreateWorkspaceInput {
  readonly gameRoot: string;
  readonly base?: string | undefined;
  readonly portDir: string;
  readonly feature: string;
  readonly config: SandboxConfig;
}

/**
 * Gives the clone its own Godot user folder (`user://`). The engine names that folder after the project, so every clone
 * and the game itself shared one: the player's settings.cfg, the saves and the ~107 MiB Vulkan pipeline cache the engine
 * reads whole into static memory, rewritten by whichever session ran the game last, which moved the others' photos
 * and memory numbers. `application/config/use_custom_user_dir` and `custom_user_dir_name`, set in the clone's own
 * override.cfg (Godot docs: File paths in Godot projects, ProjectSettings), name a folder after the feature.
 */
export function isolateUserData(workspace: string, feature: string): void {
  fs.mkdirSync(path.dirname(path.join(workspace, OVERRIDE_CFG)), { recursive: true });
  fs.writeFileSync(
    path.join(workspace, OVERRIDE_CFG),
    `[application]

config/use_custom_user_dir=true
config/custom_user_dir_name="Roadcraft-${feature}"
`,
    'utf8',
  );
  const info = path.join(workspace, '.git', 'info');
  fs.mkdirSync(info, { recursive: true });
  fs.appendFileSync(path.join(info, 'exclude'), `
/${OVERRIDE_CFG}
`, 'utf8');
}

/** Clones the game (isolated) into `<port>/workspace` on `convert/<feature>` and runs the configuration's setup there. */
export function createWorkspace(input: CreateWorkspaceInput): WorkspaceRun & { readonly record: WorkspaceRecord; readonly gameChanged: boolean } {
  if (!FEATURE_NAME.test(input.feature)) throw new Error(`feature "${input.feature}": use lower case letters, digits and dashes (it names a branch)`);
  const game = normalizePath(input.gameRoot);
  const portDir = path.resolve(input.portDir);
  // The clone goes inside the port folder, and `prepare` refuses a clone inside the game.
  const base = git(game, ['rev-parse', '--verify', `${input.base ?? 'HEAD'}^{commit}`]).trim();
  const workspace = path.join(portDir, WORKSPACE_DIR);
  const before = fingerprint(game);
  prepare(game, base, workspace, input.config);
  const branch = `convert/${input.feature}`;
  git(workspace, ['checkout', '--quiet', '-b', branch]);
  isolateUserData(workspace, input.feature);
  const record: WorkspaceRecord = { version: WORKSPACE_VERSION, feature: input.feature, game, base, branch, workspace: normalizePath(workspace) };
  fs.writeFileSync(path.join(portDir, WORKSPACE_RECORD), JSON.stringify(record, null, 2) + '\n', 'utf8');
  const steps: StepResult[] = [];
  const evidence = path.join(portDir, WORKSPACE_EVIDENCE, 'setup');
  const ok = runPhase('setup', input.config.setup, input.config, { sandbox: workspace, evidence, game }, steps);
  return { record, steps, ok, evidence, gameChanged: fingerprint(game) !== before };
}

export function readWorkspace(portDir: string): WorkspaceRecord {
  const file = path.join(path.resolve(portDir), WORKSPACE_RECORD);
  if (!fs.existsSync(file)) throw new Error(`no ${WORKSPACE_RECORD} in ${portDir}: create the workspace first (converter workspace)`);
  const record = JSON.parse(fs.readFileSync(file, 'utf8')) as WorkspaceRecord;
  if (record.version !== WORKSPACE_VERSION) throw new Error(`${file} is workspace version ${record.version}; this converter reads ${WORKSPACE_VERSION}`);
  if (!fs.existsSync(record.workspace)) throw new Error(`${file} names ${record.workspace}, which is gone`);
  return record;
}

/** The configuration's verify steps named (all, when none is), in the configuration's order. */
function chosen(config: SandboxConfig, names: readonly string[]): readonly Step[] {
  if (names.length === 0) return config.verify;
  const known = new Set(config.verify.map((s) => s.name));
  const unknown = names.filter((n) => !known.has(n));
  if (unknown.length > 0) throw new Error(`no verify step ${unknown.join(', ')}; the configuration has ${[...known].join(', ')}`);
  return config.verify.filter((s) => names.includes(s.name));
}

/**
 * Runs verify steps in the workspace as it is now. The evidence goes to `<port>/evidence/<commit>` (`-dirty` when the
 * tree has changes the commit does not hold), so each run names the code it judged.
 */
export function verifyWorkspace(portDir: string, config: SandboxConfig, names: readonly string[]): WorkspaceRun {
  const record = readWorkspace(portDir);
  const head = git(record.workspace, ['rev-parse', 'HEAD']).trim().slice(0, SHORT_SHA);
  const dirty = git(record.workspace, ['status', '--porcelain']).trim() !== '';
  const evidence = path.join(path.resolve(portDir), WORKSPACE_EVIDENCE, dirty ? `${head}-dirty` : head);
  const vars: Variables = { sandbox: record.workspace, evidence, game: record.game };
  const steps: StepResult[] = [];
  const ok = runPhase('verify', chosen(config, names), config, vars, steps);
  return { steps, ok, evidence };
}
