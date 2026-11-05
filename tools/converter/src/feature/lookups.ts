// Where code looks things up by key, by the flow of values: `X[k]` where X is an object literal of the program (through
// variables, imports, the values of another object, `?:` and `??`), and a call (of whatever function it runs: the call
// graph says) of a function that looks up with its parameter (`t(key)` reads `EN[key]`), or that hands a key made from
// its parameter to one (`plural(key)` calls `t(`${key}.one`)`), to a fixpoint. The keys a lookup is given are the strings its key expression can be (a literal, a
// template or a value made in the program: values.ts). An object's entry is used when a lookup into it is
// given its key: an object nothing looks up is used by no one, whatever its keys spell.
import type * as TS from 'typescript';
import type { StringValues } from './values.ts';

export interface KeyLookup {
  /** The objects looked up. */
  readonly objects: readonly TS.ObjectLiteralExpression[];
  /** The keys, exact, and the prefixes of keys made from a string of any value. */
  readonly exact: readonly string[];
  readonly prefixes: readonly string[];
  /** Where the lookup is written. */
  readonly at: TS.Node;
}

/** How a lookup function makes the key from its parameter: what it writes before and after it. */
type Shape = readonly [before: string, after: string];

interface LookupFunction {
  readonly objects: Set<TS.ObjectLiteralExpression>;
  readonly shapes: Map<string, Shape>;
}

/** Past this many rounds the lookup functions stop growing (a recursion making ever longer keys). */
const MAX_ROUNDS = 16;

export function keyLookups(ts: typeof TS, checker: TS.TypeChecker, files: readonly TS.SourceFile[],
  targetsOf: (call: TS.CallExpression) => readonly TS.SignatureDeclaration[], values: StringValues): KeyLookup[] {
  const strip = (e: TS.Expression): TS.Expression => {
    while (ts.isParenthesizedExpression(e) || ts.isAsExpression(e) || ts.isSatisfiesExpression(e) || ts.isNonNullExpression(e) || ts.isTypeAssertionExpression(e)) e = e.expression;
    return e;
  };
  const resolved = (s: TS.Symbol | undefined): TS.Symbol | undefined => (s && s.flags & ts.SymbolFlags.Alias ? checker.getAliasedSymbol(s) : s);

  /** The object literals an expression's value is. */
  const objectsOf = (expr: TS.Expression, seen = new Set<TS.Node>()): TS.ObjectLiteralExpression[] => {
    const e = strip(expr);
    if (seen.has(e)) return [];
    seen.add(e);
    if (ts.isObjectLiteralExpression(e)) return [e];
    if (ts.isConditionalExpression(e)) return [...objectsOf(e.whenTrue, seen), ...objectsOf(e.whenFalse, seen)];
    if (ts.isBinaryExpression(e) && [ts.SyntaxKind.QuestionQuestionToken, ts.SyntaxKind.BarBarToken].includes(e.operatorToken.kind)) {
      return [...objectsOf(e.left, seen), ...objectsOf(e.right, seen)];
    }
    if (ts.isIdentifier(e)) {
      const decl = resolved(checker.getSymbolAtLocation(e))?.valueDeclaration;
      if (decl && ts.isVariableDeclaration(decl) && decl.initializer) return objectsOf(decl.initializer, seen);
      // A JSON module imported: its one statement is the object.
      if (decl && ts.isSourceFile(decl)) {
        const first = decl.statements[0];
        return first && ts.isExpressionStatement(first) ? objectsOf(first.expression, seen) : [];
      }
      return [];
    }
    if (ts.isPropertyAccessExpression(e) || ts.isElementAccessExpression(e)) {
      const name = ts.isPropertyAccessExpression(e) ? e.name.text : null;
      return objectsOf(e.expression, seen).flatMap((o) => o.properties.flatMap((p) => {
        if (!ts.isPropertyAssignment(p) && !ts.isShorthandPropertyAssignment(p)) return [];
        if (name !== null && keyText(ts, p.name) !== name) return [];
        return objectsOf(ts.isPropertyAssignment(p) ? p.initializer : p.name, seen);
      }));
    }
    return [];
  };

  /** The strings a key expression can be, by the flow of values (values.ts); a key of any value names none in particular. */
  const keysOf = (e: TS.Expression): { exact: readonly string[]; prefixes: readonly string[] } => {
    const v = values.of(e);
    return v.top ? { exact: [], prefixes: [] } : v;
  };

  /** How an argument is made from a parameter of the function it is written in: which parameter, and the shapes. */
  const shapesOf = (arg: TS.Expression, fn: TS.SignatureDeclaration): { index: number; shape: Shape }[] => {
    const e = strip(arg);
    const paramIndex = (n: TS.Expression): number => {
      if (!ts.isIdentifier(n)) return -1;
      const decl = checker.getSymbolAtLocation(n)?.valueDeclaration;
      return decl && ts.isParameter(decl) && decl.parent === fn ? fn.parameters.indexOf(decl) : -1;
    };
    if (paramIndex(e) >= 0) return [{ index: paramIndex(e), shape: ['', ''] }];
    if (ts.isConditionalExpression(e)) return [...shapesOf(e.whenTrue, fn), ...shapesOf(e.whenFalse, fn)];
    if (ts.isTemplateExpression(e)) {
      const at = e.templateSpans.findIndex((s) => paramIndex(s.expression) >= 0);
      if (at < 0 || e.templateSpans.some((s, i) => i !== at && paramIndex(s.expression) >= 0)) return [];
      // The spans other than the parameter must be literal text for the shape to be exact.
      if (e.templateSpans.some((s, i) => i !== at && !ts.isStringLiteralLike(strip(s.expression)))) return [];
      const textOf = (s: TS.TemplateSpan): string => (strip(s.expression) as TS.StringLiteral).text;
      const head = e.head.text + e.templateSpans.slice(0, at).map((s) => textOf(s) + s.literal.text).join('');
      const tail = e.templateSpans[at]!.literal.text + e.templateSpans.slice(at + 1).map((s) => textOf(s) + s.literal.text).join('');
      return [{ index: paramIndex(e.templateSpans[at]!.expression), shape: [head, tail] }];
    }
    return [];
  };

  const enclosing = (n: TS.Node): TS.SignatureDeclaration | null => {
    for (let p = n.parent; p; p = p.parent) if (ts.isFunctionLike(p)) return p;
    return null;
  };
  const accesses: TS.ElementAccessExpression[] = [];
  const calls: TS.CallExpression[] = [];
  for (const sf of files) {
    const visit = (n: TS.Node): void => {
      if (ts.isElementAccessExpression(n)) accesses.push(n);
      if (ts.isCallExpression(n)) calls.push(n);
      ts.forEachChild(n, visit);
    };
    visit(sf);
  }
  const isWrite = (n: TS.Node): boolean => {
    const p = n.parent;
    return ts.isBinaryExpression(p) && p.left === n && p.operatorToken.kind >= ts.SyntaxKind.FirstAssignment && p.operatorToken.kind <= ts.SyntaxKind.LastAssignment;
  };

  // The lookup functions: by function, each parameter that becomes a key, with the objects and the shapes.
  const functions = new Map<TS.SignatureDeclaration, Map<number, LookupFunction>>();
  const add = (fn: TS.SignatureDeclaration, index: number, objects: Iterable<TS.ObjectLiteralExpression>, shape: Shape): boolean => {
    const byParam = functions.get(fn) ?? functions.set(fn, new Map()).get(fn)!;
    const entry = byParam.get(index) ?? byParam.set(index, { objects: new Set(), shapes: new Map() }).get(index)!;
    let changed = false;
    for (const o of objects) if (!entry.objects.has(o)) { entry.objects.add(o); changed = true; }
    const key = `${shape[0]}\u0000${shape[1]}`;
    if (!entry.shapes.has(key)) { entry.shapes.set(key, shape); changed = true; }
    return changed;
  };
  const objectsAt = new Map<TS.ElementAccessExpression, TS.ObjectLiteralExpression[]>();
  for (const a of accesses) {
    if (isWrite(a)) continue;
    const objects = objectsOf(a.expression);
    if (objects.length === 0) continue;
    objectsAt.set(a, objects);
    const fn = enclosing(a);
    if (fn) for (const { index, shape } of shapesOf(a.argumentExpression, fn)) add(fn, index, objects, shape);
  }
  // What each call runs comes from the call graph (callgraph.ts): through interfaces, callbacks, objects' functions.
  const callees = new Map(calls.map((c) => [c, targetsOf(c)] as const));
  /** The lookup functions a call runs, by parameter. */
  const lookupsOf = (c: TS.CallExpression): [number, LookupFunction][] => callees.get(c)!.flatMap((f) => [...(functions.get(f) ?? [])]);
  for (let round = 0, changed = true; changed && round < MAX_ROUNDS; round++) {
    changed = false;
    for (const c of calls) {
      const fn = enclosing(c);
      if (!fn) continue;
      for (const [j, inner] of lookupsOf(c)) {
        const arg = c.arguments[j];
        if (!arg) continue;
        for (const { index, shape } of shapesOf(arg, fn)) {
          for (const s of [...inner.shapes.values()]) changed = add(fn, index, inner.objects, [s[0] + shape[0], shape[1] + s[1]]) || changed;
        }
      }
    }
  }

  // Every lookup given keys: an access with a key of its own, a call of a lookup function with one. A key made from
  // the parameter of the function it is written in is its callers' key: when every call of the function gives that
  // parameter a known value, the lookup is theirs (the calls are lookups, with the key made as the shapes say), not
  // the function's. When nothing in the program calls it, or a call gives a value of any string, the lookup is where
  // it is written, with the values the parameter can hold.
  const callers = new Map<TS.SignatureDeclaration, TS.CallExpression[]>();
  for (const [c, fns] of callees) for (const f of fns) (callers.get(f) ?? callers.set(f, []).get(f)!).push(c);
  const fromParameter = (e: TS.Expression, at: TS.Node): boolean => {
    const fn = enclosing(at);
    const shapes = fn ? shapesOf(e, fn) : [];
    const by = fn ? callers.get(fn) ?? [] : [];
    if (shapes.length === 0 || by.length === 0) return false;
    return shapes.every(({ index }) => by.every((c) => {
      const arg = c.arguments[index];
      return arg !== undefined && !values.of(arg).top;
    }));
  };
  const out: KeyLookup[] = [];
  for (const [a, objects] of objectsAt) {
    if (fromParameter(a.argumentExpression, a)) continue;
    const keys = keysOf(a.argumentExpression);
    if (keys.exact.length + keys.prefixes.length > 0) out.push({ objects, ...keys, at: a });
  }
  for (const c of calls) {
    for (const [j, entry] of lookupsOf(c)) {
      const arg = c.arguments[j];
      if (!arg || fromParameter(arg, c)) continue;
      const keys = keysOf(arg);
      const shapes = [...entry.shapes.values()];
      const exact = keys.exact.flatMap((k) => shapes.map(([b, a]) => b + k + a));
      const prefixes = keys.prefixes.flatMap((p) => shapes.map(([b]) => b + p));
      if (exact.length + prefixes.length > 0) out.push({ objects: [...entry.objects], exact, prefixes, at: c });
    }
  }
  return out;
}

/** An object member's key as written (`a`, `'a.b'`, `1`); null for a computed one. */
export function keyText(ts: typeof TS, name: TS.PropertyName): string | null {
  if (ts.isIdentifier(name) || ts.isStringLiteralLike(name) || ts.isNumericLiteral(name) || ts.isPrivateIdentifier(name)) return name.text;
  return null;
}
