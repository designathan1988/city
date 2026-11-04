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
  /** Numbers the target project allows bare (its rules.json magic_numbers.allowed_values); null: no target rules. */
  readonly allowedNumbers: readonly number[] | null;
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

/** How execution flows on from running code: what it calls, constructs, dispatches to, or schedules. */
const HOT_EDGES = new Set<EdgeKind>(['call', 'construct', 'implementation', 'callbackAssign', 'indirectCall', 'schedule']);

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
    this.hot = this.reach(this.hotRoots, (edge) => HOT_EDGES.has(edge.kind));
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

  /**
   * Repeating callbacks (a scheduled function whose own execution schedules it again: the frame loop
   * `shown → frame → request → requestAnimationFrame(shown)`), plus the roots given with --hot. A function scheduled
   * once (a deferral) is not a root.
   */
  #hotRoots(): string[] {
    const roots = new Set<string>();
    const scheduledBy = new Map<string, Set<string>>();
    for (const edge of this.ir.edges) {
      if (edge.kind !== 'schedule') continue;
      const set = scheduledBy.get(edge.to) ?? new Set<string>();
      set.add(edge.from);
      scheduledBy.set(edge.to, set);
    }
    for (const [callback, schedulers] of scheduledBy) {
      const reached = this.reach([callback], (edge) => HOT_EDGES.has(edge.kind));
      if ([...schedulers].some((s) => reached.has(s))) roots.add(callback);
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
