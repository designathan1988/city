// The sandbox configuration: how the game is set up and verified, written as the game's own official commands so the
// verdict is the game's. `${sandbox}`, `${evidence}`, `${game}` and `${env:NAME}` are expanded; nothing machine-
// specific lives in the converter's code.
import fs from 'node:fs';

/** What a step's run must show to pass. Text checks read the step's output, or `file` when given. */
export interface Expectation {
  readonly exitCode?: number;
  readonly file?: string;
  readonly includes?: readonly string[];
  readonly excludes?: readonly string[];
  /** At least `count` occurrences of `text`. */
  readonly atLeast?: { readonly text: string; readonly count: number };
  /** At least `count` files in `dir` (expanded). */
  readonly files?: { readonly dir: string; readonly count: number };
}

export interface Step {
  readonly name: string;
  /** argv; a hidden step's argv is one command line run through the game's hidden launcher. */
  readonly run: readonly string[];
  readonly hidden?: boolean;
  readonly timeoutSeconds?: number;
  readonly expect?: Expectation;
}

export interface SandboxConfig {
  /** Paths outside version control that the build needs, copied read-only from the game. */
  readonly copy: readonly string[];
  /** The game's launcher that runs a command where no window reaches the user (relative to the sandbox). */
  readonly hiddenRunner: string;
  /** Run once, before the first verification. */
  readonly setup: readonly Step[];
  /** Run before the package (the base) and after it. */
  readonly verify: readonly Step[];
}

export function readSandboxConfig(file: string): SandboxConfig {
  const config = JSON.parse(fs.readFileSync(file, 'utf8')) as SandboxConfig;
  for (const key of ['copy', 'setup', 'verify'] as const) {
    if (!Array.isArray(config[key])) throw new Error(`${file}: "${key}" must be a list`);
  }
  if (typeof config.hiddenRunner !== 'string') throw new Error(`${file}: "hiddenRunner" must name the game's hidden launcher`);
  for (const step of [...config.setup, ...config.verify]) {
    if (!step.name || !Array.isArray(step.run) || step.run.length === 0) throw new Error(`${file}: every step needs a name and a run list`);
  }
  return config;
}

export interface Variables {
  readonly sandbox: string;
  readonly evidence: string;
  readonly game: string;
}

export function expand(text: string, vars: Variables): string {
  return text.replace(/\$\{(sandbox|evidence|game|env:([A-Za-z_][A-Za-z0-9_]*))\}/g, (_match, key: string, env: string | undefined) => {
    if (env !== undefined) {
      const value = process.env[env];
      if (value === undefined) throw new Error(`the sandbox configuration needs the environment variable ${env}`);
      return value.replace(/\\/g, '/');
    }
    return vars[key as keyof Variables].replace(/\\/g, '/');
  });
}
