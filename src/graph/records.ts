// Turns the registry into the serialisable IR: positions, sizes, signatures, export status, facts and edges keyed by
// readable ids, sorted so the same source always gives byte-identical output.
import type * as TS from 'typescript';
import type { EdgeKind, EdgeRecord, ExternalSummary, ExternalUse, Range, UnitRecord } from './ir.ts';
import { isMutableModuleVariable } from './references.ts';
import type { Registry, Unit } from './registry.ts';
import { syntaxFacts } from './facts.ts';
import type { SourceProgram } from './typescript.ts';

function hasModifier(sp: SourceProgram, n: TS.Node, kind: TS.SyntaxKind): boolean {
  const { ts } = sp;
  return ts.canHaveModifiers(n) && (ts.getModifiers(n) ?? []).some((m) => m.kind === kind);
}

function signatureOf(sp: SourceProgram, unit: Unit): string {
  const { ts, checker } = sp;
  const n = unit.ref.node;
  const flags = ts.TypeFormatFlags.NoTruncation;
  if (unit.ref.head && ts.isClassLike(n)) return (n.heritageClauses ?? []).map((h) => h.getText()).join(' ');
  if (ts.isFunctionLike(n)) {
    const signature = checker.getSignatureFromDeclaration(n);
    return signature ? checker.signatureToString(signature, undefined, flags) : '';
  }
  if (ts.isPropertyDeclaration(n)) return checker.typeToString(checker.getTypeAtLocation(n), undefined, flags);
  if (ts.isVariableStatement(n)) {
    return n.declarationList.declarations
      .map((d) => `${d.name.getText()}: ${checker.typeToString(checker.getTypeAtLocation(d), undefined, flags)}`)
      .join('; ');
  }
  return '';
}

/** The function node that runs when the unit is called (a method itself, a field's or variable's arrow). */
function functionNode(sp: SourceProgram, unit: Unit): TS.FunctionLikeDeclaration | null {
  const { ts } = sp;
  const n = unit.ref.node;
  if (unit.ref.head) return null;
  if (ts.isFunctionDeclaration(n) || ts.isMethodDeclaration(n) || ts.isConstructorDeclaration(n) || ts.isGetAccessorDeclaration(n) ||
    ts.isSetAccessorDeclaration(n)) {
    return n;
  }
  const value = ts.isPropertyDeclaration(n) || ts.isPropertyAssignment(n) ? n.initializer
    : ts.isVariableStatement(n) ? n.declarationList.declarations[0]?.initializer : undefined;
  return value && (ts.isArrowFunction(value) || ts.isFunctionExpression(value)) ? value : null;
}

function callableHead(sp: SourceProgram, unit: Unit): Range | null {
  const fn = functionNode(sp, unit);
  if (!fn) return null;
  const sf = unit.ref.node.getSourceFile();
  const at = (pos: number): { line: number; column: number } => {
    const lc = sf.getLineAndCharacterOfPosition(pos);
    return { line: lc.line + 1, column: lc.character + 1 };
  };
  return { start: at(unit.ref.node.getStart(sf)), end: at(fn.body ? fn.body.getStart(sf) : fn.end) };
}

function isCallable(sp: SourceProgram, unit: Unit): boolean {
  const { ts } = sp;
  const n = unit.ref.node;
  if (['function', 'method', 'constructor', 'getter', 'setter', 'staticBlock'].includes(unit.kind)) return true;
  return ts.isPropertyDeclaration(n) && n.initializer !== undefined &&
    (ts.isArrowFunction(n.initializer) || ts.isFunctionExpression(n.initializer));
}

function externalUses(registry: Registry, key: string): ExternalUse[] {
  const uses: ExternalUse[] = [];
  for (const [externalKey, kinds] of registry.external.get(key) ?? []) {
    for (const [kind, count] of kinds) uses.push({ key: externalKey, kind, count });
  }
  return uses.sort((a, b) => a.key.localeCompare(b.key) || a.kind.localeCompare(b.kind));
}

export function unitRecord(sp: SourceProgram, registry: Registry, unit: Unit, relativeFile: string): UnitRecord {
  const { ts } = sp;
  const n = unit.ref.node;
  const sf = n.getSourceFile();
  const start = sf.getLineAndCharacterOfPosition(n.getStart(sf));
  const end = sf.getLineAndCharacterOfPosition(n.end);
  const owner = unit.ref.head ? n : ts.isClassElement(n) && n.parent ? n.parent : n;
  const syntax = syntaxFacts(ts, unit.ref);
  const nondeterminism = [...new Set([...syntax.nondeterminism, ...(registry.nondeterminism.get(unit.key) ?? [])])].sort();
  return {
    id: unit.id,
    kind: unit.kind,
    name: unit.name,
    file: relativeFile,
    start: { line: start.line + 1, column: start.character + 1 },
    end: { line: end.line + 1, column: end.character + 1 },
    lines: end.line - start.line + 1,
    exported: hasModifier(sp, owner, ts.SyntaxKind.ExportKeyword),
    signature: signatureOf(sp, unit),
    parent: unit.parentKey ? registry.units.get(unit.parentKey)!.id : null,
    nested: syntax.nested,
    callableHead: callableHead(sp, unit),
    facts: {
      external: externalUses(registry, unit.key),
      nondeterminism,
      loopAllocations: syntax.loopAllocations,
      numericLiterals: syntax.numericLiterals,
      stringLiterals: syntax.stringLiterals,
      decisions: syntax.decisions,
      loops: syntax.loops,
      awaits: syntax.awaits,
      bitOperations: syntax.bitOperations,
      isStatic: hasModifier(sp, n, ts.SyntaxKind.StaticKeyword),
      callable: isCallable(sp, unit),
      mutableModuleState: isMutableModuleVariable(ts, n),
      patterns: syntax.patterns,
      fingerprints: syntax.fingerprints,
    },
  };
}

export function edgeRecords(registry: Registry): EdgeRecord[] {
  const edges: EdgeRecord[] = [];
  for (const [from, targets] of registry.edges) {
    for (const [to, kinds] of targets) {
      for (const [kind, count] of kinds) {
        edges.push({ from: registry.units.get(from)!.id, to: registry.units.get(to)!.id, kind: kind as EdgeKind, count });
      }
    }
  }
  return edges.sort((a, b) => a.from.localeCompare(b.from) || a.to.localeCompare(b.to) || a.kind.localeCompare(b.kind));
}

export function externalSummary(units: readonly UnitRecord[]): ExternalSummary[] {
  const byKey = new Map<string, { uses: number; units: Set<string> }>();
  for (const unit of units) {
    for (const use of unit.facts.external) {
      const entry = byKey.get(use.key) ?? { uses: 0, units: new Set<string>() };
      entry.uses += use.count;
      entry.units.add(unit.id);
      byKey.set(use.key, entry);
    }
  }
  return [...byKey]
    .map(([key, entry]) => {
      // `three:Vector3.add`, `@scope/pkg:Name`, `project:tests/x.ts:Name` (the module itself contains a colon).
      const split = key.startsWith('project:') ? key.lastIndexOf(':') : key.indexOf(':');
      return { key, module: key.slice(0, split), symbol: key.slice(split + 1), uses: entry.uses, units: entry.units.size };
    })
    .sort((a, b) => a.key.localeCompare(b.key));
}
