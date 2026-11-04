// The boundary of a feature in the whole program's graph, by dominance: a declaration is the feature's own when the
// program reaches it only through the feature (take the feature out and nothing else uses it). Everything else the
// feature touches is shared: it is the interface the feature uses, not code of the feature. The places outside that
// reach into the feature are where it plugs into the program.
//
// What the program runs: the scripts its HTML pages load (Vite's `<script type="module" src>`), and, for every file
// it reaches, what runs when the file is loaded (its statements, its variables' initializers, its classes' static
// parts: importing a module runs them). Dead code reaches nothing, so it never makes a declaration shared.
import type { EdgeKind, GraphIR, UnitRecord } from '../graph/ir.ts';
import type { Holds } from './holders.ts';

export interface FeatureUse {
  /** The shared declaration used. */
  readonly unit: string;
  readonly kind: EdgeKind;
  /** The feature's declarations that use it. */
  readonly by: string[];
}

export interface FeatureBoundary {
  /** The files the feature was given by. */
  readonly entries: readonly string[];
  /** The scripts the program starts from. */
  readonly roots: readonly string[];
  /** The feature's own declarations. */
  readonly own: readonly string[];
  /** Shared declarations the feature's code uses directly: the interface it needs from the rest of the program. */
  readonly uses: readonly FeatureUse[];
  /** Declarations outside the feature that use it: where the program plugs it in. */
  readonly hooks: readonly FeatureUse[];
  /** External APIs (packages, the platform) the feature's own code uses, with how many uses. */
  readonly external: readonly { readonly key: string; readonly count: number }[];
  /** The program's fields and variables that hold the feature's values (or values made from them, to a fixpoint). */
  readonly holders: readonly string[];
  /** The program's declarations, run by it, that name the feature or a holder: the feature has a slice of each. */
  readonly sliced: readonly string[];
  /** Every declaration the program runs without the feature (not written out: what the rest of the program is). */
  readonly program: ReadonlySet<string>;
}

/**
 * What runs when a module is evaluated (ECMAScript): its statements, the initializers of its variables (one whose value
 * is a function is a unit of kind function: it only makes the function), its classes' static blocks and static fields.
 */
const runsOnLoad = (u: UnitRecord): boolean => u.kind === 'statement' || u.kind === 'staticBlock'
  || (u.kind === 'variable' && u.parent === null) || (u.kind === 'field' && u.facts.isStatic);
/** Declarations that hold a value. */
const HOLDS = new Set(['field', 'variable', 'getter']);

export function featureBoundary(ir: GraphIR, entries: readonly string[], roots: readonly string[], holds: Holds): FeatureBoundary {
  const units = new Map(ir.units.map((u) => [u.id, u]));
  const out = new Map<string, { to: string; kind: EdgeKind }[]>();
  const into = new Map<string, { from: string; kind: EdgeKind }[]>();
  for (const e of ir.edges) {
    if (!units.has(e.from) || !units.has(e.to)) continue;
    (out.get(e.from) ?? out.set(e.from, []).get(e.from)!).push({ to: e.to, kind: e.kind });
    (into.get(e.to) ?? into.set(e.to, []).get(e.to)!).push({ from: e.from, kind: e.kind });
  }
  const byFile = new Map<string, UnitRecord[]>();
  for (const u of ir.units) (byFile.get(u.file) ?? byFile.set(u.file, []).get(u.file)!).push(u);
  const known = new Set(byFile.keys());
  for (const f of [...entries, ...roots]) if (!known.has(f)) throw new Error(`${f} has no declarations in the graph (is it in its scope?)`);

  const seed = new Set(entries.flatMap((f) => byFile.get(f)!.map((u) => u.id)));

  /** Everything reached from `start`, never entering `blocked`; a reached file runs its load-time code. */
  const reach = (start: Iterable<string>, blocked: ReadonlySet<string>): Set<string> => {
    const seen = new Set<string>();
    const loaded = new Set<string>();
    const stack: string[] = [];
    const push = (id: string): void => {
      if (seen.has(id) || blocked.has(id)) return;
      seen.add(id);
      stack.push(id);
    };
    for (const id of start) push(id);
    while (stack.length > 0) {
      const id = stack.pop()!;
      const file = units.get(id)!.file;
      if (!loaded.has(file)) {
        loaded.add(file);
        for (const u of byFile.get(file)!) if (runsOnLoad(u)) push(u.id);
      }
      for (const e of out.get(id) ?? []) push(e.to);
    }
    return seen;
  };

  const rootUnits = roots.flatMap((f) => byFile.get(f)!.filter(runsOnLoad).map((u) => u.id));
  const withoutFeature = reach(rootUnits, seed);
  const fromFeature = reach(seed, new Set());
  const own = new Set([...fromFeature].filter((id) => !withoutFeature.has(id)));

  const group = (pairs: { unit: string; kind: EdgeKind; by: string }[]): FeatureUse[] => {
    const map = new Map<string, FeatureUse>();
    for (const p of pairs) {
      const key = `${p.unit}|${p.kind}`;
      const known = map.get(key);
      if (known) { if (!known.by.includes(p.by)) known.by.push(p.by); } else map.set(key, { unit: p.unit, kind: p.kind, by: [p.by] });
    }
    return [...map.values()].sort((a, b) => a.unit.localeCompare(b.unit) || a.kind.localeCompare(b.kind));
  };
  const uses = group([...own].flatMap((id) => (out.get(id) ?? []).filter((e) => !own.has(e.to)).map((e) => ({ unit: e.to, kind: e.kind, by: id }))));
  // A hook is a place the running program reaches in from: dead code that names the feature plugs nothing in.
  const hooks = group([...own].flatMap((id) => (into.get(id) ?? []).filter((e) => withoutFeature.has(e.from)).map((e) => ({ unit: id, kind: e.kind, by: e.from }))));
  // The program's state that holds the feature's values (holders.ts decides what a stored value is), to a fixpoint.
  const feature = new Set(own);
  const holders = new Set<string>();
  const namesFeature = (id: string): boolean => (out.get(id) ?? []).some((e) => feature.has(e.to));
  for (let changed = true; changed;) {
    changed = false;
    for (const id of withoutFeature) {
      if (holders.has(id) || !HOLDS.has(units.get(id)!.kind)) continue;
      // What it is made from, or what an assignment to it is made from, names the feature; holders.ts decides.
      const written = (into.get(id) ?? []).some((e) => e.kind === 'write' && namesFeature(e.from));
      if (!(namesFeature(id) || written) || !holds(units.get(id)!, feature)) continue;
      holders.add(id);
      feature.add(id);
      changed = true;
    }
  }
  const sliced = [...withoutFeature].filter((id) => namesFeature(id));
  const external = new Map<string, number>();
  for (const id of own) for (const x of units.get(id)!.facts.external) external.set(x.key, (external.get(x.key) ?? 0) + x.count);

  return {
    entries: [...entries],
    roots: [...roots],
    own: [...own].sort(),
    uses,
    hooks,
    external: [...external].map(([key, count]) => ({ key, count })).sort((a, b) => b.count - a.count || a.key.localeCompare(b.key)),
    holders: [...holders].sort(),
    sliced: sliced.sort(),
    program: withoutFeature,
  };
}
