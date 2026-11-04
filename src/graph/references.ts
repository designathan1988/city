// Classifies one use of a resolved declaration: called, constructed, state read or written, used as a type,
// inherited, or referenced as a value. Mutation through a method of a held container (push, set, sort...) counts as a
// write of the field or variable that holds the container: that is state the target must own.
import type * as TS from 'typescript';
import type { EdgeKind } from './ir.ts';
import type { TypeScriptApi } from './typescript.ts';

const MUTATING_METHODS = new Set([
  'push', 'pop', 'shift', 'unshift', 'splice', 'sort', 'reverse', 'fill', 'copyWithin',
  'set', 'add', 'delete', 'clear',
]);

/** Skips wrappers that do not change what an expression denotes: (x), x!, x as T, x satisfies T, <T>x. */
function outer(ts: TypeScriptApi, expr: TS.Node): TS.Node {
  let e = expr;
  while (
    e.parent &&
    (ts.isParenthesizedExpression(e.parent) || ts.isNonNullExpression(e.parent) || ts.isAsExpression(e.parent) ||
      ts.isSatisfiesExpression(e.parent) || ts.isTypeAssertionExpression(e.parent))
  ) {
    e = e.parent;
  }
  return e;
}

function isAssignmentTarget(ts: TypeScriptApi, expr: TS.Node): 'plain' | 'compound' | null {
  const p = expr.parent;
  if (!p) return null;
  if (ts.isBinaryExpression(p) && p.left === expr) {
    const op = p.operatorToken.kind;
    if (op === ts.SyntaxKind.EqualsToken) return 'plain';
    if (op >= ts.SyntaxKind.FirstCompoundAssignment && op <= ts.SyntaxKind.LastCompoundAssignment) return 'compound';
  }
  if ((ts.isPrefixUnaryExpression(p) || ts.isPostfixUnaryExpression(p)) &&
    (p.operator === ts.SyntaxKind.PlusPlusToken || p.operator === ts.SyntaxKind.MinusMinusToken)) {
    return 'compound';
  }
  return null;
}

function stateAccess(ts: TypeScriptApi, expr: TS.Node): EdgeKind[] {
  const e = outer(ts, expr);
  const assigned = isAssignmentTarget(ts, e);
  if (assigned === 'plain') return ['write'];
  if (assigned === 'compound') return ['read', 'write'];
  const p = e.parent;
  if (p && ts.isDeleteExpression(p)) return ['write'];
  // field[i] = v, field[i] += v: the container held by the field is mutated.
  if (p && ts.isElementAccessExpression(p) && p.expression === e && isAssignmentTarget(ts, outer(ts, p)) !== null) {
    return ['read', 'write'];
  }
  // field.push(v), field.set(k, v): mutation through the container's own method.
  if (p && ts.isPropertyAccessExpression(p) && p.expression === e && MUTATING_METHODS.has(p.name.text)) {
    const call = outer(ts, p).parent;
    if (call && ts.isCallExpression(call) && call.expression === outer(ts, p)) return ['read', 'write'];
  }
  return ['read'];
}

export function isModuleLevelVariable(ts: TypeScriptApi, decl: TS.Declaration): boolean {
  if (!ts.isVariableDeclaration(decl)) return false;
  const statement = decl.parent?.parent;
  return statement !== undefined && ts.isVariableStatement(statement) && statement.parent !== undefined &&
    (ts.isSourceFile(statement.parent) || ts.isModuleBlock(statement.parent));
}

/** Declarations whose value is state: fields, module-level variables and members of object literals. */
function isStateDeclaration(ts: TypeScriptApi, decl: TS.Declaration): boolean {
  if (ts.isPropertyDeclaration(decl) || ts.isPropertySignature(decl) || ts.isPropertyAssignment(decl) ||
    ts.isShorthandPropertyAssignment(decl)) {
    return true;
  }
  if (ts.isParameter(decl)) return ts.getCombinedModifierFlags(decl) !== ts.ModifierFlags.None; // parameter property
  return isModuleLevelVariable(ts, decl);
}

function heritage(ts: TypeScriptApi, id: TS.Node): EdgeKind | null {
  for (let p = id.parent; p && !ts.isSourceFile(p); p = p.parent) {
    if (ts.isHeritageClause(p)) {
      const owner = p.parent;
      if (p.token === ts.SyntaxKind.ImplementsKeyword) return 'implements';
      return ts.isInterfaceDeclaration(owner) || ts.isClassLike(owner) ? 'inherit' : null;
    }
    if (ts.isBlock(p) || ts.isClassElement(p) || ts.isTypeElement(p)) return null;
  }
  return null;
}

function inTypePosition(ts: TypeScriptApi, id: TS.Node): boolean {
  for (let p = id.parent; p && !ts.isSourceFile(p); p = p.parent) {
    if (ts.isTypeNode(p) && !ts.isExpressionWithTypeArguments(p)) return true;
    if (ts.isBlock(p) || ts.isExpressionStatement(p)) return false;
  }
  return false;
}

/** `captured`: a variable or parameter of an enclosing unit (a factory's closure state) used from a nested unit. */
export function referenceKinds(
  ts: TypeScriptApi,
  id: TS.Identifier | TS.PrivateIdentifier,
  decl: TS.Declaration,
  captured = false,
): EdgeKind[] {
  const inherited = heritage(ts, id);
  if (inherited) return [inherited];
  if (inTypePosition(ts, id)) return ['typeRef'];
  const parent = id.parent;
  const isMemberName = parent !== undefined &&
    ((ts.isPropertyAccessExpression(parent) && parent.name === id) || (ts.isQualifiedName(parent) && parent.right === id));
  const expr: TS.Node = isMemberName ? parent : id;
  const user = outer(ts, expr).parent;
  if (user && ts.isCallExpression(user) && user.expression === outer(ts, expr)) return ['call'];
  if (user && ts.isNewExpression(user) && user.expression === outer(ts, expr)) return ['construct'];
  if (user && ts.isTaggedTemplateExpression(user) && user.tag === outer(ts, expr)) return ['call'];
  if (user && ts.isDecorator(user)) return ['call'];
  // Reading a getter or assigning a setter runs it.
  if (ts.isGetAccessorDeclaration(decl) || ts.isSetAccessorDeclaration(decl)) return ['call'];
  if (captured || isStateDeclaration(ts, decl)) return stateAccess(ts, expr);
  return ['reference'];
}

/** A `let`/`var` at module level: state shared by every caller in the module. */
export function isMutableModuleVariable(ts: TypeScriptApi, statement: TS.Node): boolean {
  if (!ts.isVariableStatement(statement)) return false;
  const flags = statement.declarationList.flags;
  return (flags & ts.NodeFlags.Const) === 0 && (flags & ts.NodeFlags.Using) === 0;
}
