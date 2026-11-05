// Source concepts the target already owns, matched by shape (spec.patterns): `($x + 32768) * 65536 + ($y + 32768)` is
// core/grid.h cell_key($x, $y). One matcher for the analysis (what type the match has) and the emitter (what it writes).
import type * as TS from 'typescript';
import type { TypeScriptApi } from '../graph/typescript.ts';
import type { PatternMapping } from './spec.ts';

export interface Pattern {
  readonly expr: TS.Expression;
  readonly target: string;
  readonly include: string;
  readonly type: 'int' | 'int64' | 'float' | 'bool';
  readonly context: { readonly type: string; readonly name: string } | null;
}

export interface PatternMatch {
  readonly pattern: Pattern;
  readonly bound: ReadonlyMap<string, TS.Expression>;
}

export class Patterns {
  readonly #ts: TypeScriptApi;
  readonly #patterns: readonly Pattern[];

  constructor(ts: TypeScriptApi, mappings: readonly PatternMapping[]) {
    this.#ts = ts;
    this.#patterns = mappings.map((p) => {
      const parsed = ts.createSourceFile('pattern.ts', `(${p.source});`, ts.ScriptTarget.Latest, true);
      return { expr: (parsed.statements[0] as TS.ExpressionStatement).expression, target: p.target, include: p.include, type: p.type ?? 'int64', context: p.context ?? null };
    });
  }

  match(e: TS.Expression): PatternMatch | null {
    for (const pattern of this.#patterns) {
      const bound = new Map<string, TS.Expression>();
      if (this.#match(pattern.expr, e, bound)) return { pattern, bound };
    }
    return null;
  }

  #match(p: TS.Node, n: TS.Node, bound: Map<string, TS.Expression>): boolean {
    const ts = this.#ts;
    while (ts.isParenthesizedExpression(p)) p = p.expression;
    while (ts.isParenthesizedExpression(n)) n = n.expression;
    if (ts.isIdentifier(p) && p.text.startsWith('$')) {
      const prior = bound.get(p.text);
      if (prior) return prior.getText() === n.getText();
      bound.set(p.text, n as TS.Expression);
      return true;
    }
    if (p.kind !== n.kind) return false;
    if (ts.isIdentifier(p) || ts.isNumericLiteral(p)) return p.text === (n as TS.Identifier).text;
    if (ts.isBinaryExpression(p)) {
      const b = n as TS.BinaryExpression;
      return p.operatorToken.kind === b.operatorToken.kind && this.#match(p.left, b.left, bound) && this.#match(p.right, b.right, bound);
    }
    if (ts.isPropertyAccessExpression(p)) {
      const b = n as TS.PropertyAccessExpression;
      return p.name.text === b.name.text && this.#match(p.expression, b.expression, bound);
    }
    if (ts.isCallExpression(p)) {
      const b = n as TS.CallExpression;
      return p.arguments.length === b.arguments.length && this.#match(p.expression, b.expression, bound)
        && p.arguments.every((a, i) => this.#match(a, b.arguments[i]!, bound));
    }
    return false;
  }
}
