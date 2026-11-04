// Walks one unit's code and resolves every identifier through the type checker: in-scope declarations become units
// and typed edges, out-of-scope ones become external uses. Interface and abstract members reach every
// implementation, function-typed fields reach every place that stores a callback in them, and workers or dynamic
// imports pull their whole module.
import path from 'node:path';
import fs from 'node:fs';
import type * as TS from 'typescript';
import { normalizePath } from '../common/paths.ts';
import type { ModuleLoad } from './ir.ts';
import { containerName, declarationName, externalKey, isNondeterministic, moduleOf } from './origin.ts';
import { isModuleLevelVariable, referenceKinds } from './references.ts';
import type { Registry } from './registry.ts';
import type { SourceProgram } from './typescript.ts';
import { startsUnit, statementUnits, unitKey, unitOf, type UnitRef } from './units.ts';

interface Targets {
  readonly implementations: UnitRef[];
  readonly assigners: UnitRef[];
}

export class Walker {
  readonly moduleLoads: ModuleLoad[] = [];
  readonly unresolvedModules: { from: string; specifier: string }[] = [];
  readonly #sp: SourceProgram;
  readonly #registry: Registry;
  readonly #inScope: (file: string) => boolean;
  readonly #targets = new Map<TS.Declaration, Targets>();
  /** Whole-scope mode registers every unit met; entry mode only what is used. */
  readonly #wholeScope: boolean;

  constructor(sp: SourceProgram, registry: Registry, inScope: (file: string) => boolean, wholeScope: boolean) {
    this.#sp = sp;
    this.#registry = registry;
    this.#inScope = inScope;
    this.#wholeScope = wholeScope;
  }

  walk(key: string): void {
    const { ts } = this.#sp;
    const unit = this.#registry.units.get(key)!;
    const root = unit.ref.node;
    const visit = (n: TS.Node): void => {
      // A nested unit or class member inside this unit is a unit of its own: its code is not this unit's.
      if (n !== root && startsUnit(ts, n)) {
        if (this.#wholeScope) this.#registry.add({ node: n, head: false }, null, null);
        return;
      }
      if (ts.isIdentifier(n) || ts.isPrivateIdentifier(n)) this.#identifier(key, n);
      if (this.#isWorkerConstruction(n) || (ts.isCallExpression(n) && n.expression.kind === ts.SyntaxKind.ImportKeyword)) {
        const specifier = this.#firstString(n);
        if (specifier !== null) this.#loadModule(key, root.getSourceFile().fileName, specifier, ts.isCallExpression(n) ? 'dynamicImport' : 'worker');
      }
      ts.forEachChild(n, visit);
    };
    if (unit.ref.head && ts.isClassLike(root)) {
      if (root.name) visit(root.name);
      for (const clause of root.heritageClauses ?? []) visit(clause);
      // Constructing the class runs its field initializers, static blocks and constructor.
      for (const member of root.members) {
        if (ts.isPropertyDeclaration(member) || ts.isConstructorDeclaration(member) || ts.isClassStaticBlockDeclaration(member) ||
          ts.isIndexSignatureDeclaration(member)) {
          this.#registry.add({ node: member, head: false }, null, null);
        }
      }
      return;
    }
    visit(root);
  }

  #identifier(key: string, n: TS.Identifier | TS.PrivateIdentifier): void {
    const { ts, checker } = this.#sp;
    let symbol = checker.getSymbolAtLocation(n);
    if (n.parent && ts.isShorthandPropertyAssignment(n.parent) && n.parent.name === n) {
      symbol = checker.getShorthandAssignmentValueSymbol(n.parent) ?? symbol;
    }
    if (!symbol) return;
    let declarations = symbol.declarations ?? [];
    if (symbol.flags & ts.SymbolFlags.Alias) {
      const aliased = checker.getAliasedSymbol(symbol);
      if (aliased.declarations && aliased.declarations.length > 0) {
        declarations = aliased.declarations;
      } else {
        this.#unresolvedImport(key, declarations);
        return;
      }
    }
    let externalDone = false;
    for (const declaration of declarations) {
      const d = this.#valueDeclaration(declaration);
      if (!this.#inScope(d.getSourceFile().fileName)) {
        if (!externalDone) this.#external(key, n, d, symbol.name);
        externalDone = true;
        continue;
      }
      const target = unitOf(ts, d);
      if (!target || unitKey(target) === key) continue; // code local to this unit
      // A variable or parameter of an enclosing unit, used here: closure state the enclosing unit owns.
      const captured = (ts.isVariableDeclaration(d) || ts.isParameter(d)) && !isModuleLevelVariable(ts, d) &&
        target.node !== d.parent?.parent;
      for (const kind of referenceKinds(ts, n, d, captured)) this.#registry.add(target, key, kind);
      this.#followDynamicDispatch(key, d);
    }
  }

  /** `{ add }` and `{ add: add }` stand for the function `add` itself, not for the object literal's member. */
  #valueDeclaration(d: TS.Declaration): TS.Declaration {
    const { ts, checker } = this.#sp;
    let symbol: TS.Symbol | undefined;
    if (ts.isShorthandPropertyAssignment(d)) symbol = checker.getShorthandAssignmentValueSymbol(d);
    else if (ts.isPropertyAssignment(d) && ts.isIdentifier(d.initializer)) symbol = checker.getSymbolAtLocation(d.initializer);
    if (symbol && symbol.flags & ts.SymbolFlags.Alias) symbol = checker.getAliasedSymbol(symbol);
    return symbol?.declarations?.[0] ?? d;
  }

  #followDynamicDispatch(key: string, d: TS.Declaration): void {
    const { ts, checker } = this.#sp;
    const isAbstract = ts.canHaveModifiers(d) && (ts.getModifiers(d) ?? []).some((m) => m.kind === ts.SyntaxKind.AbstractKeyword);
    const isFunctionField = (ts.isPropertySignature(d) || ts.isPropertyDeclaration(d)) &&
      checker.getTypeAtLocation(d).getNonNullableType().getCallSignatures().length > 0;
    if (!ts.isMethodSignature(d) && !isAbstract && !isFunctionField) return;
    const targets = this.#dispatchTargets(d, isFunctionField);
    for (const ref of targets.implementations) this.#registry.add(ref, key, 'implementation');
    for (const ref of targets.assigners) this.#registry.add(ref, key, 'callbackAssign');
  }

  #dispatchTargets(d: TS.Declaration, isFunctionField: boolean): Targets {
    const cached = this.#targets.get(d);
    if (cached) return cached;
    const { ts, service } = this.#sp;
    const targets: Targets = { implementations: [], assigners: [] };
    this.#targets.set(d, targets);
    const name = ts.getNameOfDeclaration(d);
    if (!name) return targets;
    const dsf = d.getSourceFile();
    for (const location of service.getImplementationAtPosition(dsf.fileName, name.getStart(dsf)) ?? []) {
      const ref = this.#unitAt(location.fileName, location.textSpan.start);
      if (ref) targets.implementations.push(ref);
    }
    if (isFunctionField) {
      for (const reference of service.getReferencesAtPosition(dsf.fileName, name.getStart(dsf)) ?? []) {
        if (!reference.isWriteAccess) continue;
        const ref = this.#unitAt(reference.fileName, reference.textSpan.start);
        if (ref) targets.assigners.push(ref);
      }
    }
    return targets;
  }

  #unitAt(fileName: string, position: number): UnitRef | null {
    if (!this.#inScope(fileName)) return null;
    const sf = this.#sp.sourceFile(fileName);
    if (!sf) return null;
    const { ts } = this.#sp;
    // Assigned inside the closure: the cast keeps the compiler from narrowing it to `null`.
    let found = null as TS.Node | null;
    const visit = (n: TS.Node): void => {
      if (position >= n.getStart(sf) && position < n.end) {
        found = n;
        ts.forEachChild(n, visit);
      }
    };
    ts.forEachChild(sf, visit);
    return found ? unitOf(ts, found) : null;
  }

  #external(key: string, n: TS.Identifier | TS.PrivateIdentifier, d: TS.Declaration, fallback: string): void {
    const { ts } = this.#sp;
    const module = moduleOf(d.getSourceFile().fileName, this.#sp.root);
    const externalId = externalKey(module, containerName(ts, d), declarationName(ts, d, fallback));
    for (const kind of referenceKinds(ts, n, d)) this.#registry.externalUse(key, externalId, kind);
    if (isNondeterministic(externalId)) this.#registry.markNondeterministic(key, externalId);
  }

  #unresolvedImport(key: string, bindings: readonly TS.Declaration[]): void {
    const { ts } = this.#sp;
    for (const binding of bindings) {
      let node: TS.Node | undefined = binding;
      while (node && !ts.isImportDeclaration(node)) node = node.parent;
      if (!node || !ts.isImportDeclaration(node) || !ts.isStringLiteral(node.moduleSpecifier)) continue;
      const specifier = node.moduleSpecifier.text;
      if (/\?(shared)?worker\b/.test(specifier)) {
        this.#loadModule(key, node.getSourceFile().fileName, specifier, 'workerImport');
      } else if (!this.unresolvedModules.some((u) => u.from === key && u.specifier === specifier)) {
        this.unresolvedModules.push({ from: key, specifier });
      }
    }
  }

  #isWorkerConstruction(n: TS.Node): boolean {
    const { ts } = this.#sp;
    return ts.isNewExpression(n) && /(^|\.)(Shared)?Worker$/.test(n.expression.getText());
  }

  #firstString(n: TS.Node): string | null {
    const { ts } = this.#sp;
    let found = null as string | null;
    const visit = (m: TS.Node): void => {
      if (found !== null) return;
      if (ts.isStringLiteralLike(m)) found = m.text;
      else ts.forEachChild(m, visit);
    };
    ts.forEachChild(n, visit);
    return found;
  }

  #loadModule(key: string, fromFile: string, specifier: string, how: ModuleLoad['how']): void {
    const { ts, program } = this.#sp;
    const clean = specifier.replace(/\?.*$/, '');
    let file = ts.resolveModuleName(clean, fromFile, program.getCompilerOptions(), ts.sys).resolvedModule?.resolvedFileName;
    if (!file) {
      const base = path.resolve(path.dirname(fromFile), clean);
      file = ['', '.ts', '.tsx', '.js', '.mjs'].map((ext) => base + ext).find((f) => fs.existsSync(f) && fs.statSync(f).isFile());
    }
    const from = this.#registry.units.get(key)!.id;
    const normal = file ? normalizePath(file) : null;
    if (!this.moduleLoads.some((m) => m.from === from && m.specifier === specifier)) {
      this.moduleLoads.push({ from, specifier, file: normal, how });
    }
    const sf = normal ? this.#sp.sourceFile(normal) : undefined;
    if (!sf || !this.#inScope(sf.fileName)) return;
    for (const statement of sf.statements) {
      for (const ref of statementUnits(ts, statement)) this.#registry.add(ref, key, 'moduleLoad');
    }
  }
}
