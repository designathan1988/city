// The shared code the feature uses, as what it is called by: each declaration as the source writes it with its
// documentation, a function or method up to its body (name, parameters, types, what it returns), a class its head (its
// members used are declarations of their own), anything else whole (types, interfaces, enums, constants, fields: their
// whole text is what they are). The types that text names, the values a value is written from and what the slices of
// shared code read are written too, to a fixpoint, so the interface reads on its own. Written by source file into
// interface/.
import fs from 'node:fs';
import path from 'node:path';
import type * as TS from 'typescript';
import type { UnitRecord } from '../graph/ir.ts';
import type { FeatureBoundary } from './boundary.ts';
import type { FeatureSlice } from './slice.ts';
import type { UnitIndex } from './units.ts';

export interface InterfaceOut {
  readonly file: string;
  readonly declarations: number;
}

export function writeInterface(ts: typeof TS, checker: TS.TypeChecker, index: UnitIndex, b: FeatureBoundary, slices: readonly FeatureSlice[], dir: string): InterfaceOut[] {
  const own = new Set(b.own);
  const included = new Map<string, string>();
  /** The declarations written whole (a declaration inside one of them is in its text already). */
  const whole = new Set<string>();
  const queue: UnitRecord[] = [];
  const add = (u: UnitRecord | null | undefined): void => {
    if (!u || own.has(u.id) || included.has(u.id) || queue.includes(u)) return;
    queue.push(u);
  };
  /** The declaration a name in the program stands for, when it is a declaration of the file's top level or a member. */
  const follow = (ref: TS.Node): void => {
    const symbol = checker.getSymbolAtLocation(ref);
    const s = symbol && symbol.flags & ts.SymbolFlags.Alias ? checker.getAliasedSymbol(symbol) : symbol;
    for (const d of s?.declarations ?? []) {
      const u = index.unitAt(d);
      // A local of the code it stands in is in that code's text.
      if (u && !isLocalTo(ts, d)) add(u);
    }
  };
  for (const x of b.uses) add(index.units.get(x.unit));
  // A slice of shared code is the feature's code too: what its lines read is part of what the feature uses.
  for (const slice of slices) {
    const node = index.nodeOf(index.units.get(slice.unit)!);
    if (node) for (const ref of namesOnLines(ts, node, new Set(slice.lines))) follow(ref);
  }
  for (let i = 0; i < queue.length; i++) {
    const u = queue[i]!;
    const node = index.nodeOf(u);
    if (!node) throw new Error(`no node for ${u.id}: the graph and the program disagree`);
    const { text, named, all } = written(ts, u, node);
    included.set(u.id, text);
    if (all) whole.add(u.id);
    // The types the written text names and the values a whole value is written from, declared in the program.
    for (const ref of named) follow(ref);
    if (all) for (const ref of namesIn(ts, node)) follow(ref);
  }

  const byFile = new Map<string, UnitRecord[]>();
  for (const id of included.keys()) {
    const u = index.units.get(id)!;
    (byFile.get(u.file) ?? byFile.set(u.file, []).get(u.file)!).push(u);
  }
  const out: InterfaceOut[] = [];
  for (const [file, units] of [...byFile].sort(([a], [c]) => a.localeCompare(c))) {
    const kept = units.filter((u) => !(u.parent !== null && whole.has(u.parent)))
      .sort((x, y) => x.start.line - y.start.line || x.start.column - y.start.column);
    const parts = kept.map((u) => `// ${file}:${u.start.line}-${u.end.line} ${u.kind} ${u.name}\n${included.get(u.id)!}`);
    const target = path.join(dir, file);
    fs.mkdirSync(path.dirname(target), { recursive: true });
    fs.writeFileSync(target, `// What the feature uses of ${file} (shared with the program): declarations as the source writes them, bodies left out.\n\n${parts.join('\n\n')}\n`, 'utf8');
    out.push({ file, declarations: kept.length });
  }
  return out;
}

/** A declaration's text as written here, the type names in it, and whether it is written whole. */
function written(ts: typeof TS, u: UnitRecord, node: TS.Node): { text: string; named: TS.Node[]; all: boolean } {
  const sf = node.getSourceFile();
  const start = node.getStart(sf, true);
  // A function value (a declaration, a method, a variable or a property holding one): up to its body.
  const fn = functionOf(ts, node);
  const body = fn ? (fn as TS.FunctionLikeDeclaration).body : undefined;
  if (fn && body) {
    const head = sf.text.slice(start, body.getStart(sf)).trimEnd();
    const signature: TS.Node[] = [...fn.parameters, ...(fn.typeParameters ?? []), ...(fn.type ? [fn.type] : [])];
    return { text: ts.isBlock(body) ? `${head};` : `${head} …;`, named: signature.flatMap((n) => typeNames(ts, n)), all: false };
  }
  if (u.kind === 'class' && ts.isClassLike(node)) {
    // The head: up to the brace that opens the members.
    const open = sf.text.indexOf('{', (node.heritageClauses?.at(-1) ?? node.name ?? node).end);
    return { text: `${sf.text.slice(start, open).trimEnd()} { … }`, named: (node.heritageClauses ?? []).flatMap((h) => typeNames(ts, h)), all: false };
  }
  return { text: sf.text.slice(start, node.end), named: typeNames(ts, node), all: true };
}

/** The function a callable unit is: the declaration itself, or a variable's function value. */
function functionOf(ts: typeof TS, node: TS.Node): TS.SignatureDeclaration | null {
  if (ts.isFunctionLike(node)) return node;
  if (ts.isVariableStatement(node)) {
    const init = node.declarationList.declarations[0]?.initializer;
    if (init && (ts.isArrowFunction(init) || ts.isFunctionExpression(init))) return init;
  }
  if (ts.isPropertyAssignment(node) && (ts.isArrowFunction(node.initializer) || ts.isFunctionExpression(node.initializer))) return node.initializer;
  return null;
}

/** Whether a declaration sits inside a function: a local of that code, not a declaration of the program. */
function isLocalTo(ts: typeof TS, d: TS.Node): boolean {
  for (let p = d.parent; p; p = p.parent) if (ts.isFunctionLike(p)) return true;
  return false;
}

/** The identifiers a node reads: a property name after a dot or in an object literal stands for no declaration here. */
function namesIn(ts: typeof TS, node: TS.Node): TS.Identifier[] {
  const out: TS.Identifier[] = [];
  const visit = (n: TS.Node): void => {
    if (ts.isIdentifier(n)) {
      const p = n.parent;
      const named = (ts.isPropertyAccessExpression(p) && p.name === n) || (ts.isPropertyAssignment(p) && p.name === n);
      if (!named) out.push(n);
    }
    ts.forEachChild(n, visit);
  };
  visit(node);
  return out;
}

/** The identifiers a node reads on the given lines (1-based). */
function namesOnLines(ts: typeof TS, node: TS.Node, lines: ReadonlySet<number>): TS.Identifier[] {
  const sf = node.getSourceFile();
  return namesIn(ts, node).filter((id) => lines.has(sf.getLineAndCharacterOfPosition(id.getStart(sf)).line + 1));
}

/** The names of types written in a node: type references, heritage, `typeof` queries. */
function typeNames(ts: typeof TS, node: TS.Node): TS.Node[] {
  const out: TS.Node[] = [];
  const visit = (n: TS.Node): void => {
    if (ts.isTypeReferenceNode(n)) out.push(n.typeName);
    if (ts.isExpressionWithTypeArguments(n)) out.push(n.expression);
    if (ts.isTypeQueryNode(n)) out.push(n.exprName);
    ts.forEachChild(n, visit);
  };
  visit(node);
  return out;
}
