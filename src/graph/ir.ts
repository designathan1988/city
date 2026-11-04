// The intermediate representation every later stage reads: the source's declarations ("units"), the typed edges
// between them, the external APIs they touch and the per-unit facts the classifier and the rule catalog decide on.

export const IR_VERSION = 1;

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
 * dynamic import pulling its whole module.
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
  | 'moduleLoad';

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

export interface NumericLiteral {
  readonly text: string;
  readonly line: number;
  readonly context: 'value' | 'index' | 'enum' | 'default';
  readonly trivial: boolean;
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
  readonly parent: string | null;
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
