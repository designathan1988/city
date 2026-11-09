// Where the platform calls the feature: code of the feature that no declaration of the program calls, because a
// package or the platform does (three.js's renderer calls `LOD.update` of every LOD it draws, `onBeforeRender` of
// every object, a listener given to `addEventListener`, a frame given to `requestAnimationFrame`). The graph's edges
// are the program's own uses, so these entries are not among the hooks; they are found here from two facts:
//  - override: a member of a feature's class whose name a class or interface of a package declares in its bases
//    (the package calls its own member, and so the override);
//  - construction: a class whose bases are a package's is built by the package too (three's `clone()` runs
//    `new this.constructor()`): its constructor;
//  - callback: a function of the feature stored in a member a package or shared code declares
//    (`mesh.onBeforeRender = f`), or given to a call of a package, of the platform or of shared code of the program
//    (`addEventListener('x', f)`, `warmLooseAhead(meshes, () => rank())`, directly or as a member of an object
//    literal): whoever holds it may call it later. Not the standard library's synchronous iterations (`Array.map`,
//    `Map.forEach`...), which the caller runs in place.
import type * as TS from 'typescript';
import { containerName, declarationName, externalKey, moduleOf } from '../graph/origin.ts';
import type { FeatureBoundary } from './boundary.ts';
import type { UnitIndex } from './units.ts';

export interface PlatformHook {
  /** The feature's unit the platform calls (or the unit holding the function given, when it is not a unit). */
  readonly unit: string;
  readonly how: 'override' | 'callback';
  /** Who calls it: a package or platform declaration (`three:LOD.update`, `three:Object3D.onBeforeRender`), or shared
   *  code of the program it was given to (`program:<unit>`). */
  readonly by: string;
  /** Where (file:line): the override's declaration, or the place the function is given. */
  readonly at: string;
}

/** Synchronous iteration of the standard library: the callback runs inside the call, the caller's own code. */
const IN_PLACE = /^es:(Readonly)?(Array|Map|Set|String|(Int|Uint|Float|BigInt|BigUint)\d+(Clamped)?Array)(Constructor)?\.(map|forEach|filter|find|findIndex|findLast|findLastIndex|some|every|reduce|reduceRight|flatMap|sort|toSorted|replace|replaceAll|from)$/;

export function platformHooks(ts: typeof TS, checker: TS.TypeChecker, index: UnitIndex, b: FeatureBoundary, root: string): PlatformHook[] {
  const own = new Set(b.own);
  const hooks = new Map<string, PlatformHook>();
  const add = (h: PlatformHook): void => { hooks.set(`${h.unit}|${h.by}|${h.at}`, h); };
  const where = (n: TS.Node): string => {
    const sf = n.getSourceFile();
    return `${index.relative(sf)}:${sf.getLineAndCharacterOfPosition(n.getStart(sf)).line + 1}`;
  };
  const keyOf = (d: TS.Declaration): string | null => {
    const module = moduleOf(d.getSourceFile().fileName, root);
    return module.startsWith('project:') ? null : externalKey(module, containerName(ts, d), declarationName(ts, d, '?'));
  };
  /** The package declaration of `name` in the bases of a class, if any (through the program's own classes). */
  const baseDeclaration = (cls: TS.ClassLikeDeclaration, name: string): string | null => {
    const type = checker.getTypeAtLocation(cls);
    const seen = new Set<TS.Type>();
    const bases = (t: TS.Type): TS.Type[] => (t.isClassOrInterface() ? checker.getBaseTypes(t as TS.InterfaceType) : []);
    const stack = [...bases(type)];
    for (const clause of cls.heritageClauses ?? []) for (const t of clause.types) stack.push(checker.getTypeAtLocation(t));
    while (stack.length > 0) {
      const t = stack.pop()!;
      if (seen.has(t)) continue;
      seen.add(t);
      const p = t.getProperty(name);
      for (const d of p?.declarations ?? []) { const k = keyOf(d); if (k) return k; }
      stack.push(...bases(t));
    }
    return null;
  };
  /** The unit a function value denotes: the declaration a name resolves to, or the unit holding the function written there. */
  const functionUnit = (e: TS.Expression): string | null => {
    let x: TS.Expression = e;
    while (ts.isParenthesizedExpression(x) || ts.isAsExpression(x) || ts.isNonNullExpression(x)) x = x.expression;
    // f.bind(this): the function is f
    if (ts.isCallExpression(x) && ts.isPropertyAccessExpression(x.expression) && x.expression.name.text === 'bind') x = x.expression.expression;
    if (ts.isArrowFunction(x) || ts.isFunctionExpression(x)) return index.unitAt(x)?.id ?? null;
    if (ts.isIdentifier(x) || ts.isPropertyAccessExpression(x)) {
      let s = checker.getSymbolAtLocation(ts.isPropertyAccessExpression(x) ? x.name : x);
      if (s && s.flags & ts.SymbolFlags.Alias) s = checker.getAliasedSymbol(s);
      for (const d of s?.declarations ?? []) {
        const init = ts.isVariableDeclaration(d) || ts.isPropertyDeclaration(d) ? d.initializer : undefined;
        if (ts.isFunctionLike(d) || (init && (ts.isArrowFunction(init) || ts.isFunctionExpression(init)))) {
          const u = index.unitAt(d);
          if (u) return u.id;
        }
      }
    }
    return null;
  };
  const isFunctionTyped = (e: TS.Expression): boolean => checker.getTypeAtLocation(e).getCallSignatures().length > 0;
  const ownOrInside = (unitId: string): boolean => {
    for (let u = index.units.get(unitId); u; u = u.parent ? index.units.get(u.parent) : undefined) if (own.has(u.id)) return true;
    return false;
  };
  /** Who may call a function given to `d`: a package or the platform, or shared code of the program (not the feature). */
  const receiverOf = (d: TS.Declaration): string | null => {
    const k = keyOf(d);
    if (k) return k;
    const u = index.unitAt(d);
    return u && !ownOrInside(u.id) ? `program:${u.id}` : null;
  };
  /** The external class among a class's bases (through the program's own classes), as its key. */
  const externalBase = (cls: TS.ClassLikeDeclaration): string | null => {
    const seen = new Set<TS.Type>();
    // the class's declared type and its bases (a base as written is a reference: its target is the class)
    const declared = (t: TS.Type): TS.Type => ((t as TS.TypeReference).target ?? t);
    const bases = (t: TS.Type): TS.Type[] => { const d = declared(t); return d.isClassOrInterface() ? checker.getBaseTypes(d as TS.InterfaceType) : []; };
    const symbol = cls.name ? checker.getSymbolAtLocation(cls.name) : undefined;
    const stack: TS.Type[] = symbol ? bases(checker.getDeclaredTypeOfSymbol(symbol)) : [];
    for (const clause of cls.heritageClauses ?? []) if (clause.token === ts.SyntaxKind.ExtendsKeyword) for (const t of clause.types) stack.push(checker.getTypeAtLocation(t));
    while (stack.length > 0) {
      const t = stack.pop()!;
      if (seen.has(t)) continue;
      seen.add(t);
      for (const d of t.getSymbol()?.declarations ?? []) { const k = keyOf(d); if (k) return k; }
      stack.push(...bases(t));
    }
    return null;
  };

  // A function the feature only forwards (`sendUp(mesh, priority)` where `priority` is its own parameter): what reaches
  // the receiver is what the feature's calls pass in that parameter, followed back to the functions written there.
  const calls = new Map<TS.Node, (TS.CallExpression | TS.NewExpression)[]>();
  const forwards: { fn: TS.SignatureDeclaration; index: number; by: string }[] = [];
  const paramOf = (e: TS.Expression): { fn: TS.SignatureDeclaration; index: number } | null => {
    let x: TS.Expression = e;
    while (ts.isParenthesizedExpression(x) || ts.isAsExpression(x) || ts.isNonNullExpression(x)) x = x.expression;
    if (!ts.isIdentifier(x)) return null;
    const d = checker.getSymbolAtLocation(x)?.declarations?.[0];
    if (!d || !ts.isParameter(d) || !ts.isFunctionLike(d.parent)) return null;
    return { fn: d.parent, index: d.parent.parameters.indexOf(d) };
  };
  const give = (a: TS.Expression, by: string, at: TS.Node): void => {
    const u = functionUnit(a);
    if (u) { add({ unit: u, how: 'callback', by, at: where(at) }); return; }
    const p = paramOf(a);
    if (p) forwards.push({ ...p, by });
  };

  for (const id of own) {
    const unit = index.units.get(id)!;
    const node = index.nodeOf(unit);
    if (!node) continue;
    // construction by the package: a class whose bases are a package's is built by it too (three's `clone()` and
    // `copy()` run `new this.constructor()`), so its constructor is an entry from the platform
    if (ts.isClassLike(node)) {
      const base = externalBase(node);
      const ctor = node.members.find((m) => ts.isConstructorDeclaration(m) && m.body);
      const target = ctor ? index.unitAt(ctor)?.id ?? id : id;
      if (base) add({ unit: target, how: 'override', by: `${base}.constructor`, at: where(ctor ?? node) });
    }
    // override: a member of a class whose bases declare it in a package
    if ((ts.isMethodDeclaration(node) || ts.isGetAccessorDeclaration(node) || ts.isSetAccessorDeclaration(node) || ts.isPropertyDeclaration(node))
      && ts.isClassLike(node.parent) && node.name && (ts.isIdentifier(node.name) || ts.isStringLiteral(node.name))) {
      const by = baseDeclaration(node.parent, node.name.text);
      if (by) add({ unit: id, how: 'override', by, at: where(node) });
    }
    // callback: written in the unit's own code (nested units are visited as their own unit)
    const visit = (n: TS.Node): void => {
      const inner = n === node ? null : index.unitAt(n);
      if (inner && inner.id !== id && index.nodeOf(inner) === n) return;
      if (ts.isBinaryExpression(n) && n.operatorToken.kind === ts.SyntaxKind.EqualsToken && ts.isPropertyAccessExpression(n.left)) {
        const s = checker.getSymbolAtLocation(n.left.name);
        const d = s?.declarations?.[0];
        const by = d ? receiverOf(d) : null;
        if (by && isFunctionTyped(n.right)) give(n.right, by, n);
      }
      if (ts.isCallExpression(n) || ts.isNewExpression(n)) {
        const sig = checker.getResolvedSignature(n);
        const d = sig?.declaration;
        if (d && !ts.isJSDocSignature(d)) (calls.get(d) ?? calls.set(d, []).get(d)!).push(n);
        const by = d && !ts.isJSDocSignature(d) ? receiverOf(d) : null;
        if (by && !IN_PLACE.test(by)) {
          // a function given as an argument, or as a member of an object literal given as an argument
          const given = (n.arguments ?? []).flatMap((a) => ts.isObjectLiteralExpression(a)
            ? a.properties.flatMap((p) => ts.isPropertyAssignment(p) ? [p.initializer] : ts.isMethodDeclaration(p) ? [] : [])
            : [a]);
          for (const a of given) if (isFunctionTyped(a)) give(a, by, a);
          for (const a of n.arguments ?? []) if (ts.isObjectLiteralExpression(a)) for (const p of a.properties) if (ts.isMethodDeclaration(p)) {
            const u = index.unitAt(p)?.id;
            if (u) add({ unit: u, how: 'callback', by, at: where(p) });
          }
        }
      }
      ts.forEachChild(n, visit);
    };
    ts.forEachChild(node, visit);
  }
  // the forwarded parameters, back through the feature's calls, to a fixpoint
  const done = new Set<string>();
  while (forwards.length > 0) {
    const f = forwards.pop()!;
    const k = `${f.fn.pos}:${f.fn.getSourceFile().fileName}:${f.index}:${f.by}`;
    if (done.has(k)) continue;
    done.add(k);
    for (const call of calls.get(f.fn) ?? []) {
      const a = call.arguments?.[f.index];
      if (a && isFunctionTyped(a)) give(a, f.by, a);
    }
  }
  return [...hooks.values()].sort((x, y) => x.unit.localeCompare(y.unit) || x.by.localeCompare(y.by));
}

export interface CastExternal {
  /** The package's member reached through a type of the program (`three:BatchedMesh._validateGeometry`). */
  readonly key: string;
  /** Where the value is cast (file:line). */
  readonly at: string;
}

/**
 * A package's object seen through a type of the program (`this as unknown as Internals`, `mesh as any as Packed`):
 * every member of that type is the package's member, even one its declarations do not have (a private field, an
 * internal method). The graph sees only the program's type, so these are the external APIs a cast hides.
 */
export function castExternals(ts: typeof TS, checker: TS.TypeChecker, index: UnitIndex, b: FeatureBoundary, root: string): CastExternal[] {
  const found = new Map<string, CastExternal>();
  const keyOf = (d: TS.Declaration): string | null => {
    const module = moduleOf(d.getSourceFile().fileName, root);
    return module.startsWith('project:') ? null : externalKey(module, containerName(ts, d), declarationName(ts, d, '?'));
  };
  /** The package class a type is, or derives from. */
  const externalOf = (t: TS.Type): string | null => {
    const seen = new Set<TS.Type>();
    const stack = [t];
    while (stack.length > 0) {
      const x = stack.pop()!;
      if (seen.has(x)) continue;
      seen.add(x);
      for (const d of x.getSymbol()?.declarations ?? []) { const k = keyOf(d); if (k) return k; }
      // `this` is a type parameter whose constraint is the class; a reference's target is the class it names
      if (x.isTypeParameter()) { const c = checker.getBaseConstraintOfType(x); if (c) stack.push(c); }
      const declared = (x as TS.TypeReference).target ?? x;
      if (declared.isClassOrInterface()) stack.push(...checker.getBaseTypes(declared as TS.InterfaceType));
      if (x.isIntersection()) stack.push(...x.types);
    }
    return null;
  };
  for (const id of b.own) {
    const node = index.nodeOf(index.units.get(id)!);
    if (!node) continue;
    const visit = (n: TS.Node): void => {
      if ((ts.isAsExpression(n) || ts.isTypeAssertionExpression(n)) && !(ts.isAsExpression(n.parent) || ts.isTypeAssertionExpression(n.parent))) {
        let source: TS.Expression = n.expression;
        while (ts.isAsExpression(source) || ts.isTypeAssertionExpression(source) || ts.isParenthesizedExpression(source)) source = source.expression;
        const from = externalOf(checker.getTypeAtLocation(source));
        const target = checker.getTypeAtLocation(n);
        if (from && !externalOf(target)) {
          const sf = n.getSourceFile();
          const at = `${index.relative(sf)}:${sf.getLineAndCharacterOfPosition(n.getStart(sf)).line + 1}`;
          for (const p of target.getProperties()) {
            if (!(p.declarations ?? []).some((d) => moduleOf(d.getSourceFile().fileName, root).startsWith('project:'))) continue;
            const key = `${from}.${p.name}`;
            found.set(key, { key, at });
          }
        }
      }
      ts.forEachChild(n, visit);
    };
    visit(node);
  }
  return [...found.values()].sort((x, y) => x.key.localeCompare(y.key));
}
