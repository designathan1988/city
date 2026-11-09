// The sandbox configuration: how the game is set up and verified, written as the game's own official commands so the
// verdict is the game's. `${sandbox}`, `${evidence}`, `${game}` and `${machine:key}` (a key of the machine file,
// src/common/machine.ts) are expanded; nothing machine-specific lives in the converter's code.
import fs from 'node:fs';
import { machine } from '../common/machine.ts';

/** What a step's run must show to pass. Text checks read the step's output, or `file` when given. */
export interface Expectation {
  readonly exitCode?: number;
  /** A file the step writes itself (expanded), read instead of its output. */
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
  /** argv (expanded): a process, run as a task of the repository's orchestration owner. */
  readonly run: readonly string[];
  /** Run where no window reaches the user's desktop. */
  readonly hidden?: boolean;
  /** Its deadline; past it the step's whole process tree is stopped. */
  readonly timeoutSeconds: number;
  /** Steps of the same phase it needs (their products); a step needs none unless it says so, and runs beside the others. */
  readonly after?: readonly string[];
  /** What it makes that later steps read (expanded): restored when its result comes from the cache. */
  readonly outputs?: readonly string[];
  readonly expect?: Expectation;
}

export interface SandboxConfig {
  /** Paths outside version control that the build needs, copied read-only from the game. */
  readonly copy: readonly string[];
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
  for (const [phase, steps] of [['setup', config.setup], ['verify', config.verify]] as const) {
    const names = new Set(steps.map((s) => s.name));
    for (const step of steps) {
      if (!step.name || !Array.isArray(step.run) || step.run.length === 0) throw new Error(`${file}: every step needs a name and a run list`);
      if (!(typeof step.timeoutSeconds === 'number' && step.timeoutSeconds > 0)) throw new Error(`${file}: step ${step.name} needs its deadline (timeoutSeconds)`);
      for (const need of step.after ?? []) if (!names.has(need)) throw new Error(`${file}: step ${step.name} comes after ${need}, which ${phase} does not have`);
    }
  }
  return config;
}

export interface Variables {
  readonly sandbox: string;
  readonly evidence: string;
  readonly game: string;
}

export function expand(text: string, vars: Variables): string {
  return text.replace(/\$\{(sandbox|evidence|game|machine:(\w+))\}/g, (_match, key: string, machineKey: string | undefined) => {
    if (machineKey !== undefined) return machine().get(machineKey);
    return vars[key as keyof Variables].replace(/\\/g, '/');
  });
}
