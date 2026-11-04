// The intermediate representation every later stage reads: the source's declarations ("units"), the typed edges
// between them, the external APIs they touch and the per-unit facts the classifier and the rule catalog decide on.

export const IR_VERSION = 4;

export type UnitKind =
  | 'class'
  | 'method'
  | 'constructor'
  | 'getter'
  | 'setter'
  | 'field'
  | 'staticBlock'
  | 'function'
  | 'variable'
  | 'interface'
  | 'type'
  | 'enum'
  | 'namespace'
  | 'statement';

/**
 * call/construct: invoked; read/write: state (fields, module variables, object-literal members) read or mutated;
 * reference: a value used without calling it (a function passed as callback, a constant, an enum); typeRef: used as a
 * type; inherit/implements: heritage; implementation: an interface or abstract member reaching each implementation;
 * callbackAssign: a function-typed field reaching each place that stores a function in it; moduleLoad: a worker or
 * dynamic import pulling its whole module; indirectCall: a call through a parameter or field reaching each function
 * passed into it (`new FrameClock(frame)` … `this.frame(now)`); schedule: a function handed to
 * requestAnimationFrame, setTimeout or setInterval, to run later.
 */
export type EdgeKind =
  | 'call'
  | 'construct'
  | 'read'
  | 'write'
  | 'reference'
  | 'typeRef'
  | 'inherit'
  | 'implements'
  | 'implementation'
  | 'callbackAssign'
  | 'moduleLoad'
  | 'indirectCall'
  | 'schedule';

export interface Position {
  readonly line: number;
  readonly column: number;
}

export interface ExternalUse {
  readonly key: string;
  readonly kind: EdgeKind;
  readonly count: number;
}

export interface AllocationSite {
  readonly line: number;
  readonly what: string;
}

export interface Range {
  readonly start: Position;
  readonly end: Position;
}

/** A methodology pattern found in the code; the audit catalog says why it is wrong and what to use instead. */
export interface PatternSite {
  readonly id: 'queue-shift' | 'object-delete' | 'holey-array' | 'quadratic-proximity' | 'sync-gpu-readback' | 'worker-copy';
  readonly line: number;
  readonly detail: string;
}

/**
 * A number written in the code. `constant`: the value of a named constant (its owner); `value`: anywhere else in an
 * expression. Which values are allowed bare is the target project's rule (rules.json magic_numbers), not the graph's.
 */
export interface NumericLiteral {
  readonly text: string;
  readonly line: number;
  readonly context: 'value' | 'constant' | 'index' | 'enum' | 'default';
}

export interface UnitFacts {
  readonly external: ExternalUse[];
  readonly nondeterminism: string[];
  readonly loopAllocations: AllocationSite[];
  readonly numericLiterals: NumericLiteral[];
  readonly stringLiterals: number;
  readonly decisions: number;
  readonly loops: number;
  readonly awaits: number;
  readonly bitOperations: number;
  readonly isStatic: boolean;
  readonly callable: boolean;
  readonly mutableModuleState: boolean;
  readonly patterns: PatternSite[];
  /** Winnowing fingerprints of the unit's own tokens: [hash, line]. */
  readonly fingerprints: [number, number][];
}

export interface UnitRecord {
  readonly id: string;
  readonly kind: UnitKind;
  readonly name: string;
  readonly file: string;
  readonly start: Position;
  readonly end: Position;
  readonly lines: number;
  readonly exported: boolean;
  readonly signature: string;
  /** The class head of a member, or the enclosing function of a nested unit. */
  readonly parent: string | null;
  /** Ranges inside this unit that belong to nested units (used or not), so its own code can be told apart. */
  readonly nested: Range[];
  readonly facts: UnitFacts;
}

export interface EdgeRecord {
  readonly from: string;
  readonly to: string;
  readonly kind: EdgeKind;
  readonly count: number;
}

export interface ModuleLoad {
  readonly from: string;
  readonly specifier: string;
  readonly file: string | null;
  readonly how: 'worker' | 'workerImport' | 'dynamicImport';
}

export interface ExternalSummary {
  readonly key: string;
  readonly module: string;
  readonly symbol: string;
  readonly uses: number;
  readonly units: number;
}

export interface GraphIR {
  readonly version: number;
  readonly sourceRoot: string;
  readonly tsconfig: string;
  readonly typescriptVersion: string;
  readonly entries: readonly string[];
  readonly include: readonly string[];
  readonly exclude: readonly string[];
  readonly files: { readonly path: string; readonly lines: number }[];
  readonly units: UnitRecord[];
  readonly edges: EdgeRecord[];
  readonly moduleLoads: ModuleLoad[];
  readonly external: ExternalSummary[];
  readonly unresolvedModules: { readonly from: string; readonly specifier: string }[];
  readonly stats: {
    readonly files: number;
    readonly units: number;
    readonly edges: number;
    readonly lines: number;
    readonly milliseconds: number;
  };
}
