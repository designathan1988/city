// Splits the source into the units the converter reasons about: each class member, each top-level function or
// statement, each interface, and a class "head" (name, heritage, fields). Code nested inside a unit belongs to it.
import type * as TS from 'typescript';
import type { UnitKind } from './ir.ts';
import type { TypeScriptApi } from './typescript.ts';

export interface UnitRef {
  readonly node: TS.Node;
  /** True for a class head: the class itself minus its methods, which are units of their own. */
  readonly head: boolean;
}

function isFunctionBody(ts: TypeScriptApi, node: TS.Node): boolean {
  const owner = node.parent;
  return ts.isBlock(node) && owner !== undefined && ts.isFunctionLike(owner) &&
    (owner as TS.FunctionLikeDeclaration).body === node;
}

function isFunctionValue(ts: TypeScriptApi, node: TS.Node | undefined): boolean {
  return node !== undefined && (ts.isArrowFunction(node) || ts.isFunctionExpression(node));
}

/**
 * A named function declared directly in another function's body, or a method of the object a function returns:
 * the factory pattern (`function createX() { function update() {} return { update } }`). Each is a unit of its own,
 * so using one inner function does not pull the whole factory; what it captures is an edge to the factory.
 */
export function isNestedUnitNode(ts: TypeScriptApi, node: TS.Node): boolean {
  const parent = node.parent;
  if (!parent) return false;
  if (ts.isFunctionDeclaration(node) && node.name !== undefined && isFunctionBody(ts, parent)) return true;
  if (ts.isVariableStatement(node) && isFunctionBody(ts, parent)) {
    return node.declarationList.declarations.every((d) => isFunctionValue(ts, d.initializer));
  }
  const returned = ts.isObjectLiteralExpression(parent) && parent.parent !== undefined && ts.isReturnStatement(parent.parent) &&
    parent.parent.parent !== undefined && isFunctionBody(ts, parent.parent.parent);
  return returned && (ts.isMethodDeclaration(node) || (ts.isPropertyAssignment(node) && isFunctionValue(ts, node.initializer)));
}

/** True when `node` is where a unit begins (a nested unit or a class member), as opposed to code inside one. */
export function startsUnit(ts: TypeScriptApi, node: TS.Node): boolean {
  return isNestedUnitNode(ts, node) || (ts.isClassElement(node) && node.parent !== undefined && ts.isClassLike(node.parent));
}

export function unitOf(ts: TypeScriptApi, node: TS.Node): UnitRef | null {
  for (let p: TS.Node | undefined = node; p && !ts.isSourceFile(p); p = p.parent) {
    const parent: TS.Node | undefined = p.parent;
    if (!parent) return null;
    if (isNestedUnitNode(ts, p)) return { node: p, head: false };
    if (ts.isClassElement(p) && ts.isClassLike(parent)) return { node: p, head: false };
    if (ts.isFunctionDeclaration(p) && (ts.isSourceFile(parent) || ts.isModuleBlock(parent))) return { node: p, head: false };
    if (ts.isTypeElement(p) && ts.isInterfaceDeclaration(parent)) return { node: parent, head: false };
    if (ts.isSourceFile(parent) || ts.isModuleBlock(parent)) {
      return ts.isClassDeclaration(p) ? { node: p, head: true } : { node: p, head: false };
    }
  }
  return null;
}

/** Identity of a unit: the node's file and start, plus whether it is a class head. */
export function unitKey(ref: UnitRef): string {
  const sf = ref.node.getSourceFile();
  return `${ref.head ? 'H' : 'N'}${sf.fileName}@${ref.node.getStart(sf)}`;
}

/** Units declared directly by a top-level statement: a class gives its head and every member. */
export function statementUnits(ts: TypeScriptApi, statement: TS.Statement): UnitRef[] {
  if (ts.isImportDeclaration(statement) || ts.isExportDeclaration(statement) || ts.isImportEqualsDeclaration(statement)) return [];
  if (ts.isClassDeclaration(statement)) {
    return [{ node: statement, head: true }, ...statement.members.map((member) => ({ node: member, head: false }))];
  }
  if (ts.isModuleDeclaration(statement) && statement.body && ts.isModuleBlock(statement.body)) {
    return statement.body.statements.flatMap((inner) => statementUnits(ts, inner));
  }
  return [{ node: statement, head: false }];
}

export function kindOf(ts: TypeScriptApi, ref: UnitRef): UnitKind {
  const n = ref.node;
  if (ref.head || ts.isClassDeclaration(n)) return 'class';
  if (ts.isMethodDeclaration(n) || ts.isPropertyAssignment(n)) return 'method';
  if (ts.isConstructorDeclaration(n)) return 'constructor';
  if (ts.isGetAccessorDeclaration(n)) return 'getter';
  if (ts.isSetAccessorDeclaration(n)) return 'setter';
  if (ts.isPropertyDeclaration(n) || ts.isIndexSignatureDeclaration(n)) return 'field';
  if (ts.isClassStaticBlockDeclaration(n)) return 'staticBlock';
  if (ts.isFunctionDeclaration(n)) return 'function';
  if (ts.isVariableStatement(n)) {
    const allFunctions = n.declarationList.declarations.every(
      (d) => d.initializer !== undefined && (ts.isArrowFunction(d.initializer) || ts.isFunctionExpression(d.initializer)),
    );
    return allFunctions ? 'function' : 'variable';
  }
  if (ts.isInterfaceDeclaration(n)) return 'interface';
  if (ts.isTypeAliasDeclaration(n)) return 'type';
  if (ts.isEnumDeclaration(n)) return 'enum';
  if (ts.isModuleDeclaration(n)) return 'namespace';
  return 'statement';
}

function className(ts: TypeScriptApi, cls: TS.ClassLikeDeclaration): string {
  if (cls.name) return cls.name.text;
  const owner = cls.parent;
  if (owner && ts.isVariableDeclaration(owner) && ts.isIdentifier(owner.name)) return owner.name.text;
  return '<anonymous class>';
}

function memberName(ts: TypeScriptApi, member: TS.ClassElement): string {
  if (ts.isConstructorDeclaration(member)) return 'constructor';
  if (ts.isClassStaticBlockDeclaration(member)) return '<static>';
  if (ts.isIndexSignatureDeclaration(member)) return '<index>';
  const name = member.name;
  if (!name) return '<member>';
  if (ts.isIdentifier(name) || ts.isPrivateIdentifier(name) || ts.isStringLiteral(name) || ts.isNumericLiteral(name)) return name.text;
  return name.getText();
}

export function nameOf(ts: TypeScriptApi, ref: UnitRef): string {
  const own = ownName(ts, ref);
  if (!ref.head && isNestedUnitNode(ts, ref.node) && ref.node.parent) {
    const enclosing = unitOf(ts, ref.node.parent);
    if (enclosing) return `${nameOf(ts, enclosing)}.${own}`;
  }
  return own;
}

function ownName(ts: TypeScriptApi, ref: UnitRef): string {
  const n = ref.node;
  if ((ts.isMethodDeclaration(n) || ts.isPropertyAssignment(n)) && n.parent && ts.isObjectLiteralExpression(n.parent)) {
    return ts.isIdentifier(n.name) || ts.isStringLiteral(n.name) ? n.name.text : n.name.getText();
  }
  if (ref.head && ts.isClassLike(n)) return className(ts, n);
  if (ts.isClassElement(n) && n.parent && ts.isClassLike(n.parent)) return `${className(ts, n.parent)}.${memberName(ts, n)}`;
  if (ts.isVariableStatement(n)) return n.declarationList.declarations.map((d) => d.name.getText()).join(',');
  if (
    (ts.isFunctionDeclaration(n) || ts.isInterfaceDeclaration(n) || ts.isTypeAliasDeclaration(n) || ts.isEnumDeclaration(n) ||
      ts.isClassDeclaration(n)) && n.name
  ) {
    return n.name.text;
  }
  if (ts.isModuleDeclaration(n)) return n.name.text;
  return `<${ts.SyntaxKind[n.kind]}>`;
}

/**
 * The unit another one cannot exist without: a member's class head (the class's fields), a nested unit's enclosing
 * function (which creates it and holds what it captures).
 */
export function ownerOf(ts: TypeScriptApi, ref: UnitRef): UnitRef | null {
  const n = ref.node;
  if (ref.head) return null;
  if (isNestedUnitNode(ts, n)) return n.parent ? unitOf(ts, n.parent) : null;
  if (!ts.isClassElement(n) || !n.parent || !ts.isClassLike(n.parent)) return null;
  const cls = n.parent;
  if (ts.isClassDeclaration(cls) && cls.parent && (ts.isSourceFile(cls.parent) || ts.isModuleBlock(cls.parent))) return { node: cls, head: true };
  // A class written inside code (`function watch() { class Watched extends Worker {…} }`, `const A = class {…}`) is
  // not a unit: its members belong to the unit around it, which is what makes and hands out the class.
  return cls.parent ? unitOf(ts, cls.parent) : null;
}
