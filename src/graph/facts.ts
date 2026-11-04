// Syntactic facts of one unit that the classifier and the performance rules decide on: allocations inside loops
// (per-tick garbage in the source, forbidden in the target's tick), numeric literals (candidates for named constants
// or data keys), decision and loop counts, awaits, integer bit operations (JS int32 semantics the C++ must keep).
import type * as TS from 'typescript';
import type { AllocationSite, NumericLiteral } from './ir.ts';
import type { UnitRef } from './units.ts';
import type { TypeScriptApi } from './typescript.ts';

export interface SyntaxFacts {
  readonly loopAllocations: AllocationSite[];
  readonly numericLiterals: NumericLiteral[];
  readonly stringLiterals: number;
  readonly decisions: number;
  readonly loops: number;
  readonly awaits: number;
  readonly bitOperations: number;
  readonly nondeterminism: string[];
}

/** Array methods whose function argument runs once per element: its body is a loop body. */
const ITERATION_METHODS = new Set([
  'forEach', 'map', 'filter', 'reduce', 'reduceRight', 'some', 'every', 'find', 'findIndex', 'findLast',
  'findLastIndex', 'flatMap', 'sort',
]);
/** Calls that return a fresh array or string. */
const COPYING_METHODS = new Set(['slice', 'map', 'filter', 'concat', 'flat', 'flatMap', 'toSorted', 'toReversed', 'split', 'join']);
const TRIVIAL_NUMBERS = new Set(['0', '1', '-1', '2', '0.5']);

function isLoop(ts: TypeScriptApi, n: TS.Node): boolean {
  return ts.isForStatement(n) || ts.isForOfStatement(n) || ts.isForInStatement(n) || ts.isWhileStatement(n) ||
    ts.isDoStatement(n);
}

function isIterationCallback(ts: TypeScriptApi, n: TS.Node): boolean {
  const call = n.parent;
  if (!call || !ts.isCallExpression(call) || !call.arguments.includes(n as TS.Expression)) return false;
  return ts.isPropertyAccessExpression(call.expression) && ITERATION_METHODS.has(call.expression.name.text);
}

function allocationOf(ts: TypeScriptApi, n: TS.Node): string | null {
  if (ts.isNewExpression(n)) return `new ${n.expression.getText()}`;
  if (ts.isArrayLiteralExpression(n)) return 'array literal';
  if (ts.isObjectLiteralExpression(n)) return 'object literal';
  if (ts.isArrowFunction(n) || ts.isFunctionExpression(n)) return 'closure';
  if (ts.isSpreadElement(n)) return 'spread';
  if (ts.isTemplateExpression(n)) return 'template string';
  if (ts.isCallExpression(n) && ts.isPropertyAccessExpression(n.expression) && COPYING_METHODS.has(n.expression.name.text)) {
    return `.${n.expression.name.text}()`;
  }
  if (ts.isCallExpression(n) && n.expression.getText() === 'Array.from') return 'Array.from()';
  return null;
}

function literalContext(ts: TypeScriptApi, n: TS.Node): NumericLiteral['context'] {
  const p = n.parent;
  if (p && ts.isElementAccessExpression(p) && p.argumentExpression === n) return 'index';
  if (p && ts.isEnumMember(p)) return 'enum';
  if (p && (ts.isParameter(p) || ts.isBindingElement(p)) && p.initializer === n) return 'default';
  return 'value';
}

function isDecision(ts: TypeScriptApi, n: TS.Node): boolean {
  if (ts.isIfStatement(n) || ts.isConditionalExpression(n) || ts.isCaseClause(n) || ts.isCatchClause(n)) return true;
  if (ts.isBinaryExpression(n)) {
    const op = n.operatorToken.kind;
    return op === ts.SyntaxKind.AmpersandAmpersandToken || op === ts.SyntaxKind.BarBarToken ||
      op === ts.SyntaxKind.QuestionQuestionToken;
  }
  return (ts.isPropertyAccessExpression(n) || ts.isElementAccessExpression(n) || ts.isCallExpression(n)) &&
    n.questionDotToken !== undefined;
}

const BIT_OPERATORS = new Set<number>();

function isBitOperation(ts: TypeScriptApi, n: TS.Node): boolean {
  if (BIT_OPERATORS.size === 0) {
    for (const k of [
      ts.SyntaxKind.BarToken, ts.SyntaxKind.AmpersandToken, ts.SyntaxKind.CaretToken, ts.SyntaxKind.LessThanLessThanToken,
      ts.SyntaxKind.GreaterThanGreaterThanToken, ts.SyntaxKind.GreaterThanGreaterThanGreaterThanToken, ts.SyntaxKind.BarEqualsToken,
      ts.SyntaxKind.AmpersandEqualsToken, ts.SyntaxKind.CaretEqualsToken, ts.SyntaxKind.LessThanLessThanEqualsToken,
      ts.SyntaxKind.GreaterThanGreaterThanEqualsToken, ts.SyntaxKind.GreaterThanGreaterThanGreaterThanEqualsToken,
    ]) BIT_OPERATORS.add(k);
  }
  if (ts.isBinaryExpression(n)) return BIT_OPERATORS.has(n.operatorToken.kind);
  return ts.isPrefixUnaryExpression(n) && n.operator === ts.SyntaxKind.TildeToken;
}

/** The nodes a unit owns: a class head owns its name and heritage only (fields and methods are units). */
function ownedRoots(ts: TypeScriptApi, ref: UnitRef): TS.Node[] {
  if (ref.head && ts.isClassLike(ref.node)) return [...(ref.node.name ? [ref.node.name] : []), ...(ref.node.heritageClauses ?? [])];
  return [ref.node];
}

export function syntaxFacts(ts: TypeScriptApi, ref: UnitRef): SyntaxFacts {
  const sf = ref.node.getSourceFile();
  const line = (n: TS.Node): number => sf.getLineAndCharacterOfPosition(n.getStart(sf)).line + 1;
  const facts = {
    loopAllocations: [] as AllocationSite[], numericLiterals: [] as NumericLiteral[], stringLiterals: 0, decisions: 0,
    loops: 0, awaits: 0, bitOperations: 0, nondeterminism: [] as string[],
  };
  const visit = (n: TS.Node, loopDepth: number): void => {
    let depth = loopDepth;
    if (isLoop(ts, n)) { facts.loops++; depth++; }
    if (isIterationCallback(ts, n)) { facts.loops++; depth++; }
    if (isDecision(ts, n)) facts.decisions++;
    if (isBitOperation(ts, n)) facts.bitOperations++;
    if (ts.isAwaitExpression(n) || (ts.isForOfStatement(n) && n.awaitModifier)) facts.awaits++;
    if (ts.isStringLiteral(n) || ts.isNoSubstitutionTemplateLiteral(n)) facts.stringLiterals++;
    if (ts.isNewExpression(n) && n.expression.getText() === 'Date' && (n.arguments?.length ?? 0) === 0) {
      facts.nondeterminism.push('es:new Date()');
    }
    if (loopDepth > 0) {
      const what = allocationOf(ts, n);
      if (what) facts.loopAllocations.push({ line: line(n), what });
    }
    if (ts.isNumericLiteral(n)) {
      const negative = n.parent && ts.isPrefixUnaryExpression(n.parent) && n.parent.operator === ts.SyntaxKind.MinusToken;
      const text = (negative ? '-' : '') + n.text;
      facts.numericLiterals.push({ text, line: line(n), context: literalContext(ts, negative ? n.parent : n), trivial: TRIVIAL_NUMBERS.has(text) });
    }
    ts.forEachChild(n, (child) => visit(child, depth));
  };
  for (const root of ownedRoots(ts, ref)) visit(root, 0);
  return facts;
}
