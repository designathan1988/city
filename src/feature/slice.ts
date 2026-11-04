// The feature's code inside code it shares with the program, by forward slicing (Weiser): in every declaration the
// program runs that names the feature (its declarations, or a field or variable of the program that holds its values),
// the statements that name it, the statements that use what those give a value to (data dependence, to a fixpoint),
// and what those control (an `if` on the feature's value is the feature's, both branches). The statements around them
// are kept as their first and last lines, so a slice reads as the code it was cut from.
import type * as TS from 'typescript';
import type { UnitRecord } from '../graph/ir.ts';
import type { FeatureBoundary } from './boundary.ts';
import { stringsOf } from './strings.ts';
import type { UnitIndex } from './units.ts';

export interface FeatureSlice {
  /** The program's declaration the slice is cut from. */
  readonly unit: string;
  readonly file: string;
  readonly start: number;
  readonly end: number;
  /** The lines of the declaration that are the feature's, or the headers around them (1-based, ascending). */
  readonly lines: readonly number[];
  /** The strings the slice's code can produce (see strings.ts). */
  readonly strings: readonly string[];
}

export function sliceShared(ts: typeof TS, checker: TS.TypeChecker, index: UnitIndex, b: FeatureBoundary): FeatureSlice[] {
  const feature = new Set([...b.own, ...b.holders]);
  const known = new Map<TS.Symbol, boolean>();
  const isFeature = (symbol: TS.Symbol | undefined): boolean => {
    if (!symbol) return false;
    const cached = known.get(symbol);
    if (cached !== undefined) return cached;
    const resolved = symbol.flags & ts.SymbolFlags.Alias ? checker.getAliasedSymbol(symbol) : symbol;
    const result = (resolved.declarations ?? []).some((d) => {
      const u = index.unitAt(d);
      return u !== null && feature.has(u.id);
    });
    known.set(symbol, result);
    return result;
  };
  const slices: FeatureSlice[] = [];
  for (const id of b.sliced) {
    const unit = index.units.get(id)!;
    const node = index.nodeOf(unit);
    if (!node) throw new Error(`no node for ${id}: the graph and the program disagree`);
    const slice = sliceOf(ts, checker, node, unit, isFeature);
    if (slice) slices.push(slice);
  }
  return slices;
}

function sliceOf(ts: typeof TS, checker: TS.TypeChecker, root: TS.Node, unit: UnitRecord, isFeature: (s: TS.Symbol | undefined) => boolean): FeatureSlice | null {
  const sf = root.getSourceFile();
  const lineOf = (pos: number): number => sf.getLineAndCharacterOfPosition(pos).line + 1;
  // Nested declarations are units of their own (sliced on their own when they name the feature).
  const nested = unit.nested.map((r) => [sf.getPositionOfLineAndCharacter(r.start.line - 1, r.start.column - 1), sf.getPositionOfLineAndCharacter(r.end.line - 1, r.end.column - 1)] as const);
  const inNested = (n: TS.Node): boolean => nested.some(([s, e]) => n.getStart(sf) >= s && n.end <= e);
  const identifiers: TS.Identifier[] = [];
  const collect = (n: TS.Node): void => {
    if (n !== root && inNested(n)) return;
    if (ts.isIdentifier(n)) identifiers.push(n);
    ts.forEachChild(n, collect);
  };
  collect(root);

  /** The statement code belongs to: the nearest node that is an element of a statement list, of a class or of an object. */
  const statementOf = (n: TS.Node): TS.Node => {
    let at = n;
    while (at !== root && at.parent) {
      const p = at.parent;
      if (p === root || ts.isBlock(p) || ts.isCaseClause(p) || ts.isDefaultClause(p) || ts.isSourceFile(p) || ts.isModuleBlock(p)
        || ts.isClassLike(p) || ts.isObjectLiteralExpression(p)) return at;
      at = p;
    }
    return at;
  };
  const symbolOf = (id: TS.Identifier): TS.Symbol | undefined => {
    const p = id.parent;
    if (p && ts.isShorthandPropertyAssignment(p) && p.name === id) return checker.getShorthandAssignmentValueSymbol(p);
    return checker.getSymbolAtLocation(id);
  };
  const isLocal = (s: TS.Symbol): boolean => (s.declarations ?? []).some((d) => d.getSourceFile() === sf && d.getStart(sf) >= root.getStart(sf) && d.end <= root.end);
  /** The locals a statement gives a value to: what it declares (variables, parameters, bindings) or assigns. */
  const defines = (st: TS.Node): TS.Symbol[] => {
    const out: TS.Symbol[] = [];
    const bind = (b: TS.BindingName): void => {
      if (ts.isIdentifier(b)) {
        const s = checker.getSymbolAtLocation(b);
        if (s) out.push(s);
      } else for (const el of b.elements) if (!ts.isOmittedExpression(el)) bind(el.name);
    };
    const visit = (n: TS.Node): void => {
      if (ts.isVariableDeclaration(n) || ts.isParameter(n)) bind(n.name);
      if (ts.isBinaryExpression(n) && n.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && n.operatorToken.kind <= ts.SyntaxKind.LastAssignment && ts.isIdentifier(n.left)) {
        const s = checker.getSymbolAtLocation(n.left);
        if (s) out.push(s);
      }
      ts.forEachChild(n, visit);
    };
    visit(st);
    return out.filter(isLocal);
  };

  const kept = new Set<TS.Node>();
  const tainted = new Set<TS.Symbol>();
  const keep = (st: TS.Node): boolean => {
    if (kept.has(st)) return false;
    kept.add(st);
    for (const s of defines(st)) tainted.add(s);
    return true;
  };
  const symbols = identifiers.map(symbolOf);
  identifiers.forEach((id, i) => { if (isFeature(symbols[i])) keep(statementOf(id)); });
  if (kept.size === 0) return null;
  // Only a read carries a value on: a name being declared, or given a value by `=`, reads nothing.
  const defined = (id: TS.Identifier): boolean => {
    const p = id.parent;
    return ((ts.isVariableDeclaration(p) || ts.isParameter(p) || ts.isBindingElement(p)) && p.name === id)
      || (ts.isBinaryExpression(p) && p.operatorToken.kind === ts.SyntaxKind.EqualsToken && p.left === id);
  };
  for (let changed = true; changed;) {
    changed = false;
    identifiers.forEach((id, i) => {
      const s = symbols[i];
      if (s && tainted.has(s) && !defined(id) && keep(statementOf(id))) changed = true;
    });
  }

  const lines = new Set<number>([lineOf(root.getStart(sf)), lineOf(root.end)]);
  const strings = new Set<string>();
  for (const st of kept) {
    for (let l = lineOf(st.getStart(sf)); l <= lineOf(st.end); l++) lines.add(l);
    for (let p = st.parent; p && p !== root; p = p.parent) {
      lines.add(lineOf(p.getStart(sf)));
      lines.add(lineOf(p.end));
    }
    const visit = (n: TS.Node): void => {
      for (const s of stringsOf(ts, checker, n)) strings.add(s);
      ts.forEachChild(n, visit);
    };
    visit(st);
  }
  return { unit: unit.id, file: unit.file, start: unit.start.line, end: unit.end.line, lines: [...lines].sort((a, c) => a - c), strings: [...strings].sort() };
}
