// What the feature uses that is not code: the entries of the program's objects it looks up by key (the interface's
// texts, registries keyed by name: lookups.ts), the style rules of the classes it names, and what it imports that is
// not a code module (style sheets, data files, modules a build plugin provides). Each is the feature's when only its
// code (its own and its slices of shared code) uses it, shared when the rest of the program does too.
import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';
import type * as TS from 'typescript';
import type { FeatureBoundary } from './boundary.ts';
import type { CallTargets } from './callgraph.ts';
import type { PointsTo } from './pointsto.ts';
import { htmlStyleSheets } from './html.ts';
import { keyLookups, keyText, type KeyLookup } from './lookups.ts';
import type { FeatureSlice } from './slice.ts';
import { matches } from './strings.ts';
import { StringValues } from './values.ts';
import type { UnitIndex } from './units.ts';

export interface FeatureResource {
  /** entry: a shared dictionary's entry; style: a style rule; import: a module that is not code. */
  readonly kind: 'entry' | 'style' | 'import';
  /** feature: only the feature uses it; shared: the rest of the program uses it too (a resource the feature uses). */
  readonly owner: 'feature' | 'shared';
  readonly file: string;
  readonly line: number;
  /** The key, class or module specifier it was found by. */
  readonly key: string;
  readonly text: string;
}

/** The part of PostCSS read here (the source project's own copy is loaded, like its TypeScript). */
interface CssNode {
  readonly type: string;
  readonly selector?: string;
  readonly name?: string;
  readonly params?: string;
  readonly parent?: CssNode;
  readonly source?: { readonly start?: { readonly line: number } };
  toString(): string;
}
interface PostCss {
  parse(css: string, options: { from: string }): { walkRules(visit: (rule: CssNode) => void): void };
}

/** `postcssDir` overrides the package; by default it is resolved the way the source project resolves it. */
export function loadPostCss(sourceRoot: string, postcssDir: string | undefined): PostCss {
  try {
    if (postcssDir !== undefined) return createRequire(path.join(path.resolve(postcssDir), 'package.json'))(path.resolve(postcssDir)) as PostCss;
    return createRequire(path.join(path.resolve(sourceRoot), 'package.json'))('postcss') as PostCss;
  } catch (error) {
    throw new Error(`the source has style sheets but PostCSS does not resolve (${postcssDir ?? `from ${sourceRoot}`}): ${String(error)}`);
  }
}

export function featureResources(ts: typeof TS, checker: TS.TypeChecker, index: UnitIndex, sourceRoot: string,
  b: FeatureBoundary, slices: readonly FeatureSlice[], calls: CallTargets, pts: PointsTo, postcssDir: string | undefined): FeatureResource[] {
  const own = new Set(b.own);
  const files = [...index.byFile.keys()].sort();
  const sliceLines = new Map<string, Set<number>>();
  for (const s of slices) for (const l of s.lines) (sliceLines.get(s.file) ?? sliceLines.set(s.file, new Set()).get(s.file)!).add(l);
  const isFeatureCode = (n: TS.Node, file: string): boolean => {
    const u = index.unitAt(n);
    if (u && own.has(u.id)) return true;
    const sf = n.getSourceFile();
    return sliceLines.get(file)?.has(sf.getLineAndCharacterOfPosition(n.getStart(sf)).line + 1) ?? false;
  };
  const featureFiles = files.filter((f) => index.byFile.get(f)!.some((u) => own.has(u.id)) || sliceLines.has(f));

  // The strings the feature's code can produce, and the ones the rest of the program can (what it runs, not dead code).
  // Keys being defined (an object's property names) and module specifiers are not strings code looks things up by.
  const sources = files.map((f) => index.sourceFile(f)).filter((sf): sf is TS.SourceFile => sf !== undefined);
  const values = new StringValues(ts, checker, sources, calls, pts);
  const feature = { exact: new Set<string>(), prefixes: new Set<string>() };
  const program = { exact: new Set<string>(), prefixes: new Set<string>() };
  const isProgramCode = (n: TS.Node, file: string): boolean => !isFeatureCode(n, file) && b.program.has(index.unitAt(n)?.id ?? '');
  for (const file of files) {
    const sf = index.sourceFile(file);
    if (!sf) continue;
    const visit = (n: TS.Node): void => {
      const p = n.parent;
      const defining = p !== undefined && ((ts.isPropertyAssignment(p) && p.name === n) || ((ts.isImportDeclaration(p) || ts.isExportDeclaration(p)) && p.moduleSpecifier === n));
      if (!defining && (ts.isStringLiteralLike(n) || ts.isTemplateExpression(n))) {
        const into = isFeatureCode(n, file) ? feature : isProgramCode(n, file) ? program : null;
        const v = into ? values.of(n) : null;
        if (into && v && !v.top) {
          for (const x of v.exact) into.exact.add(x);
          for (const x of v.prefixes) into.prefixes.add(x);
        }
      }
      ts.forEachChild(n, visit);
    };
    visit(sf);
  }
  const out: FeatureResource[] = [];

  // Entries of the program's objects the feature looks up by key (lookups.ts): the entry is the feature's when only its
  // code looks it up, shared when the rest of the program does too.
  const lookedUp = new Map<TS.ObjectLiteralExpression, { feature: KeyLookup[]; program: KeyLookup[] }>();
  for (const l of keyLookups(ts, checker, sources, (c) => calls.of(c), values)) {
    const file = index.relative(l.at.getSourceFile());
    const side = isFeatureCode(l.at, file) ? 'feature' : isProgramCode(l.at, file) ? 'program' : null;
    if (side) for (const o of l.objects) (lookedUp.get(o) ?? lookedUp.set(o, { feature: [], program: [] }).get(o)!)[side].push(l);
  }
  const given = (key: string, by: readonly KeyLookup[]): boolean => by.some((l) => l.exact.includes(key) || l.prefixes.some((p) => key.startsWith(p)));
  for (const [o, by] of lookedUp) {
    if (by.feature.length === 0) continue;
    const sf = o.getSourceFile();
    const file = index.relative(sf);
    for (const p of o.properties) {
      const key = p.name ? keyText(ts, p.name) : null;
      if (key === null || !given(key, by.feature) || isFeatureCode(p, file)) continue;
      out.push({ kind: 'entry', owner: given(key, by.program) ? 'shared' : 'feature', file, line: sf.getLineAndCharacterOfPosition(p.getStart(sf)).line + 1, key, text: p.getText(sf) });
    }
  }

  // What the feature's code imports that is not a code module.
  const styleSheets = new Set(htmlStyleSheets(sourceRoot));
  for (const file of files) {
    const sf = index.sourceFile(file);
    if (!sf) continue;
    const whole = index.byFile.get(file)!.every((u) => own.has(u.id));
    const inFeature = featureFiles.includes(file);
    for (const st of sf.statements) {
      if (!ts.isImportDeclaration(st) || !ts.isStringLiteral(st.moduleSpecifier) || st.importClause?.isTypeOnly) continue;
      const specifier = st.moduleSpecifier.text;
      if (/\.css(\?|$)/.test(specifier) && specifier.startsWith('.')) styleSheets.add(path.posix.normalize(path.posix.join(path.posix.dirname(file), specifier)));
      if (!inFeature || isCodeModule(ts, checker, st.moduleSpecifier, sourceRoot)) continue;
      if (whole || usesBinding(ts, checker, sf, st, (n) => isFeatureCode(n, file))) {
        const shared = !whole && usesBinding(ts, checker, sf, st, (n) => isProgramCode(n, file));
        out.push({ kind: 'import', owner: shared ? 'shared' : 'feature', file, line: sf.getLineAndCharacterOfPosition(st.getStart(sf)).line + 1, key: specifier, text: st.getText(sf) });
      }
    }
  }

  // Style rules for elements of the feature: a selector whose every class the feature names. The rule is the feature's
  // when one of those classes only the feature names; shared when the rest of the program names them all.
  const isClass = (s: string): boolean => /^-?[_a-zA-Z][\w-]*$/.test(s);
  const classesOf = (found: { exact: Set<string>; prefixes: Set<string> }): { exact: Set<string>; prefixes: string[] } => ({
    exact: new Set([...found.exact].flatMap((x) => x.split(/\s+/)).filter(isClass)),
    prefixes: [...found.prefixes].map((x) => x.split(/\s+/).pop()!).filter((x) => x !== '' && isClass(x)),
  });
  const featureClasses = classesOf(feature);
  const programClasses = classesOf(program);
  if (styleSheets.size > 0 && (featureClasses.exact.size > 0 || featureClasses.prefixes.length > 0)) {
    const postcss = loadPostCss(sourceRoot, postcssDir);
    for (const sheet of [...styleSheets].sort()) {
      const full = path.join(sourceRoot, sheet);
      if (!fs.existsSync(full)) continue;
      postcss.parse(fs.readFileSync(full, 'utf8'), { from: full }).walkRules((rule) => {
        let hit: { key: string; owner: FeatureResource['owner'] } | null = null;
        for (const part of (rule.selector ?? '').split(',')) {
          const named = [...part.matchAll(/\.(-?[_a-zA-Z][\w-]*)/g)].map((m) => m[1]!);
          if (named.length === 0 || !named.every((c) => matches(c, featureClasses.exact, featureClasses.prefixes))) continue;
          const only = named.find((c) => !matches(c, programClasses.exact, programClasses.prefixes));
          if (only) { hit = { key: only, owner: 'feature' }; break; }
          hit ??= { key: named[0]!, owner: 'shared' };
        }
        if (!hit) return;
        const at = rule.parent?.type === 'atrule' ? `@${rule.parent.name} ${rule.parent.params} { ` : '';
        out.push({ kind: 'style', owner: hit.owner, file: sheet, line: rule.source?.start?.line ?? 0, key: hit.key, text: `${at}${rule.toString().trim()}${at ? ' }' : ''}` });
      });
    }
  }
  return out.sort((a, c) => a.kind.localeCompare(c.kind) || a.file.localeCompare(c.file) || a.line - c.line);
}

/**
 * A module of code: a source file of the program or a package's. Not code: what does not resolve (a style sheet), a
 * data file (JSON), a module the project declares for a build plugin to provide (`declare module 'virtual:…'`).
 */
function isCodeModule(ts: typeof TS, checker: TS.TypeChecker, specifier: TS.StringLiteral, sourceRoot: string): boolean {
  const symbol = checker.getSymbolAtLocation(specifier);
  const decl = symbol?.declarations?.[0];
  if (!decl) return false;
  const file = decl.getSourceFile().fileName;
  if (/\.json$/i.test(file)) return false;
  const inProject = path.resolve(file).toLowerCase().startsWith(path.resolve(sourceRoot).toLowerCase() + path.sep) && !/[\\/]node_modules[\\/]/.test(file);
  return !(ts.isModuleDeclaration(decl) && ts.isStringLiteral(decl.name) && inProject);
}

/** Whether a name an import binds is used by the code `inFeature` accepts. */
function usesBinding(ts: typeof TS, checker: TS.TypeChecker, sf: TS.SourceFile, st: TS.ImportDeclaration, inFeature: (n: TS.Node) => boolean): boolean {
  const clause = st.importClause;
  if (!clause) return false;
  const names: TS.Identifier[] = [];
  if (clause.name) names.push(clause.name);
  const nb = clause.namedBindings;
  if (nb && ts.isNamespaceImport(nb)) names.push(nb.name);
  if (nb && ts.isNamedImports(nb)) for (const el of nb.elements) names.push(el.name);
  const bound = new Set(names.map((n) => checker.getSymbolAtLocation(n)).filter((s): s is TS.Symbol => s !== undefined));
  let used = false;
  const visit = (n: TS.Node): void => {
    if (used || n === st) return;
    if (ts.isIdentifier(n)) {
      const s = checker.getSymbolAtLocation(n);
      if (s && bound.has(s) && inFeature(n)) used = true;
    }
    ts.forEachChild(n, visit);
  };
  visit(sf);
  return used;
}
