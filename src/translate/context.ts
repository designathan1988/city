// What every part of a translation shares: the source program, the solved flow of values (the one owner of what each
// value is in C++), the type mapper that writes those facts as C++ types, the plan of the units (functions with their
// signatures, constants, indexes) and the names in scope.
import type * as TS from 'typescript';
import type { TypeScriptApi } from '../graph/typescript.ts';
import type { CppType } from './cpptype.ts';
import type { FlowGraph } from './flow.ts';
import type { LiteralNames } from './literal-names.ts';
import type { ModulePlacement } from './modules.ts';
import type { Patterns } from './patterns.ts';
import type { TranslationSpec } from './spec.ts';
import type { TypeMapper } from './types.ts';

export interface Param {
  readonly name: string;
  readonly type: CppType;
  /** Taken by value even though it is an object (an Omit<T> the function fills in). */
  readonly byValue: boolean;
  /** The function changes its contents (an element written, a mutating method): taken by mutable reference, so the
   *  caller sees the change, as JavaScript's shared array does. */
  readonly mutable?: boolean;
}

/** A source function and its C++ form. */
export interface MemberOwner {
  readonly struct: string;
  /** The class's name in the source: the unit a refusal of the member belongs to. */
  readonly source: string;
  readonly kind: 'method' | 'getter' | 'constructor';
  readonly isStatic: boolean;
  readonly node: TS.ClassDeclaration;
}

export interface FunctionPlan {
  readonly source: string;
  readonly cpp: string;
  readonly decl: TS.SignatureDeclaration & { readonly body?: TS.ConciseBody | undefined };
  readonly params: readonly Param[];
  /** Its type parameters: a template, written whole where its callers see it. */
  readonly typeParams: readonly string[];
  /** Context parameters it takes after its own (a context pattern used, directly or through a call). */
  readonly context?: readonly { readonly type: string; readonly name: string }[];
  readonly ret: CppType;
  /** An edit takes the feature's state first and changes it in place; `result` is what it returns besides. */
  readonly edit: { readonly result: CppType | null; readonly resultField: string | null } | null;
  readonly exported: boolean;
  /** A member of a class: written in its struct, not as a free function. */
  readonly owner?: MemberOwner;
  /** The source file it comes from, relative to the source root: its module. */
  readonly module: string;
  readonly line: number;
  readonly doc: string | null;
}

/** An index of the state (the source's identityMemo): rebuilt by every edit, read by queries. */
export interface MemoPlan {
  readonly source: string;
  readonly member: string;
  readonly struct: string;
  readonly fn: TS.ArrowFunction | TS.FunctionExpression;
  /** The locals the builder returns, as members of the index. */
  readonly fields: { readonly local: string; readonly name: string; readonly type: CppType }[];
  /** Objects the builder creates and files by reference: struct → the deque member that owns them. */
  readonly stores: Map<string, string>;
  /** The builder returns one container (read as that container), not an object of several. */
  readonly single: boolean;
  /** The source file it is written in (the index itself belongs to the state's owner). */
  readonly writtenIn: string;
  readonly line: number;
  readonly doc: string | null;
}

export interface ConstantPlan {
  readonly source: string;
  readonly cpp: string;
  readonly type: CppType;
  readonly value: string;
  /** The source file it comes from (null: a literal the spec names, kept with the state's owner). */
  readonly module: string | null;
  readonly line: number;
  readonly doc: string | null;
  readonly constexpr: boolean;
  /** Its value calls a function: it is written after the module's functions are declared, in the module's header. */
  readonly calling?: boolean;
  /** The modules its value names. */
  readonly uses?: ReadonlySet<string>;
}

export interface Local {
  readonly cpp: string;
  readonly type: CppType;
  /** A companion the spec gives a class (a heap's arrival counter). */
  readonly companion?: string;
}

export interface TranslateContext {
  readonly ts: TypeScriptApi;
  readonly checker: TS.TypeChecker;
  readonly spec: TranslationSpec;
  /** The file that declares the feature's state and its roots. */
  readonly file: TS.SourceFile;
  /** Every file of the source program (tests left out). */
  readonly sources: readonly TS.SourceFile[];
  /** A source file's path relative to the source root (its module's name). */
  readonly relative: (file: TS.SourceFile) => string;
  /** Where every source file goes in the target. */
  readonly placement: ModulePlacement;
  readonly flow: FlowGraph;
  readonly types: TypeMapper;
  readonly patterns: Patterns;
  readonly errors: string[];
  readonly includes: Set<string>;
  readonly functions: Map<TS.Symbol, FunctionPlan>;
  readonly memos: Map<TS.Symbol, MemoPlan>;
  readonly constants: Map<TS.Symbol, ConstantPlan>;
  /** Module constants whose value is a context pattern (RAMP_GRADE = ROAD_TUNING.grade.ramp): each reference is the
   *  target expression (tn.grade_ramp), read through the context parameter; nothing is written as a constant. */
  readonly contextConstants: Map<TS.Symbol, { readonly code: string; readonly type: CppType; readonly include: string; readonly context: { readonly type: string; readonly name: string } }>;
  /** Names the literals the target does not allow bare. */
  readonly literals: LiteralNames;
  /** The constants those literals became, by module and name (written in their module's types header). */
  readonly literalConstants: Map<string, ConstantPlan>;
  /** Numbers the target allows bare (its gate rules); any other literal needs a name. */
  readonly allowedNumbers: readonly number[];
}
