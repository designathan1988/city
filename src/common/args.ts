// Parses `--name value` flags against a declared spec. Every path the converter touches comes from these flags:
// nothing is hard-coded, because the source and target projects move.

export interface FlagSpec {
  readonly name: string;
  readonly help: string;
  readonly required?: boolean;
  readonly repeatable?: boolean;
}

export class ArgsError extends Error {}

export class Flags {
  readonly #values: Map<string, string[]>;

  constructor(values: Map<string, string[]>) {
    this.#values = values;
  }

  one(name: string): string | undefined {
    return this.#values.get(name)?.[0];
  }

  require(name: string): string {
    const value = this.one(name);
    if (value === undefined) throw new ArgsError(`missing --${name}`);
    return value;
  }

  all(name: string): readonly string[] {
    return this.#values.get(name) ?? [];
  }
}

export function parseFlags(argv: readonly string[], specs: readonly FlagSpec[]): Flags {
  const byName = new Map(specs.map((spec) => [spec.name, spec]));
  const values = new Map<string, string[]>();
  for (let i = 0; i < argv.length; i++) {
    const token = argv[i]!;
    if (!token.startsWith('--')) throw new ArgsError(`unexpected argument: ${token}`);
    const name = token.slice(2);
    const spec = byName.get(name);
    if (!spec) throw new ArgsError(`unknown flag: ${token}`);
    const value = argv[++i];
    if (value === undefined || value.startsWith('--')) throw new ArgsError(`flag ${token} needs a value`);
    const list = values.get(name) ?? [];
    if (list.length > 0 && !spec.repeatable) throw new ArgsError(`flag ${token} given twice`);
    list.push(value);
    values.set(name, list);
  }
  for (const spec of specs) {
    if (spec.required && !values.has(spec.name)) throw new ArgsError(`missing required flag --${spec.name}`);
  }
  return new Flags(values);
}

export function usage(command: string, specs: readonly FlagSpec[]): string {
  const lines = specs.map((spec) => {
    const marks = [spec.required ? 'required' : '', spec.repeatable ? 'repeatable' : ''].filter(Boolean).join(', ');
    return `  --${spec.name}${marks ? ` (${marks})` : ''}  ${spec.help}`;
  });
  return [`usage: converter ${command}`, ...lines].join('\n');
}
