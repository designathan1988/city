// Indexes over a graph IR that every audit rule reads: units by id, edges by endpoint, a unit's own lines (without
// nested units), directory membership and the hot set (code that runs every frame or every tick).
import type { EdgeKind, EdgeRecord, GraphIR, UnitRecord } from '../graph/ir.ts';

export interface AuditOptions {
  readonly hot: readonly string[];
  readonly uiDirs: readonly string[];
  readonly stateDirs: readonly string[];
  readonly simDirs: readonly string[];
  readonly maxFileLines: number | undefined;
  readonly maxFunctionLines: number | undefined;
  readonly features: readonly { readonly name: string; readonly entry: string }[];
  /** Degree of scattering at or above which a feature is reported (a project decision; Eaddy gives no threshold). */
  readonly maxScattering: number;
  readonly minWriterFiles: number;
  readonly minConstantFiles: number;
}

export type Severity = 'high' | 'medium' | 'low';

export interface Finding {
  readonly rule: string;
  readonly severity: Severity;
  readonly unit: string | null;
  readonly file: string;
  readonly line: number;
  readonly message: string;
}

/** A rule either returns its findings or says why it could not run. */
export type RuleOutcome = Finding[] | { readonly skipped: string };

const HOT_EDGES = new Set<EdgeKind>(['call', 'construct', 'implementation', 'callbackAssign']);

export class AuditContext {
  readonly ir: GraphIR;
  readonly options: AuditOptions;
  readonly byId: Map<string, UnitRecord>;
  readonly outgoing = new Map<string, EdgeRecord[]>();
  readonly incoming = new Map<string, EdgeRecord[]>();
  readonly hotRoots: string[];
  readonly hot: Set<string>;

  constructor(ir: GraphIR, options: AuditOptions) {
    this.ir = ir;
    this.options = options;
    this.byId = new Map(ir.units.map((u) => [u.id, u]));
    for (const edge of ir.edges) {
      push(this.outgoing, edge.from, edge);
      push(this.incoming, edge.to, edge);
    }
    this.hotRoots = this.#hotRoots();
    this.hot = this.reach(this.hotRoots, (edge) =>
      HOT_EDGES.has(edge.kind) || (edge.kind === 'reference' && this.byId.get(edge.to)?.facts.callable === true));
  }

  /** Lines of the unit's own code: its span minus the spans of the units nested in it. */
  ownLines(unit: UnitRecord): number {
    return unit.lines - unit.nested.reduce((sum, r) => sum + (r.end.line - r.start.line + 1), 0);
  }

  inDirs(file: string, dirs: readonly string[]): boolean {
    return dirs.some((d) => file === d || file.startsWith(d.endsWith('/') ? d : d + '/'));
  }

  /** Every unit reachable from `roots` through the edges `follow` accepts. */
  reach(roots: readonly string[], follow: (edge: EdgeRecord) => boolean): Set<string> {
    const seen = new Set<string>(roots.filter((r) => this.byId.has(r)));
    const queue = [...seen];
    for (let i = 0; i < queue.length; i++) {
      for (const edge of this.outgoing.get(queue[i]!) ?? []) {
        if (!seen.has(edge.to) && follow(edge)) {
          seen.add(edge.to);
          queue.push(edge.to);
        }
      }
    }
    return seen;
  }

  /** Units that schedule animation frames (and the callbacks they pass), plus the roots given with --hot. */
  #hotRoots(): string[] {
    const roots = new Set<string>();
    for (const unit of this.ir.units) {
      if (!unit.facts.external.some((e) => e.key.endsWith(':requestAnimationFrame'))) continue;
      roots.add(unit.id);
      for (const edge of this.outgoing.get(unit.id) ?? []) {
        if (edge.kind === 'reference' && this.byId.get(edge.to)?.facts.callable) roots.add(edge.to);
      }
    }
    for (const spec of this.options.hot) {
      const matched = this.byId.has(spec) ? [spec] : this.ir.units.filter((u) => u.file === spec || u.id.startsWith(spec)).map((u) => u.id);
      if (matched.length === 0) throw new Error(`--hot ${spec} matches no unit (give a unit id, an id prefix or a file)`);
      for (const id of matched) roots.add(id);
    }
    return [...roots].sort();
  }
}

function push(map: Map<string, EdgeRecord[]>, key: string, edge: EdgeRecord): void {
  const list = map.get(key);
  if (list) list.push(edge);
  else map.set(key, [edge]);
}
