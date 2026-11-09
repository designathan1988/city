// The name of a literal the target does not allow bare, from what receives it: the variable it is declared into, the
// field or property it is written to, the parameter it is passed as, the other side of a comparison, or the
// function it is returned from. A bound gets its role: the cap of Math.min or of `x < n` is MAX_, the floor of
// Math.max or of `x > n` is MIN_. One name per value in a module: the same name and value is one constant, a second
// value under a taken name gets the next number.
import type * as TS from 'typescript';
import type { TypeScriptApi } from '../graph/typescript.ts';
import { upper } from './names.ts';

export class LiteralNames {
  readonly #ts: TypeScriptApi;
  readonly #checker: TS.TypeChecker;
  /** module → name → value. */
  readonly #taken = new Map<string, Map<string, number>>();

  constructor(ts: TypeScriptApi, checker: TS.TypeChecker) {
    this.#ts = ts;
    this.#checker = checker;
  }

  /** The constant's name for this literal in this module (the same name for the same value). */
  name(literal: TS.Expression, value: number, module: string): string {
    const base = this.#base(literal);
    const names = this.#taken.get(module) ?? new Map<string, number>();
    this.#taken.set(module, names);
    for (let i = 1; ; i++) {
      const candidate = i === 1 ? base : `${base}_${i}`;
      const known = names.get(candidate);
      if (known === undefined) {
        names.set(candidate, value);
        return candidate;
      }
      if (known === value) return candidate;
    }
  }

  #base(literal: TS.Expression): string {
    const ts = this.#ts;
    let role = '';
    let node: TS.Node = literal;
    while (node.parent) {
      const parent: TS.Node = node.parent;
      if (ts.isCallExpression(parent) && parent.arguments.includes(node as TS.Expression)) {
        const callee = parent.expression;
        if (ts.isPropertyAccessExpression(callee) && ts.isIdentifier(callee.expression) && callee.expression.text === 'Math') {
          if (!role && callee.name.text === 'min') role = 'MAX_';
          if (!role && callee.name.text === 'max') role = 'MIN_';
          node = parent;
          continue;
        }
        const param = this.#checker.getResolvedSignature(parent)?.getDeclaration()?.parameters?.[parent.arguments.indexOf(node as TS.Expression)];
        if (param && ts.isIdentifier(param.name)) return `${role}${upper(param.name.text)}`;
        node = parent;
        continue;
      }
      if (ts.isBinaryExpression(parent)) {
        const k = parent.operatorToken.kind;
        const other = parent.left === node ? parent.right : parent.left;
        const comparing = [ts.SyntaxKind.LessThanToken, ts.SyntaxKind.LessThanEqualsToken, ts.SyntaxKind.GreaterThanToken,
          ts.SyntaxKind.GreaterThanEqualsToken, ts.SyntaxKind.EqualsEqualsEqualsToken, ts.SyntaxKind.ExclamationEqualsEqualsToken].includes(k);
        if (comparing) {
          const literalOnRight = parent.right === node;
          const less = k === ts.SyntaxKind.LessThanToken || k === ts.SyntaxKind.LessThanEqualsToken;
          const greater = k === ts.SyntaxKind.GreaterThanToken || k === ts.SyntaxKind.GreaterThanEqualsToken;
          const bound = (less && literalOnRight) || (greater && !literalOnRight) ? 'MAX_' : (greater && literalOnRight) || (less && !literalOnRight) ? 'MIN_' : '';
          const named = this.#nameOf(other);
          if (named) return `${role || bound}${named}`;
        }
        if (k >= ts.SyntaxKind.FirstAssignment && k <= ts.SyntaxKind.LastAssignment && parent.right === node) {
          const named = this.#nameOf(parent.left);
          if (named) return `${role}${named}`;
        }
        node = parent;
        continue;
      }
      if (ts.isVariableDeclaration(parent) && ts.isIdentifier(parent.name)) return `${role}${upper(parent.name.text)}`;
      if (ts.isPropertyAssignment(parent)) return `${role}${upper(parent.name.getText())}`;
      if (ts.isReturnStatement(parent) || ts.isArrowFunction(parent)) {
        const fn = this.#enclosingName(parent);
        if (fn) return `${role}${fn}`;
      }
      if (ts.isFunctionLike(parent) || ts.isSourceFile(parent)) break;
      node = parent;
    }
    return `${role}${this.#enclosingName(literal) ?? 'CONSTANT'}${role ? '' : '_VALUE'}`;
  }

  /** The name an expression stands for: a variable, the last field of an access, a function's name for a call. */
  #nameOf(e: TS.Node): string | null {
    const ts = this.#ts;
    while (ts.isParenthesizedExpression(e) || ts.isNonNullExpression(e)) e = e.expression;
    if (ts.isIdentifier(e)) return upper(e.text);
    if (ts.isPropertyAccessExpression(e)) return upper(e.name.text);
    if (ts.isElementAccessExpression(e)) return this.#nameOf(e.expression);
    if (ts.isCallExpression(e)) return this.#nameOf(e.expression);
    return null;
  }

  #enclosingName(n: TS.Node): string | null {
    const ts = this.#ts;
    for (let p: TS.Node | undefined = n; p; p = p.parent) {
      if (ts.isFunctionDeclaration(p) && p.name) return upper(p.name.text);
      if ((ts.isArrowFunction(p) || ts.isFunctionExpression(p)) && ts.isVariableDeclaration(p.parent) && ts.isIdentifier(p.parent.name)) return upper(p.parent.name.text);
    }
    return null;
  }
}
