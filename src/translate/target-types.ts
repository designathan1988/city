// The source types the target already has, found by shape instead of named by hand. A source interface whose fields
// are exactly a target struct's fields (same names in the target's case, compatible scalar kinds: a real where the
// target has float or double, an integer where it has an integer, a boolean where it has bool) is that struct. The
// target's own layer rules choose among candidates: one the using layer may not include is out, and core (which
// every layer includes) is preferred; an ambiguity left after that is not mapped (the type is translated). The struct
// must be built the way the translation builds a value: a constructor taking its fields in order, or none at all.
import type * as TS from 'typescript';
import type { GraphIR, UnitRecord } from '../graph/ir.ts';
import type { TypeScriptApi } from '../graph/typescript.ts';
import { Num, type FlowGraph } from './flow.ts';
import { snake } from './names.ts';

export interface TargetType {
  readonly type: string;
  readonly include: string;
  /** Source field → target field, in the target's declaration order (the constructor's order). */
  readonly fields: Readonly<Record<string, string>>;
}

type Kind = 'real' | 'int' | 'bool';

function targetKind(type: string): Kind | null {
  const t = type.replace(/^const\s+/, '').trim();
  if (/^(float|double)$/.test(t)) return 'real';
  if (/^(int|unsigned|long|short|u?int(8|16|32|64)_t|size_t)$/.test(t)) return 'int';
  if (t === 'bool') return 'bool';
  return null;
}

interface Candidate {
  readonly unit: UnitRecord;
  readonly header: string;
  readonly fields: readonly { readonly name: string; readonly kind: Kind }[];
}

/** The target's structs a value can be: scalar fields only, built by a constructor of its fields or by aggregate. */
function candidates(target: GraphIR): Candidate[] {
  const byParent = new Map<string, UnitRecord[]>();
  for (const u of target.units) if (u.parent) byParent.set(u.parent, [...(byParent.get(u.parent) ?? []), u]);
  const out: Candidate[] = [];
  for (const cls of target.units.filter((u) => u.kind === 'class')) {
    const members = byParent.get(cls.id) ?? [];
    const fields = members.filter((m) => m.kind === 'field' && !m.facts.isStatic).sort((a, b) => a.start.line - b.start.line || a.start.column - b.start.column);
    if (fields.length === 0) continue;
    const kinds = fields.map((f) => targetKind(f.signature));
    if (kinds.some((k) => k === null)) continue;
    const ctors = members.filter((m) => m.kind === 'constructor');
    const byFields = ctors.some((c) => (c.signature.match(/\(([^)]*)\)/)?.[1] ?? '').split(',').filter((p) => p.trim()).length === fields.length);
    if (ctors.length > 0 && !byFields) continue;
    out.push({ unit: cls, header: cls.file.replace(/^native\/src\//, ''),
      fields: fields.map((f, i) => ({ name: f.name.slice(f.name.lastIndexOf(':') + 1), kind: kinds[i]! })) });
  }
  return out;
}

export interface TargetTypeInput {
  readonly ts: TypeScriptApi;
  readonly checker: TS.TypeChecker;
  readonly sources: readonly TS.SourceFile[];
  readonly flow: FlowGraph;
  readonly target: GraphIR;
  /** The target's layer rules (rules.json cpp_layers): folder → the include prefixes it may use. */
  readonly layers: Readonly<Record<string, readonly string[]>>;
}

/** Source type name → the target struct it is, for every source interface that has one. */
export function matchTargetTypes(input: TargetTypeInput): Record<string, TargetType> {
  const { ts, checker, flow } = input;
  const pool = candidates(input.target);
  // A header every simulation layer may include: what a shared value type of the source must live in.
  const includableEverywhere = (header: string): boolean =>
    Object.entries(input.layers).filter(([folder]) => folder.startsWith('native/src/sim')).every(([, allowed]) => allowed.some((p) => header.startsWith(p)));
  const out: Record<string, TargetType> = Object.create(null) as Record<string, TargetType>;
  for (const file of input.sources) {
    for (const st of file.statements) {
      if (!ts.isInterfaceDeclaration(st) && !ts.isTypeAliasDeclaration(st)) continue;
      const type = checker.getTypeAtLocation(st.name);
      if (!(type.flags & ts.TypeFlags.Object) || type.getCallSignatures().length > 0) continue;
      const props = checker.getPropertiesOfType(type);
      if (props.length === 0 || props.some((p) => (p.flags & ts.SymbolFlags.Optional) !== 0)) continue;
      const kinds = props.map((p): Kind | null => {
        const t = checker.getNonNullableType(checker.getTypeOfSymbol(p));
        if (t.flags & ts.TypeFlags.NumberLike) return flow.numberOf(flow.symbolSlot(p)) === Num.Float ? 'real' : 'int';
        if (t.flags & ts.TypeFlags.BooleanLike) return 'bool';
        return null;
      });
      if (kinds.some((k) => k === null)) continue;
      const wanted = new Map(props.map((p, i) => [snake(p.name), { source: p.name, kind: kinds[i]! }]));
      const fit = pool.filter((c) => c.fields.length === wanted.size && c.fields.every((f) => wanted.get(f.name)?.kind === f.kind));
      const allowed = fit.filter((c) => includableEverywhere(c.header));
      if (allowed.length !== 1) continue;
      const chosen = allowed[0]!;
      const fields = Object.fromEntries(chosen.fields.map((f) => [wanted.get(f.name)!.source, f.name]));
      out[st.name.text] = { type: chosen.unit.name, include: chosen.header, fields };
    }
  }
  return out;
}
