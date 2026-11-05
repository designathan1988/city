// Whether a field, variable or getter of the program holds the feature's values: what is stored in it (its
// initializer, every assignment to it, what a getter returns) is a value of the feature. A value is the feature's when
// it is one of its declarations or holders, a member of one, what a function of the feature returns, an instance of a
// class of the feature, or made of those (an element of an array or an object, a branch of `?:`, `??`, an operand). What
// a function of the program returns is that function's value, even when it is given the feature's: it stops there.
import type * as TS from 'typescript';
import type { GraphIR, UnitRecord } from '../graph/ir.ts';
import type { UnitIndex } from './units.ts';

export type Holds = (unit: UnitRecord, feature: ReadonlySet<string>) => boolean;

export function holdsFeature(ts: typeof TS, checker: TS.TypeChecker, index: UnitIndex, ir: GraphIR): Holds {
  const writers = new Map<string, string[]>();
  for (const e of ir.edges) if (e.kind === 'write') (writers.get(e.to) ?? writers.set(e.to, []).get(e.to)!).push(e.from);
  const declaredIn = (symbol: TS.Symbol | undefined): UnitRecord[] => {
    if (!symbol) return [];
    const s = symbol.flags & ts.SymbolFlags.Alias ? checker.getAliasedSymbol(symbol) : symbol;
    return (s.declarations ?? []).map((d) => index.unitAt(d)).filter((u): u is UnitRecord => u !== null);
  };
  const symbolOf = (n: TS.Node): TS.Symbol | undefined => {
    const p = n.parent;
    if (ts.isIdentifier(n) && p && ts.isShorthandPropertyAssignment(p) && p.name === n) return checker.getShorthandAssignmentValueSymbol(p);
    return checker.getSymbolAtLocation(n);
  };

  return (unit, feature) => {
    const isFeature = (n: TS.Node): boolean => declaredIn(symbolOf(n)).some((u) => feature.has(u.id));
    /** A type the feature declares: the type itself, a member of a union, or the element of an array or map of it. */
    const ofFeatureType = (t: TS.Type, depth = 0): boolean => {
      if (t.isUnion()) return t.types.some((x) => ofFeatureType(x, depth));
      if (declaredIn(t.aliasSymbol ?? t.getSymbol()).some((u) => feature.has(u.id))) return true;
      return depth === 0 && (t.flags & ts.TypeFlags.Object) !== 0 && checker.getTypeArguments(t as TS.TypeReference).some((a) => ofFeatureType(a, 1));
    };
    const followed = new Set<TS.Node>();
    const carries = (e: TS.Expression): boolean => {
      if (ts.isParenthesizedExpression(e) || ts.isAsExpression(e) || ts.isNonNullExpression(e) || ts.isSatisfiesExpression(e)
        || ts.isTypeAssertionExpression(e) || ts.isAwaitExpression(e)) return carries(e.expression);
      if (ts.isIdentifier(e) || ts.isPropertyAccessExpression(e)) {
        // A value whose type is one the feature declares is its value (what a parameter is given comes from callers).
        if (ofFeatureType(checker.getTypeAtLocation(e))) return true;
      }
      if (ts.isIdentifier(e)) {
        if (isFeature(e)) return true;
        // A local is what it was made from.
        const d = symbolOf(e)?.valueDeclaration;
        if (d && ts.isVariableDeclaration(d) && d.initializer && !followed.has(d)) {
          followed.add(d);
          return carries(d.initializer);
        }
        return false;
      }
      if (ts.isPropertyAccessExpression(e)) return isFeature(e.name) || carries(e.expression);
      if (ts.isElementAccessExpression(e)) return carries(e.expression);
      if (ts.isCallExpression(e)) {
        const callee = ts.isPropertyAccessExpression(e.expression) ? e.expression.name : e.expression;
        if (isFeature(callee)) return true;
        // A method of a platform type (an array's, a map's) on the feature's value gives the feature's value.
        const ofProgram = declaredIn(symbolOf(callee)).length > 0;
        return !ofProgram && ts.isPropertyAccessExpression(e.expression) && carries(e.expression.expression);
      }
      if (ts.isNewExpression(e)) return isFeature(e.expression);
      if (ts.isConditionalExpression(e)) return carries(e.whenTrue) || carries(e.whenFalse);
      if (ts.isBinaryExpression(e)) return carries(e.left) || carries(e.right);
      if (ts.isPrefixUnaryExpression(e) || ts.isPostfixUnaryExpression(e)) return carries(e.operand);
      if (ts.isArrayLiteralExpression(e)) return e.elements.some((x) => carries(ts.isSpreadElement(x) ? x.expression : x));
      if (ts.isObjectLiteralExpression(e)) {
        return e.properties.some((p) => (ts.isPropertyAssignment(p) && carries(p.initializer)) || (ts.isShorthandPropertyAssignment(p) && isFeature(p.name))
          || (ts.isSpreadAssignment(p) && carries(p.expression)));
      }
      if (ts.isTemplateExpression(e)) return e.templateSpans.some((s) => carries(s.expression));
      return false;
    };
    /** An assignment anywhere in the program that stores the feature's value in this unit. */
    const assigned = (): boolean => (writers.get(unit.id) ?? []).some((w) => {
      const node = index.nodeOf(index.units.get(w)!);
      let found = false;
      const visit = (n: TS.Node): void => {
        if (found) return;
        if (ts.isBinaryExpression(n) && n.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && n.operatorToken.kind <= ts.SyntaxKind.LastAssignment) {
          const target = ts.isPropertyAccessExpression(n.left) ? n.left.name : n.left;
          if (declaredIn(symbolOf(target)).some((u) => u.id === unit.id) && carries(n.right)) found = true;
        }
        ts.forEachChild(n, visit);
      };
      if (node) visit(node);
      return found;
    });
    const node = index.nodeOf(unit);
    if (!node) return false;
    if (unit.kind === 'variable' && ts.isVariableStatement(node)) {
      return node.declarationList.declarations.some((d) => d.initializer !== undefined && carries(d.initializer)) || assigned();
    }
    if (unit.kind === 'field' && ts.isPropertyDeclaration(node)) return (node.initializer !== undefined && carries(node.initializer)) || assigned();
    if (unit.kind === 'getter' && ts.isGetAccessorDeclaration(node) && node.body) {
      let found = false;
      const visit = (n: TS.Node): void => {
        if (found || (n !== node && ts.isFunctionLike(n))) return;
        if (ts.isReturnStatement(n) && n.expression && carries(n.expression)) found = true;
        ts.forEachChild(n, visit);
      };
      visit(node.body);
      return found;
    }
    return false;
  };
}
