// Syntactic facts of one unit that the audit and the conversion rules decide on: allocations inside loops, numeric
// literals (candidates for named constants or data keys), decisions, loops, awaits, integer bit operations (JS int32
// semantics the C++ must keep), methodology patterns, winnowing fingerprints and the ranges of nested units (code
// that is not this unit's own).
import type * as TS from 'typescript';
import { tokenHash, winnow } from './fingerprint.ts';
import type { AllocationSite, NumericLiteral, PatternSite, Range } from './ir.ts';
import { startsUnit, type UnitRef } from './units.ts';
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
  readonly patterns: PatternSite[];
  readonly fingerprints: [number, number][];
  readonly nested: Range[];
}

/** Array methods whose function argument runs once per element: its body is a loop body. */
const ITERATION_METHODS = new Set([
  'forEach', 'map', 'filter', 'reduce', 'reduceRight', 'some', 'every', 'find', 'findIndex', 'findLast',
  'findLastIndex', 'flatMap', 'sort',
]);
/** Calls that return a fresh array or string. */
const COPYING_METHODS = new Set(['slice', 'map', 'filter', 'concat', 'flat', 'flatMap', 'toSorted', 'toReversed', 'split', 'join']);
/** The naming convention of a named constant: its value is owned there, not a bare number. */
const CONSTANT_NAME = /^[A-Z][A-Z0-9_]*$/;
/** WebGL calls that stall the calling thread until the GPU answers (MDN, WebGL best practices). */
const SYNC_GPU_CALLS = new Set([
  'readPixels', 'getError', 'getBufferSubData', 'checkFramebufferStatus', 'getParameter', 'getProgramParameter',
  'getShaderParameter', 'getProgramInfoLog', 'getShaderInfoLog',
]);
/** Distance computations: inside a nested loop they are an all-pairs proximity search. */
const DISTANCE_CALL = /(^|\.)(hypot|sqrt|distanceTo|distanceToSquared|distanceSq|distance)$/;

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
  if (p && (ts.isVariableDeclaration(p) || ts.isPropertyDeclaration(p)) && p.initializer === n && ts.isIdentifier(p.name) &&
    CONSTANT_NAME.test(p.name.text)) {
    return 'constant';
  }
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

function isBitOperation(ts: TypeScriptApi, n: TS.Node): boolean {
  const k = ts.SyntaxKind;
  if (ts.isPrefixUnaryExpression(n)) return n.operator === k.TildeToken;
  if (!ts.isBinaryExpression(n)) return false;
  const op = n.operatorToken.kind;
  return op === k.BarToken || op === k.AmpersandToken || op === k.CaretToken || op === k.LessThanLessThanToken ||
    op === k.GreaterThanGreaterThanToken || op === k.GreaterThanGreaterThanGreaterThanToken || op === k.BarEqualsToken ||
    op === k.AmpersandEqualsToken || op === k.CaretEqualsToken || op === k.LessThanLessThanEqualsToken ||
    op === k.GreaterThanGreaterThanEqualsToken || op === k.GreaterThanGreaterThanGreaterThanEqualsToken;
}

function calledName(ts: TypeScriptApi, n: TS.CallExpression): string | null {
  return ts.isPropertyAccessExpression(n.expression) ? n.expression.name.text : ts.isIdentifier(n.expression) ? n.expression.text : null;
}

/** The nodes a unit owns: a class head owns its name and heritage only (fields and methods are units). */
function ownedRoots(ts: TypeScriptApi, ref: UnitRef): TS.Node[] {
  if (ref.head && ts.isClassLike(ref.node)) return [...(ref.node.name ? [ref.node.name] : []), ...(ref.node.heritageClauses ?? [])];
  return [ref.node];
}

export function syntaxFacts(ts: TypeScriptApi, ref: UnitRef): SyntaxFacts {
  const sf = ref.node.getSourceFile();
  const root = ref.node;
  const position = (pos: number): { line: number; column: number } => {
    const lc = sf.getLineAndCharacterOfPosition(pos);
    return { line: lc.line + 1, column: lc.character + 1 };
  };
  const line = (n: TS.Node): number => position(n.getStart(sf)).line;
  const facts = {
    loopAllocations: [] as AllocationSite[], numericLiterals: [] as NumericLiteral[], stringLiterals: 0, decisions: 0,
    loops: 0, awaits: 0, bitOperations: 0, nondeterminism: [] as string[], patterns: [] as PatternSite[], nested: [] as Range[],
  };
  const isOwnNested = (n: TS.Node): boolean => n !== root && startsUnit(ts, n);
  const containsDistance = (n: TS.Node): boolean => {
    let found = false;
    const scan = (m: TS.Node): void => {
      if (found || isOwnNested(m)) return;
      if (ts.isCallExpression(m) && DISTANCE_CALL.test(m.expression.getText())) found = true;
      else ts.forEachChild(m, scan);
    };
    ts.forEachChild(n, scan);
    return found;
  };
  const visit = (n: TS.Node, loopDepth: number): void => {
    if (isOwnNested(n)) {
      facts.nested.push({ start: position(n.getStart(sf)), end: position(n.end) });
      return;
    }
    let depth = loopDepth;
    if (isLoop(ts, n)) {
      facts.loops++;
      if (loopDepth > 0 && containsDistance(n)) facts.patterns.push({ id: 'quadratic-proximity', line: line(n), detail: 'distance computed in a nested loop' });
      depth++;
    }
    if (isIterationCallback(ts, n)) { facts.loops++; depth++; }
    if (isDecision(ts, n)) facts.decisions++;
    if (isBitOperation(ts, n)) facts.bitOperations++;
    if (ts.isAwaitExpression(n) || (ts.isForOfStatement(n) && n.awaitModifier)) facts.awaits++;
    if (ts.isStringLiteral(n) || ts.isNoSubstitutionTemplateLiteral(n)) facts.stringLiterals++;
    // Details describe the pattern, never quote the source: the audit is read by people barred from the source.
    if (ts.isDeleteExpression(n)) facts.patterns.push({ id: 'object-delete', line: line(n), detail: 'property removed with delete' });
    if ((ts.isNewExpression(n) || ts.isCallExpression(n)) && n.expression.getText() === 'Array' && n.arguments?.length === 1) {
      facts.patterns.push({ id: 'holey-array', line: line(n), detail: 'array preallocated with Array(n)' });
    }
    if (ts.isNewExpression(n) && n.expression.getText() === 'Date' && (n.arguments?.length ?? 0) === 0) facts.nondeterminism.push('es:new Date()');
    if (ts.isCallExpression(n)) {
      const name = calledName(ts, n);
      if (name === 'shift' && n.arguments.length === 0 && loopDepth > 0) facts.patterns.push({ id: 'queue-shift', line: line(n), detail: 'shift() inside a loop' });
      if (name !== null && SYNC_GPU_CALLS.has(name) && ts.isPropertyAccessExpression(n.expression)) facts.patterns.push({ id: 'sync-gpu-readback', line: line(n), detail: `${name}()` });
      if (name === 'postMessage' && n.arguments.length === 1) facts.patterns.push({ id: 'worker-copy', line: line(n), detail: 'postMessage without a transfer list' });
    }
    if (loopDepth > 0) {
      const what = allocationOf(ts, n);
      if (what) facts.loopAllocations.push({ line: line(n), what });
    }
    if (ts.isNumericLiteral(n)) {
      const negative = n.parent !== undefined && ts.isPrefixUnaryExpression(n.parent) && n.parent.operator === ts.SyntaxKind.MinusToken;
      const text = (negative ? '-' : '') + n.text;
      facts.numericLiterals.push({ text, line: line(n), context: literalContext(ts, negative ? n.parent : n) });
    }
    ts.forEachChild(n, (child) => visit(child, depth));
  };
  const hashes: number[] = [];
  const tokenLines: number[] = [];
  const tokens = (n: TS.Node): void => {
    if (isOwnNested(n) || (n.kind >= ts.SyntaxKind.FirstJSDocNode && n.kind <= ts.SyntaxKind.LastJSDocNode)) return;
    const children = n.getChildren(sf);
    if (children.length === 0) {
      if (n.kind !== ts.SyntaxKind.EndOfFileToken) {
        hashes.push(tokenHash(n.getText(sf)));
        tokenLines.push(line(n));
      }
      return;
    }
    for (const child of children) tokens(child);
  };
  for (const owned of ownedRoots(ts, ref)) {
    visit(owned, 0);
    tokens(owned);
  }
  return { ...facts, fingerprints: winnow(hashes, tokenLines) };
}
