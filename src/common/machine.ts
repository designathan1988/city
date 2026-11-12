// The one reader of configs/roadcraft-machine.json: where things are on this machine, every `${name}` expanded (another
// key, or one of the roots this file knows: `converter` is the converter's folder, `repo` the game repository holding
// it). An environment variable only overrides a key it names (MACHINE_KEY_ENV); nothing else in the converter reads
// paths from the environment. Every command, script and test resolves its tools here.
import fs from 'node:fs';
import path from 'node:path';

export const CONVERTER_ROOT = path.resolve(import.meta.dirname, '..', '..');
export const MACHINE_FILE = path.join(CONVERTER_ROOT, 'configs', 'roadcraft-machine.json');

/** The roots the expansion knows without the file: the converter's folder and the game repository that holds it. */
const BUILT_IN: Readonly<Record<string, string>> = {
  converter: CONVERTER_ROOT,
  repo: path.resolve(CONVERTER_ROOT, '..', '..'),
};

/** The environment variable that overrides each key (the names the tests and scripts were given before this file). */
export const MACHINE_KEY_ENV: Readonly<Record<string, string>> = {
  typescript: 'CONVERTER_TYPESCRIPT', jelly: 'CONVERTER_JELLY', postcss: 'CONVERTER_POSTCSS', clangd: 'CONVERTER_CLANGD', cxx: 'CONVERTER_CXX',
  glsl: 'CONVERTER_GLSL',
  godot: 'GODOT', mingw: 'MINGW', bash: 'BASH', python: 'PYTHON',
};

export class MachineError extends Error {}

export interface Machine {
  /** The resolved value of a key; a missing key is an error that names the file. */
  get(key: string): string;
  /** Every key resolved (for `doctor` and for the Python tools, through scripts/machine.ts). */
  all(): Readonly<Record<string, string>>;
  /** A path written with the first root it lies under (`${ports}/roads/workspace`), the inverse of the expansion. */
  contract(file: string, roots: readonly string[]): string;
  /** A text with every `${name}` expanded. */
  expand(text: string): string;
}

const SLASHES = /\\/g;
const forward = (p: string): string => p.replace(SLASHES, '/');

export function readMachine(file: string = MACHINE_FILE, env: NodeJS.ProcessEnv = process.env): Machine {
  if (!fs.existsSync(file)) throw new MachineError(`no machine file at ${file}`);
  const raw = JSON.parse(fs.readFileSync(file, 'utf8')) as Record<string, unknown>;
  const resolved = new Map<string, string>();
  const resolve = (key: string, seen: readonly string[]): string => {
    const known = resolved.get(key);
    if (known !== undefined) return known;
    if (seen.includes(key)) throw new MachineError(`${file}: "${key}" refers to itself (${[...seen, key].join(' -> ')})`);
    const override = MACHINE_KEY_ENV[key] ? env[MACHINE_KEY_ENV[key]] : undefined;
    let value: string;
    if (override !== undefined && override !== '') value = override;
    else if (typeof raw[key] === 'string') value = raw[key] as string;
    else if (BUILT_IN[key] !== undefined) value = BUILT_IN[key];
    else throw new MachineError(`${file} has no "${key}"`);
    const out = forward(value.replace(/\$\{(\w+)\}/g, (_m, other: string) => resolve(other, [...seen, key])));
    resolved.set(key, out);
    return out;
  };
  const keys = [...Object.keys(raw).filter((k) => !k.startsWith('_')), ...Object.keys(BUILT_IN)];
  return {
    get: (key) => resolve(key, []),
    all: () => Object.fromEntries(keys.map((k) => [k, resolve(k, [])])),
    expand: (text) => forward(text.replace(/\$\{(\w+)\}/g, (_m, key: string) => resolve(key, []))),
    contract: (target, roots) => {
      const t = forward(path.resolve(target));
      for (const key of roots) {
        const root = forward(path.resolve(resolve(key, [])));
        if (t.toLowerCase() === root.toLowerCase()) return `\${${key}}`;
        if (t.toLowerCase().startsWith(`${root.toLowerCase()}/`)) return `\${${key}}${t.slice(root.length)}`;
      }
      return t;
    },
  };
}

let shared: Machine | null = null;

/** This machine's file, read once per process. */
export function machine(): Machine {
  shared ??= readMachine();
  return shared;
}
