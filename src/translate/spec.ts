// The inputs of a translation: which source declarations become which owner's C++, how the source's types map to the
// target's, which source concepts the target already owns (an expression pattern → the owner's function), and what
// stays with another owner. Everything the translator decides by name or shape is written here, so it is auditable.
import fs from 'node:fs';

/** A source concept the target owns already: matched by shape, written as the owner's call. */
export interface PatternMapping {
  /** A source expression with $placeholders, e.g. "($x + 32768) * 65536 + ($y + 32768)". */
  readonly source: string;
  /** The target expression, the same placeholders, e.g. "cell_key($x, $y)". */
  readonly target: string;
  /** The target header that owns it. */
  readonly include: string;
  readonly reason: string;
}

/** A source class the target expresses with a standard or owned type, method by method. */
export interface ClassMapping {
  /** The C++ type of a variable holding one, `$T` may stand for the element type. */
  readonly type: string;
  readonly include: string;
  /** Extra state a holder needs next to it (`$name` is the variable), e.g. a sequence for stable order. */
  readonly companion?: { readonly suffix: string; readonly type: string; readonly init: string };
  /** method or getter name → template: $self, $seq (the companion), $0, $1 the arguments. */
  readonly members: Readonly<Record<string, string>>;
  /** Members whose value is taken and removed: `const k = q.pop()` becomes two statements. */
  readonly takes?: Readonly<Record<string, { readonly read: string; readonly remove: string }>>;
  /** The C++ type each member returns (what the source's number means there: an item index, a priority). */
  readonly returns: Readonly<Record<string, string>>;
  readonly reason: string;
}

/** A unit left to the owner it belongs to: the package is incomplete until that owner provides it. */
export interface Pending {
  readonly unit: string;
  readonly owner: string;
  readonly reason: string;
}

export interface TranslationSpec {
  readonly feature: string;
  /** The target folder that owns the feature, relative to the target root (native/src/sim/transit). */
  readonly subsystem: string;
  readonly namespace: string;
  /** The source file the feature's declarations are in, relative to the source root. */
  readonly source: string;
  /** One line on what the feature is, the header comment of every file. */
  readonly summary: string;
  /** The feature's state: the source type, the target struct, and its member in the state manager. */
  readonly state: {
    readonly source: string; readonly target: string; readonly member: string; readonly root: string; readonly rootHeader: string;
    /** The source function that makes the empty state: its values are the C++ defaults. */
    readonly empty?: string;
  };
  /** Declarations of the source file to translate, by name (types follow from what they use). */
  readonly units: readonly string[];
  readonly pending: readonly Pending[];
  /** The C++ type of a TypeScript number (the target's convention for world values). */
  readonly number: string;
  /** Names whose numbers are integers (ids, counts, positions): regex on a field, parameter or variable name. */
  readonly integers: { readonly match: string; readonly params: string; readonly type: string };
  /** Source types the target has (Vec2 → V2), with the header that declares them. */
  readonly types: Readonly<Record<string, { readonly type: string; readonly include: string; readonly fields?: Readonly<Record<string, string>> }>>;
  /** Source functions that are unit conversions or identities in the target (m(x) → x: one world unit is a metre). */
  readonly functions: Readonly<Record<string, { readonly template: string; readonly reason: string }>>;
  readonly classes: Readonly<Record<string, ClassMapping>>;
  /** Source functions that memoize a function of the state by identity (identityMemo): each such value is an index. */
  readonly memoizers: readonly string[];
  readonly patterns: readonly PatternMapping[];
  /** Names of anonymous object types, by their shape as TypeScript prints it: { e: number; u: number; ... } -> OnEdge. */
  readonly typeNames: Readonly<Record<string, string>>;
  /** Literals the target does not allow bare, by value: the named constant each becomes, and why it has that value. */
  readonly literals: Readonly<Record<string, { readonly name: string; readonly comment: string }>>;
  /** The gate rules file of the target, relative to its root, and the layer the new subsystem may include. */
  readonly layer: { readonly rules: string; readonly includes: readonly string[] };
}

const REQUIRED: readonly (keyof TranslationSpec)[] = ['feature', 'subsystem', 'namespace', 'source', 'summary', 'state', 'units',
  'pending', 'number', 'integers', 'types', 'functions', 'classes', 'memoizers', 'patterns', 'typeNames', 'literals', 'layer'];

export function readTranslationSpec(file: string): TranslationSpec {
  const spec = JSON.parse(fs.readFileSync(file, 'utf8')) as TranslationSpec;
  for (const key of REQUIRED) if (spec[key] === undefined) throw new Error(`${file}: "${key}" is required`);
  if (!/^native\/src\/[a-z_/]+$/.test(spec.subsystem)) throw new Error(`${file}: "subsystem" must be a folder under native/src`);
  if (spec.units.length === 0) throw new Error(`${file}: "units" must name the declarations to translate`);
  return spec;
}
