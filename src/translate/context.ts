// What every part of a translation shares: the source program, the solved flow of values (the one owner of what each
// value is in C++), the type mapper that writes those facts as C++ types, the plan of the units (functions with their
// signatures, constants, indexes) and the names in scope.
import type * as TS from 'typescript';
import type { TypeScriptApi } from '../graph/typescript.ts';
import type { CppType } from './cpptype.ts';
import type { FlowGraph } from './flow.ts';
import type { Patterns } from './patterns.ts';
import type { TranslationSpec } from './spec.ts';
import type { TypeMapper } from './types.ts';

export interface Param {
  readonly name: string;
  readonly type: CppType;
  /** Taken by value even though it is an object (an Omit<T> the function fills in). */
  readonly byValue: boolean;
}

/** A source function and its C++ form. */
export interface FunctionPlan {
  readonly source: string;
  readonly cpp: string;
  readonly decl: TS.SignatureDeclaration & { readonly body?: TS.ConciseBody | undefined };
  readonly params: readonly Param[];
  readonly ret: CppType;
  /** An edit takes the feature's state first and changes it in place; `result` is what it returns besides. */
  readonly edit: { readonly result: CppType | null; readonly resultField: string | null } | null;
  readonly exported: boolean;
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
  readonly line: number;
  readonly doc: string | null;
}

export interface ConstantPlan {
  readonly source: string;
  readonly cpp: string;
  readonly type: CppType;
  readonly value: string;
  readonly line: number;
  readonly doc: string | null;
  readonly constexpr: boolean;
}

export interface Local {
  readonly cpp: string;
  readonly type: CppType;
  /** A Set: the hash set that keeps each member once (the vector keeps their order). */
  readonly set?: string;
  /** A companion the spec gives a class (a heap's arrival counter). */
  readonly companion?: string;
}

export interface TranslateContext {
  readonly ts: TypeScriptApi;
  readonly checker: TS.TypeChecker;
  readonly spec: TranslationSpec;
  readonly file: TS.SourceFile;
  readonly flow: FlowGraph;
  readonly types: TypeMapper;
  readonly patterns: Patterns;
  readonly errors: string[];
  readonly includes: Set<string>;
  readonly functions: Map<TS.Symbol, FunctionPlan>;
  readonly memos: Map<TS.Symbol, MemoPlan>;
  readonly constants: Map<TS.Symbol, ConstantPlan>;
  /** Literals the spec names, by value: their constants are written in the feature header. */
  readonly namedLiterals: Map<number, ConstantPlan>;
  /** Numbers the target allows bare (its gate rules); any other literal needs a name. */
  readonly allowedNumbers: readonly number[];
}
