// The inputs of a translation: which source declarations become which owner's C++, how the source's types map to the
// target's, which source concepts the target already owns (an expression pattern → the owner's function), and what
// stays with another owner. Everything the translator decides by name or shape is written here, so it is auditable.
import fs from 'node:fs';

/** A source concept the target owns already: matched by shape, written as the owner's call. */
/**
 * A source function the target owns: its call is the template ($0, $1 the arguments). What it returns is the C++ form
 * of the source's return type (a source type the spec maps, `types`: RoadStructureSpec → StructureSpec).
 */
export interface FunctionMapping {
  readonly template: string;
  readonly reason: string;
  /** The target header that declares it (absent: a template that is only arithmetic, m(x) → x). */
  readonly include?: string;
  /** A parameter the target function reads (the tuning): every caller takes it, as a context pattern's callers do. */
  readonly context?: { readonly type: string; readonly name: string };
}

export interface PatternMapping {
  /** A source expression with $placeholders, e.g. "($x + 32768) * 65536 + ($y + 32768)". */
  readonly source: string;
  /** The target expression, the same placeholders, e.g. "cell_key($x, $y)". */
  readonly target: string;
  /** The target header that owns it. */
  readonly include: string;
  /** What the target expression is (default int64: the target's cell keys). A tuning value is float: an outside
   *  constant (ROAD_TUNING.grade.ramp) mapped to the game's own (its tuning accessor). */
  readonly type?: 'int' | 'int64' | 'float' | 'bool' | 'void' | 'owned';
  /** A parameter the target expression reads (the game passes its tuning: "const Tuning &" tn): every function that
   *  uses the pattern, directly, through a module constant or through a call, takes it, and passes it on. */
  readonly context?: { readonly type: string; readonly name: string };
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
    readonly source: string; readonly target: string; readonly member: string; readonly root: string;
    /** The source function that makes the empty state: its values are the C++ defaults. */
    readonly empty?: string;
    /** The line of the state manager the member goes after, and the member's line (with its comment). */
    readonly anchor?: string;
    readonly memberLine?: string;
  };
  /** Where the target registers a new subsystem: its instructions file with the map, and the map's line for it. */
  readonly registry?: { readonly doc: string; readonly mapLine: string };
  /** Declarations of the source file to translate, by name (types follow from what they use). */
  readonly units: readonly string[];
  /**
   * Declarations of other modules this translation also roots (`src/core/polyline.ts#Frame`): what earlier accepted
   * translations wrote into a module this one shares, so the shared module's files are the same union for both.
   */
  readonly shared?: readonly string[];
  readonly pending: readonly Pending[];
  /** The C++ type of a TypeScript number (the target's convention for world values). */
  readonly number: string;
  /** Source types the target has (Vec2 → V2), with the header that declares them. */
  /** The source types the target already has, found by shape when a translation runs (target-types.ts). */
  readonly types: Record<string, { readonly type: string; readonly include: string; readonly fields?: Readonly<Record<string, string>> }>;
  /** Source functions that are unit conversions or identities in the target (m(x) → x: one world unit is a metre). */
  readonly functions: Readonly<Record<string, FunctionMapping>>;
  readonly classes: Readonly<Record<string, ClassMapping>>;
  /** Source functions that memoize a function of the state by identity (identityMemo): each such value is an index. */
  readonly memoizers: readonly string[];
  readonly patterns: readonly PatternMapping[];
  /** Names of anonymous object types, by their shape as TypeScript prints it: { e: number; u: number; ... } -> OnEdge. */
  readonly typeNames: Readonly<Record<string, string>>;
  /** The gate rules file of the target, relative to its root, and the layer the new subsystem may include. */
  readonly layer: { readonly rules: string; readonly includes: readonly string[] };
}

const REQUIRED: readonly (keyof TranslationSpec)[] = ['feature', 'subsystem', 'namespace', 'source', 'summary', 'state', 'units',
  'pending', 'number', 'functions', 'classes', 'memoizers', 'patterns', 'typeNames', 'layer'];

/**
 * The spec as one translation uses it: `$real` in a class mapping (a heap's priority) is the translation's number
 * type, so a mapping never fixes a precision the translation does not have.
 */
export function resolveSpec(spec: TranslationSpec): TranslationSpec {
  const real = (text: string): string => text.replace(/\$real/g, spec.number);
  const classes = dictionary(Object.entries(spec.classes).map(([name, k]) => [name, {
    ...k, type: real(k.type), members: dictionary(Object.entries(k.members)), returns: dictionary(Object.entries(k.returns).map(([m, t]) => [m, real(t)])),
  }]));
  // The target's types are found by shape after the flow is solved (target-types.ts); this dictionary receives them.
  return { ...spec, classes, types: dictionary(Object.entries(spec.types ?? {})), functions: dictionary(Object.entries(spec.functions)),
    typeNames: dictionary(Object.entries(spec.typeNames)) };
}

/**
 * A dictionary looked up by names of the source program: without a prototype, so `constructor`, `toString` or `call`
 * are absent keys, not Object's inherited members.
 */
function dictionary<V>(entries: readonly (readonly [string, V])[]): Record<string, V> {
  const out = Object.create(null) as Record<string, V>;
  for (const [k, v] of entries) out[k] = v;
  return out;
}

export function readTranslationSpec(file: string): TranslationSpec {
  const spec = JSON.parse(fs.readFileSync(file, 'utf8')) as TranslationSpec;
  for (const key of REQUIRED) if (spec[key] === undefined) throw new Error(`${file}: "${key}" is required`);
  if (!/^native\/src\/[a-z_/]+$/.test(spec.subsystem)) throw new Error(`${file}: "subsystem" must be a folder under native/src`);
  if (spec.units.length === 0) throw new Error(`${file}: "units" must name the declarations to translate`);
  return spec;
}
